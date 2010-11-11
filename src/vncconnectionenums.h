


#ifndef VNC_CONNECTION_ENUMS_H
#define VNC_CONNECTION_ENUMS_H

G_BEGIN_DECLS


/* --- vncconnection.h --- */
#define VNC_TYPE_CONNECTION_ENCODING vnc_connection_encoding_get_type()
GType vnc_connection_encoding_get_type (void);
#define VNC_TYPE_CONNECTION_AUTH vnc_connection_auth_get_type()
GType vnc_connection_auth_get_type (void);
#define VNC_TYPE_CONNECTION_AUTH_VENCRYPT vnc_connection_auth_vencrypt_get_type()
GType vnc_connection_auth_vencrypt_get_type (void);
#define VNC_TYPE_CONNECTION_CREDENTIAL vnc_connection_credential_get_type()
GType vnc_connection_credential_get_type (void);
G_END_DECLS

#endif /* VNC_CONNECTION_ENUMS_H */



