
/* Generated data (by glib-mkenums) */

#include <glib-object.h>
#include "enums.h"


/* enumerations from "../src/vncdisplay.h" */
#include "../src/vncdisplay.h"
static const GEnumValue _vnc_display_credential_values[] = {
  { VNC_DISPLAY_CREDENTIAL_PASSWORD, "VNC_DISPLAY_CREDENTIAL_PASSWORD", "password" },
  { VNC_DISPLAY_CREDENTIAL_USERNAME, "VNC_DISPLAY_CREDENTIAL_USERNAME", "username" },
  { VNC_DISPLAY_CREDENTIAL_CLIENTNAME, "VNC_DISPLAY_CREDENTIAL_CLIENTNAME", "clientname" },
  { 0, NULL, NULL }
};

GType
vnc_display_credential_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncDisplayCredential", _vnc_display_credential_values);

  return type;
}

static const GEnumValue _vnc_display_key_event_values[] = {
  { VNC_DISPLAY_KEY_EVENT_PRESS, "VNC_DISPLAY_KEY_EVENT_PRESS", "press" },
  { VNC_DISPLAY_KEY_EVENT_RELEASE, "VNC_DISPLAY_KEY_EVENT_RELEASE", "release" },
  { VNC_DISPLAY_KEY_EVENT_CLICK, "VNC_DISPLAY_KEY_EVENT_CLICK", "click" },
  { 0, NULL, NULL }
};

GType
vnc_display_key_event_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncDisplayKeyEvent", _vnc_display_key_event_values);

  return type;
}

static const GEnumValue _vnc_display_depth_color_values[] = {
  { VNC_DISPLAY_DEPTH_COLOR_DEFAULT, "VNC_DISPLAY_DEPTH_COLOR_DEFAULT", "default" },
  { VNC_DISPLAY_DEPTH_COLOR_FULL, "VNC_DISPLAY_DEPTH_COLOR_FULL", "full" },
  { VNC_DISPLAY_DEPTH_COLOR_MEDIUM, "VNC_DISPLAY_DEPTH_COLOR_MEDIUM", "medium" },
  { VNC_DISPLAY_DEPTH_COLOR_LOW, "VNC_DISPLAY_DEPTH_COLOR_LOW", "low" },
  { VNC_DISPLAY_DEPTH_COLOR_ULTRA_LOW, "VNC_DISPLAY_DEPTH_COLOR_ULTRA_LOW", "ultra-low" },
  { 0, NULL, NULL }
};

GType
vnc_display_depth_color_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncDisplayDepthColor", _vnc_display_depth_color_values);

  return type;
}


/* Generated data ends here */

