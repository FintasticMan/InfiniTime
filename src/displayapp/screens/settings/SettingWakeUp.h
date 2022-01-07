#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/screens/ScreenList.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingWakeUp : public Screen {
      public:
        SettingWakeUp(DisplayApp* app, Pinetime::Controllers::Settings& settingsController);
        ~SettingWakeUp() override;

        bool OnTouchEvent(Pinetime::Applications::TouchEvents event) override;

      private:
        Controllers::Settings& settingsController;

        ScreenList<1> screens;

        std::unique_ptr<Screen> CreateScreen1();
      };
    }
  }
}
