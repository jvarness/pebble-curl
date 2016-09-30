#include "fonts.h"

static GFont monoton_large;
static GFont monoton_small;
static Window *window;
static TextLayer *hour_layer;
static TextLayer *minute_layer;
static TextLayer *battery_layer;

static void update_time() {
  time_t now = time(NULL);
  struct tm *local = localtime(&now);
  
  static char hour_buffer[3];
  static char minute_buffer[3];
  
  strftime(hour_buffer, sizeof(hour_buffer), clock_is_24h_style() ? "%H" : "%I", local);
  strftime(minute_buffer, sizeof(minute_buffer), "%M", local);
  
  text_layer_set_text(hour_layer, hour_buffer);
  text_layer_set_text(minute_layer, minute_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_battery(BatteryChargeState battery_state) {
  int charge = battery_state.charge_percent;
  bool is_charging = battery_state.is_charging;
  
  if(is_charging) {
    text_layer_set_text(battery_layer, "+   +");
  }
  else {
    if(charge <= 20) {
      text_layer_set_text(battery_layer, "-   -");
    }
    else {
      text_layer_set_text(battery_layer, "*   *");
    }
  }
}

static void load_window() {
  Layer *win_layer = window_get_root_layer(window);
  GRect dimensions = layer_get_bounds(win_layer);
  
  hour_layer = text_layer_create(GRect(0, dimensions.size.h / 2 - 65, dimensions.size.w, 50));
  text_layer_set_text_alignment(hour_layer, GTextAlignmentCenter);
  text_layer_set_text_color(hour_layer, GColorBlack);
  text_layer_set_background_color(hour_layer, GColorClear);
  text_layer_set_font(hour_layer, monoton_large);
  
  minute_layer = text_layer_create(GRect(0, dimensions.size.h / 2 + 15, dimensions.size.w, 50));
  text_layer_set_text_alignment(minute_layer, GTextAlignmentCenter);
  text_layer_set_text_color(minute_layer, GColorBlack);
  text_layer_set_background_color(minute_layer, GColorClear);
  text_layer_set_font(minute_layer, monoton_large);
  
  battery_layer = text_layer_create(GRect(0, dimensions.size.h / 2 - 15, dimensions.size.w, 50));
  text_layer_set_text_alignment(battery_layer, GTextAlignmentCenter);
  text_layer_set_text_color(battery_layer, GColorBlack);
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_font(battery_layer, monoton_small);
  
  layer_add_child(win_layer, text_layer_get_layer(hour_layer));
  layer_add_child(win_layer, text_layer_get_layer(minute_layer));
  layer_add_child(win_layer, text_layer_get_layer(battery_layer));
  
  update_time();
  update_battery(battery_state_service_peek());
}

static void unload_window() {
  text_layer_destroy(hour_layer);
  text_layer_destroy(minute_layer);
  text_layer_destroy(battery_layer);
  fonts_unload_custom_font(monoton_large);
  fonts_unload_custom_font(monoton_small);
}

static void curl_init() {
  monoton_large = curl_font_monoton_large();
  monoton_small = curl_font_monoton_small();
  
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = load_window,
    .unload = unload_window
  });
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  battery_state_service_subscribe(update_battery);
  
  window_set_background_color(window, GColorWhite);
  window_stack_push(window, false);
}

static void curl_deinit() {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  curl_init();
  app_event_loop();
  curl_deinit();
}