


#include <glib-object.h>
#include "vncconnectionenums.h"


/* enumerations from "vncconnection.h" */
#include "vncconnection.h"
static const GEnumValue _vnc_connection_encoding_values[] = {
  { VNC_CONNECTION_ENCODING_RAW, "VNC_CONNECTION_ENCODING_RAW", "raw" },
  { VNC_CONNECTION_ENCODING_COPY_RECT, "VNC_CONNECTION_ENCODING_COPY_RECT", "copy-rect" },
  { VNC_CONNECTION_ENCODING_RRE, "VNC_CONNECTION_ENCODING_RRE", "rre" },
  { VNC_CONNECTION_ENCODING_CORRE, "VNC_CONNECTION_ENCODING_CORRE", "corre" },
  { VNC_CONNECTION_ENCODING_HEXTILE, "VNC_CONNECTION_ENCODING_HEXTILE", "hextile" },
  { VNC_CONNECTION_ENCODING_TIGHT, "VNC_CONNECTION_ENCODING_TIGHT", "tight" },
  { VNC_CONNECTION_ENCODING_ZRLE, "VNC_CONNECTION_ENCODING_ZRLE", "zrle" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG0, "VNC_CONNECTION_ENCODING_TIGHT_JPEG0", "tight-jpeg0" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG1, "VNC_CONNECTION_ENCODING_TIGHT_JPEG1", "tight-jpeg1" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG2, "VNC_CONNECTION_ENCODING_TIGHT_JPEG2", "tight-jpeg2" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG3, "VNC_CONNECTION_ENCODING_TIGHT_JPEG3", "tight-jpeg3" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG4, "VNC_CONNECTION_ENCODING_TIGHT_JPEG4", "tight-jpeg4" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG5, "VNC_CONNECTION_ENCODING_TIGHT_JPEG5", "tight-jpeg5" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG6, "VNC_CONNECTION_ENCODING_TIGHT_JPEG6", "tight-jpeg6" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG7, "VNC_CONNECTION_ENCODING_TIGHT_JPEG7", "tight-jpeg7" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG8, "VNC_CONNECTION_ENCODING_TIGHT_JPEG8", "tight-jpeg8" },
  { VNC_CONNECTION_ENCODING_TIGHT_JPEG9, "VNC_CONNECTION_ENCODING_TIGHT_JPEG9", "tight-jpeg9" },
  { VNC_CONNECTION_ENCODING_DESKTOP_RESIZE, "VNC_CONNECTION_ENCODING_DESKTOP_RESIZE", "desktop-resize" },
  { VNC_CONNECTION_ENCODING_WMVi, "VNC_CONNECTION_ENCODING_WMVi", "wmvi" },
  { VNC_CONNECTION_ENCODING_CURSOR_POS, "VNC_CONNECTION_ENCODING_CURSOR_POS", "cursor-pos" },
  { VNC_CONNECTION_ENCODING_RICH_CURSOR, "VNC_CONNECTION_ENCODING_RICH_CURSOR", "rich-cursor" },
  { VNC_CONNECTION_ENCODING_XCURSOR, "VNC_CONNECTION_ENCODING_XCURSOR", "xcursor" },
  { VNC_CONNECTION_ENCODING_POINTER_CHANGE, "VNC_CONNECTION_ENCODING_POINTER_CHANGE", "pointer-change" },
  { VNC_CONNECTION_ENCODING_EXT_KEY_EVENT, "VNC_CONNECTION_ENCODING_EXT_KEY_EVENT", "ext-key-event" },
  { 0, NULL, NULL }
};

GType
vnc_connection_encoding_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncConnectionEncoding", _vnc_connection_encoding_values);

  return type;
}

static const GEnumValue _vnc_connection_auth_values[] = {
  { VNC_CONNECTION_AUTH_INVALID, "VNC_CONNECTION_AUTH_INVALID", "invalid" },
  { VNC_CONNECTION_AUTH_NONE, "VNC_CONNECTION_AUTH_NONE", "none" },
  { VNC_CONNECTION_AUTH_VNC, "VNC_CONNECTION_AUTH_VNC", "vnc" },
  { VNC_CONNECTION_AUTH_RA2, "VNC_CONNECTION_AUTH_RA2", "ra2" },
  { VNC_CONNECTION_AUTH_RA2NE, "VNC_CONNECTION_AUTH_RA2NE", "ra2ne" },
  { VNC_CONNECTION_AUTH_TIGHT, "VNC_CONNECTION_AUTH_TIGHT", "tight" },
  { VNC_CONNECTION_AUTH_ULTRA, "VNC_CONNECTION_AUTH_ULTRA", "ultra" },
  { VNC_CONNECTION_AUTH_TLS, "VNC_CONNECTION_AUTH_TLS", "tls" },
  { VNC_CONNECTION_AUTH_VENCRYPT, "VNC_CONNECTION_AUTH_VENCRYPT", "vencrypt" },
  { VNC_CONNECTION_AUTH_SASL, "VNC_CONNECTION_AUTH_SASL", "sasl" },
  { VNC_CONNECTION_AUTH_ARD, "VNC_CONNECTION_AUTH_ARD", "ard" },
  { VNC_CONNECTION_AUTH_MSLOGON, "VNC_CONNECTION_AUTH_MSLOGON", "mslogon" },
  { 0, NULL, NULL }
};

GType
vnc_connection_auth_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncConnectionAuth", _vnc_connection_auth_values);

  return type;
}

static const GEnumValue _vnc_connection_auth_vencrypt_values[] = {
  { VNC_CONNECTION_AUTH_VENCRYPT_PLAIN, "VNC_CONNECTION_AUTH_VENCRYPT_PLAIN", "plain" },
  { VNC_CONNECTION_AUTH_VENCRYPT_TLSNONE, "VNC_CONNECTION_AUTH_VENCRYPT_TLSNONE", "tlsnone" },
  { VNC_CONNECTION_AUTH_VENCRYPT_TLSVNC, "VNC_CONNECTION_AUTH_VENCRYPT_TLSVNC", "tlsvnc" },
  { VNC_CONNECTION_AUTH_VENCRYPT_TLSPLAIN, "VNC_CONNECTION_AUTH_VENCRYPT_TLSPLAIN", "tlsplain" },
  { VNC_CONNECTION_AUTH_VENCRYPT_X509NONE, "VNC_CONNECTION_AUTH_VENCRYPT_X509NONE", "x509none" },
  { VNC_CONNECTION_AUTH_VENCRYPT_X509VNC, "VNC_CONNECTION_AUTH_VENCRYPT_X509VNC", "x509vnc" },
  { VNC_CONNECTION_AUTH_VENCRYPT_X509PLAIN, "VNC_CONNECTION_AUTH_VENCRYPT_X509PLAIN", "x509plain" },
  { VNC_CONNECTION_AUTH_VENCRYPT_X509SASL, "VNC_CONNECTION_AUTH_VENCRYPT_X509SASL", "x509sasl" },
  { VNC_CONNECTION_AUTH_VENCRYPT_TLSSASL, "VNC_CONNECTION_AUTH_VENCRYPT_TLSSASL", "tlssasl" },
  { 0, NULL, NULL }
};

GType
vnc_connection_auth_vencrypt_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncConnectionAuthVencrypt", _vnc_connection_auth_vencrypt_values);

  return type;
}

static const GEnumValue _vnc_connection_credential_values[] = {
  { VNC_CONNECTION_CREDENTIAL_PASSWORD, "VNC_CONNECTION_CREDENTIAL_PASSWORD", "password" },
  { VNC_CONNECTION_CREDENTIAL_USERNAME, "VNC_CONNECTION_CREDENTIAL_USERNAME", "username" },
  { VNC_CONNECTION_CREDENTIAL_CLIENTNAME, "VNC_CONNECTION_CREDENTIAL_CLIENTNAME", "clientname" },
  { 0, NULL, NULL }
};

GType
vnc_connection_credential_get_type (void)
{
  static GType type = 0;

  if (!type)
    type = g_enum_register_static ("VncConnectionCredential", _vnc_connection_credential_values);

  return type;
}



