#include "components/motion/MotionController.h"

#include <task.h>

using namespace Pinetime::Controllers;

namespace {
  // approximation of 1024*sin(x) between -45 and 45 degrees
  constexpr int16_t DegreesToAccel(int16_t degrees) {
    // if (degrees < -30) {
    //   return degrees * 12 - 180;
    // }
    // if (degrees > 30) {
    //   return degrees * 12 + 180;
    // }
    // return degrees * 18;
    return degrees * 17;
  }

  // approximation of asin(x/1024) between -45 and 45 degrees
  constexpr int16_t AccelToDegrees(int16_t accel) {
    // if (accel < -540) {
    //   return accel / 12 + 15;
    // }
    // if (accel > 540) {
    //   return accel / 12 - 15;
    // }
    // return accel / 18;
    return accel / 17;
  }
}

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

int16_t MotionController::DegreesRolled() const {
  if (y < DegreesToAccel(-45)) {
    if (lastY < DegreesToAccel(-45)) {
      return AccelToDegrees(z) - AccelToDegrees(lastZ);
    }

    return AccelToDegrees(lastY) + AccelToDegrees(z) + 90;
  }

  return AccelToDegrees(lastY) - AccelToDegrees(y);
}

bool MotionController::ShouldRaiseWake() const {
  if (x < DegreesToAccel(-22) || x > DegreesToAccel(22) || y > DegreesToAccel(-8) || z > DegreesToAccel(45)) {
    return false;
  }

  return DegreesRolled() > static_cast<int16_t>(time - lastTime) / 6;
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
