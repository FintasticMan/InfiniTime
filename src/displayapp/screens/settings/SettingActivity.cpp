#include "displayapp/screens/settings/SettingActivity.h"

#include <cstdint>
#include <cinttypes>

#include "displayapp/DisplayApp.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

namespace {
  void EventHandler(lv_obj_t* obj, lv_event_t event) {
    SettingActivity* screen = static_cast<SettingActivity*>(obj->user_data);
    screen->UpdateSelected(obj, event);
  }
}

SettingActivity::SettingActivity(Controllers::Settings& settingsController) : settingsController {settingsController} {
  lv_obj_t* container = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_pos(container, 30, 60);
  lv_obj_set_width(container, LV_HOR_RES - 50);
  lv_obj_set_height(container, LV_VER_RES - 60);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Activity notifications");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

  lv_label_set_text_static(icon, Symbols::shoe);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  enabled = lv_checkbox_create(container, nullptr);
  lv_checkbox_set_text(enabled, "Enabled");
  if (settingsController.GetActivity() == Controllers::Settings::Activity::On) {
    lv_checkbox_set_checked(enabled, true);
  }
  enabled->user_data = this;
  lv_obj_set_event_cb(enabled, EventHandler);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_label_set_text_fmt(stepValue, "%" PRIu32, settingsController.GetActivityThresh());
  lv_label_set_align(stepValue, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_CENTER, 0, 20);

  btnPlus = lv_btn_create(lv_scr_act(), nullptr);
  btnPlus->user_data = this;
  lv_obj_set_size(btnPlus, 80, 50);
  lv_obj_align(btnPlus, lv_scr_act(), LV_ALIGN_CENTER, 55, 80);
  lv_obj_t* lblPlus = lv_label_create(btnPlus, nullptr);
  lv_label_set_text_static(lblPlus, "+");
  lv_obj_set_event_cb(btnPlus, EventHandler);

  btnMinus = lv_btn_create(lv_scr_act(), nullptr);
  btnMinus->user_data = this;
  lv_obj_set_size(btnMinus, 80, 50);
  lv_obj_set_event_cb(btnMinus, EventHandler);
  lv_obj_align(btnMinus, lv_scr_act(), LV_ALIGN_CENTER, -55, 80);
  lv_obj_t* lblMinus = lv_label_create(btnMinus, nullptr);
  lv_label_set_text_static(lblMinus, "-");
}

SettingActivity::~SettingActivity() {
  lv_obj_clean(lv_scr_act());
  settingsController.SaveSettings();
}

void SettingActivity::UpdateSelected(lv_obj_t* object, lv_event_t event) {
  uint16_t value = settingsController.GetActivityThresh();
  if (object == btnPlus && event == LV_EVENT_PRESSED) {
    value += 1;
    if (value <= 15000) {
      settingsController.SetActivityThresh(value);
      lv_label_set_text_fmt(stepValue, "%" PRIu16, settingsController.GetActivityThresh());
      lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_CENTER, 0, 20);
    }
  }

  if (object == btnMinus && event == LV_EVENT_PRESSED) {
    value -= 1;
    if (value >= 1) {
      settingsController.SetActivityThresh(value);
      lv_label_set_text_fmt(stepValue, "%" PRIu16, settingsController.GetActivityThresh());
      lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_CENTER, 0, 20);
    }
  }

  if (object == enabled && event == LV_EVENT_VALUE_CHANGED) {
    bool currentState = settingsController.GetActivity() == Controllers::Settings::Activity::On;
    settingsController.SetActivity(currentState ? Controllers::Settings::Activity::Off : Controllers::Settings::Activity::On);
    lv_checkbox_set_checked(enabled, !currentState);
  }
}