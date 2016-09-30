#include "fonts.h"

static GFont curl_get_font(ResHandle handle) {
  return fonts_load_custom_font(handle);
}

GFont curl_font_monoton_large() {
  return curl_get_font(resource_get_handle(RESOURCE_ID_FONT_MONOTON_48));
}

GFont curl_font_monoton_small() {
  return curl_get_font(resource_get_handle(RESOURCE_ID_FONT_MONOTON_30));
}