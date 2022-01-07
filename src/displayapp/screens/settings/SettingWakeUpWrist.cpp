#include "displayapp/screens/settings/SettingWakeUpWrist.h"
#include <lvgl/lvgl.h>
#include "displayapp/DisplayApp.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/screens/Styles.h"
#include "displayapp/screens/Symbols.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

namespace {
  static void event_handler(lv_obj_t* obj, lv_event_t event) {
    SettingWakeUpWrist* screen = static_cast<SettingWakeUpWrist*>(obj->user_data);
    screen->UpdateSelected(obj, event);
  }
}

SettingWakeUpWrist::SettingWakeUpWrist(Pinetime::Applications::DisplayApp* app, Pinetime::Controllers::Settings& settingsController)
  : Screen(app), settingsController {settingsController} {
  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 10, 60);
  lv_obj_set_width(container1, LV_HOR_RES - 20);
  lv_obj_set_height(container1, LV_VER_RES - 50);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Wake Up Wrist");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, Symbols::eye);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  optionsTotal = 0;
  cbOption[optionsTotal] = lv_checkbox_create(container1, nullptr);
  lv_checkbox_set_text_static(cbOption[optionsTotal], " Off");
  cbOption[optionsTotal]->user_data = this;
  lv_obj_set_event_cb(cbOption[optionsTotal], event_handler);
  SetRadioButtonStyle(cbOption[optionsTotal]);
  if (!settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::RaiseWrist) &&
      !settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::Shake)) {
    lv_checkbox_set_checked(cbOption[optionsTotal], true);
  }
  optionsTotal++;
  cbOption[optionsTotal] = lv_checkbox_create(container1, nullptr);
  lv_checkbox_set_text_static(cbOption[optionsTotal], " Raise Wrist");
  cbOption[optionsTotal]->user_data = this;
  lv_obj_set_event_cb(cbOption[optionsTotal], event_handler);
  SetRadioButtonStyle(cbOption[optionsTotal]);
  if (settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::RaiseWrist)) {
    lv_checkbox_set_checked(cbOption[optionsTotal], true);
  }
  optionsTotal++;
  cbOption[optionsTotal] = lv_checkbox_create(container1, nullptr);
  lv_checkbox_set_text_static(cbOption[optionsTotal], " Shake Wrist");
  cbOption[optionsTotal]->user_data = this;
  lv_obj_set_event_cb(cbOption[optionsTotal], event_handler);
  SetRadioButtonStyle(cbOption[optionsTotal]);
  if (settingsController.isWakeUpModeOn(Pinetime::Controllers::Settings::WakeUpMode::Shake)) {
    lv_checkbox_set_checked(cbOption[optionsTotal], true);
  }
  optionsTotal++;
}

SettingWakeUpWrist::~SettingWakeUpWrist() {
  lv_obj_clean(lv_scr_act());
  settingsController.SaveSettings();
}

void SettingWakeUpWrist::UpdateSelected(lv_obj_t* object, lv_event_t event) {
  using WakeUpMode = Pinetime::Controllers::Settings::WakeUpMode;
  if (event == LV_EVENT_VALUE_CHANGED) {
    // Find the index of the checkbox that triggered the event
    for (int index = 0; index < optionsTotal; ++index) {
      if (cbOption[index] == object) {
        lv_checkbox_set_checked(cbOption[index], true);
        switch (index) {
          case 0:
            settingsController.setWakeUpMode(WakeUpMode::RaiseWrist, false);
            settingsController.setWakeUpMode(WakeUpMode::Shake, false);
            break;
          case 1:
            settingsController.setWakeUpMode(WakeUpMode::RaiseWrist, true);
            settingsController.setWakeUpMode(WakeUpMode::Shake, false);
            break;
          case 2:
            settingsController.setWakeUpMode(WakeUpMode::RaiseWrist, false);
            settingsController.setWakeUpMode(WakeUpMode::Shake, true);
            break;
        }
      } else {
        lv_checkbox_set_checked(cbOption[index], false);
      }
    }
  }
}
