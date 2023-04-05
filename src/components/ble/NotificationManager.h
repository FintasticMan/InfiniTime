#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "utility/CircularBuffer.h"

namespace Pinetime {
  namespace Controllers {
    class NotificationManager {
    public:
      enum class Categories {
        Unknown,
        SimpleAlert,
        Email,
        News,
        IncomingCall,
        MissedCall,
        Sms,
        VoiceMail,
        Schedule,
        HighProriotyAlert,
        InstantMessage
      };
      static constexpr uint8_t MessageSize {100};

      struct Notification {
        using Id = uint8_t;
        using Idx = uint8_t;
        Id id = 0;
        bool valid = false;
        uint8_t size;
        std::array<char, MessageSize + 1> message;
        Categories category = Categories::Unknown;

        const char* Message() const;
        const char* Title() const;
      };

      void Push(Notification&& notif);
      Notification GetLastNotification() const;
      Notification Get(Notification::Id id) const;
      Notification GetNext(Notification::Id id) const;
      Notification GetPrevious(Notification::Id id) const;
      // Return the index of the notification with the specified id, if not found return NbNotifications()
      Notification::Idx IndexOf(Notification::Id id) const;
      bool ClearNewNotificationFlag();
      bool AreNewNotificationsAvailable() const;
      void Dismiss(Notification::Id id);

      static constexpr size_t MaximumMessageSize() {
        return MessageSize;
      };

      bool IsEmpty() const {
        return size == 0;
      }

      size_t NbNotifications() const;

    private:
      Notification::Id nextId {0};
      Notification::Id GetNextId();
      void DismissIdx(Notification::Idx idx);

      Utility::CircularBuffer<Notification, 5> notifications;
      size_t size = 0; // number of valid notifications in buffer

      std::atomic<bool> newNotification {false};
    };
  }
}
