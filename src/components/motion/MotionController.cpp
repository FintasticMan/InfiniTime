#include "components/motion/MotionController.h"

#include <task.h>

using namespace Pinetime::Controllers;

void MotionController::Update(int16_t x, int16_t y, int16_t z, uint32_t nbSteps) {
  if (this->nbSteps != nbSteps && service != nullptr) {
    service->OnNewStepCountValue(nbSteps);
  }

  if (service != nullptr && (this->x != x || this->y != y || this->z != z)) {
    service->OnNewMotionValues(x, y, z);
  }

  lastTime = time;
  time = xTaskGetTickCount();

  this->x = x;
  lastY = this->y;
  this->y = y;
  lastZ = this->z;
  this->z = z;
  int32_t deltaSteps = nbSteps - this->nbSteps;
  this->nbSteps = nbSteps;
  if (deltaSteps > 0) {
    currentTripSteps += deltaSteps;
  }
}

int16_t MotionController::AmountRolled() const {
  // 724 is 1024*sin(PI/4)
  if (y < -724) {
    if (lastY < -724) {
      return lastZ + z;
    }

    return lastY + z + 1148;
  }

  return lastY - y;
}

bool MotionController::ShouldRaiseWake() const {
  if (x < -384 || x > 384 || y > -128 || z > 724) {
    return false;
  }

  return AmountRolled() > static_cast<int16_t>(time - lastTime) * 3;
}

bool MotionController::ShouldShakeWake(uint16_t thresh) {
  /* Currently Polling at 10hz, If this ever goes faster scalar and EMA might need adjusting */
  int32_t speed = std::abs(z + (y / 2) + (x / 4) - lastY / 2 - lastZ) / (time - lastTime) * 100;
  //(.2 * speed) + ((1 - .2) * accumulatedSpeed);
  // implemented without floats as .25Alpha
  accumulatedSpeed = (speed / 5) + ((accumulatedSpeed / 5) * 4);

  return accumulatedSpeed > thresh;
}

void MotionController::Init(Pinetime::Drivers::Bma421::DeviceTypes types) {
  switch (types) {
    case Drivers::Bma421::DeviceTypes::BMA421:
      this->deviceType = DeviceTypes::BMA421;
      break;
    case Drivers::Bma421::DeviceTypes::BMA425:
      this->deviceType = DeviceTypes::BMA425;
      break;
    default:
      this->deviceType = DeviceTypes::Unknown;
      break;
  }
}

void MotionController::SetService(Pinetime::Controllers::MotionService* service) {
  this->service = service;
}
