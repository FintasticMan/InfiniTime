#pragma once

#include <cstdint>
#include <lvgl/lvgl.h>
#include "components/settings/Settings.h"
#include "displayapp/screens/Screen.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingWakeUpTap : public Screen {
      public:
        SettingWakeUpTap(DisplayApp* app, Pinetime::Controllers::Settings& settingsController);
        ~SettingWakeUpTap() override;

        void UpdateSelected(lv_obj_t* object, lv_event_t event);

      private:
        Controllers::Settings& settingsController;
        uint8_t optionsTotal;
        lv_obj_t* cbOption[3];
      };
    }
  }
}
