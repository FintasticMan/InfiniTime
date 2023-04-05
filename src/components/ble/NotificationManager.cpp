#include "components/ble/NotificationManager.h"
#include <cstring>
#include <algorithm>
#include <cassert>

using namespace Pinetime::Controllers;

constexpr uint8_t NotificationManager::MessageSize;

void NotificationManager::Push(NotificationManager::Notification&& notif) {
  notif.id = GetNextId();
  notif.valid = true;
  newNotification = true;
  notifications--;
  notifications[0] = std::move(notif);
  if (size < notifications.Size()) {
    size++;
  }
}

NotificationManager::Notification::Id NotificationManager::GetNextId() {
  return nextId++;
}

NotificationManager::Notification NotificationManager::GetLastNotification() const {
  if (this->IsEmpty()) {
    return {};
  }
  return notifications[0];
}

NotificationManager::Notification::Idx NotificationManager::IndexOf(NotificationManager::Notification::Id id) const {
  for (NotificationManager::Notification::Idx idx = 0; idx < this->size; idx++) {
    const NotificationManager::Notification& notification = notifications[idx];
    if (notification.id == id) {
      return idx;
    }
  }
  return size;
}

NotificationManager::Notification NotificationManager::Get(NotificationManager::Notification::Id id) const {
  NotificationManager::Notification::Idx idx = this->IndexOf(id);
  if (idx == this->size) {
    return {};
  }
  return notifications[idx];
}

NotificationManager::Notification NotificationManager::GetNext(NotificationManager::Notification::Id id) const {
  NotificationManager::Notification::Idx idx = this->IndexOf(id);
  if (idx == this->size) {
    return {};
  }
  if (idx == 0 || idx > notifications.Size()) {
    return {};
  }
  return notifications[idx - 1];
}

NotificationManager::Notification NotificationManager::GetPrevious(NotificationManager::Notification::Id id) const {
  NotificationManager::Notification::Idx idx = this->IndexOf(id);
  if (idx == this->size) {
    return {};
  }
  if (idx + 1u >= notifications.Size()) {
    return {};
  }
  return notifications[idx + 1];
}

void NotificationManager::DismissIdx(NotificationManager::Notification::Idx idx) {
  if (this->IsEmpty()) {
    return;
  }
  if (idx >= size) {
    assert(false);
    return; // this should not happen
  }
  if (idx == 0) { // just remove the first element, don't need to change the other elements
    notifications[0].valid = false;
    notifications++;
  } else {
    // overwrite the specified entry by moving all later messages one index to the front
    for (size_t i = idx; i < size - 1; ++i) {
      notifications[i] = notifications[i + 1];
    }
    notifications[size - 1].valid = false;
  }
  --size;
}

void NotificationManager::Dismiss(NotificationManager::Notification::Id id) {
  NotificationManager::Notification::Idx idx = this->IndexOf(id);
  if (idx == this->size) {
    return;
  }
  this->DismissIdx(idx);
}

bool NotificationManager::AreNewNotificationsAvailable() const {
  return newNotification;
}

bool NotificationManager::ClearNewNotificationFlag() {
  return newNotification.exchange(false);
}

size_t NotificationManager::NbNotifications() const {
  return size;
}

const char* NotificationManager::Notification::Message() const {
  const char* itField = std::find(message.begin(), message.begin() + size - 1, '\0');
  if (itField != message.begin() + size - 1) {
    const char* ptr = (itField) + 1;
    return ptr;
  }
  return const_cast<char*>(message.data());
}

const char* NotificationManager::Notification::Title() const {
  const char* itField = std::find(message.begin(), message.begin() + size - 1, '\0');
  if (itField != message.begin() + size - 1) {
    return message.data();
  }
  return {};
}
