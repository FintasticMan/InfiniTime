#include "displayapp/screens/settings/SettingWakeUp.h"
#include <lvgl/lvgl.h>
#include <array>
#include "displayapp/screens/List.h"
#include "displayapp/Apps.h"
#include "displayapp/DisplayApp.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingWakeUp::SettingWakeUp(Pinetime::Applications::DisplayApp* app, Pinetime::Controllers::Settings& settingsController)
  : Screen(app),
    settingsController {settingsController},
    screens {app,
             settingsController.GetSettingsMenu(),
             {[this]() -> std::unique_ptr<Screen> {
                return CreateScreen1();
              }},
             Screens::ScreenListModes::UpDown} {
}

SettingWakeUp::~SettingWakeUp() {
  lv_obj_clean(lv_scr_act());
}

bool SettingWakeUp::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

std::unique_ptr<Screen> SettingWakeUp::CreateScreen1() {

  std::array<Screens::List::Applications, 4> applications {{
    {Symbols::none, "Tap", Apps::SettingWakeUpTap},
    {Symbols::none, "Wrist", Apps::SettingWakeUpWrist},
    {Symbols::none, "None", Apps::None},
    {Symbols::none, "None", Apps::None}
  }};

  return std::make_unique<Screens::List>(0, 1, app, settingsController, applications);
}
