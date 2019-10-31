/*
 * GTK VNC Widget
 *
 * Copyright (C) 2006  Anthony Liguori <anthony@codemonkey.ws>
 * Copyright (C) 2009-2010 Daniel P. Berrange <dan@berrange.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include <string.h>
#include <stdlib.h>

#include "vncconnection.h"
#include "vncbaseframebuffer.h"

static gboolean debug;
static gboolean allowfail;

struct GVncTest {
    GMutex lock;
    GMutex clock;
    GCond cond;
    int port;
    VncConnection *conn;
    GMainLoop *loop;
    gboolean connected;
    gboolean quit;
    char *error;

    guint8 *pixels;

    void (*test_func)(GInputStream *, GOutputStream *);
};


static void test_send_bytes(GOutputStream *os, const guint8 *str, gsize len)
{
    g_assert(g_output_stream_write_all(os, str, len, NULL, NULL, NULL) || allowfail);
}

static void test_send_u8(GOutputStream *os, guint8 v)
{
    g_assert(g_output_stream_write_all(os, &v, 1, NULL, NULL, NULL) || allowfail);
}

static void test_send_u16(GOutputStream *os, guint16 v)
{
    v = GUINT16_TO_BE(v);
    g_assert(g_output_stream_write_all(os, &v, 2, NULL, NULL, NULL) || allowfail);
}

static void test_send_u32(GOutputStream *os, guint32 v)
{
    v = GUINT32_TO_BE(v);
    g_assert(g_output_stream_write_all(os, &v, 4, NULL, NULL, NULL) || allowfail);
}

static void test_send_s32(GOutputStream *os, gint32 v)
{
    v = GINT32_TO_BE(v);
    g_assert(g_output_stream_write_all(os, &v, 4, NULL, NULL, NULL) || allowfail);
}

static void test_recv_bytes(GInputStream *is, guint8 *str, gsize len)
{
    g_assert(g_input_stream_read_all(is, str, len, NULL, NULL, NULL));
}

static void test_recv_u8(GInputStream *is, guint8 v)
{
    guint8 e;
    g_assert(g_input_stream_read_all(is, &e, 1, NULL, NULL, NULL));
    g_assert(e == v);
}

static void test_recv_u16(GInputStream *is, guint16 v)
{
    guint16 e;
    g_assert(g_input_stream_read_all(is, &e, 2, NULL, NULL, NULL));
    e = GINT16_FROM_BE(e);
    g_assert(e == v);
}

static void test_recv_s32(GInputStream *is, gint32 v)
{
    gint32 e;
    g_assert(g_input_stream_read_all(is, &e, 4, NULL, NULL, NULL));
    e = GINT32_FROM_BE(e);
    g_assert(e == v);
}


static gpointer test_helper_server(gpointer opaque)
{
    struct GVncTest *data = opaque;
    GSocketListener *server;
    GSocketConnection *client;
    GIOStream *ios;
    GInputStream *is;
    GOutputStream *os;

    server = g_socket_listener_new();

    data->port = g_socket_listener_add_any_inet_port(server, NULL, NULL);
    g_mutex_unlock(&data->lock);

    client = g_socket_listener_accept(server, NULL, NULL, NULL);

    ios = G_IO_STREAM(client);
    is = g_io_stream_get_input_stream(ios);
    os = g_io_stream_get_output_stream(ios);

    guint8 greeting[] = {
        'R', 'F', 'B', ' ',
        '0', '0', '3', '.',
        '0', '0', '8', '\n',
    };

    /* Greeting */
    test_send_bytes(os, greeting, G_N_ELEMENTS(greeting));
    test_recv_bytes(is, greeting, G_N_ELEMENTS(greeting));

    /* N auth */
    test_send_u8(os, 1);
    /* auth == none */
    test_send_u8(os, 1);
    test_recv_u8(is, 1);

    /* auth result */
    test_send_u32(os, 0);

    /* shared flag */
    test_recv_u8(is, 0);

    data->test_func(is, os);

    g_mutex_lock(&data->clock);
    while (!data->quit) {
        g_cond_wait(&data->cond, &data->clock);
    }

    g_object_unref(client);
    g_object_unref(server);

    return NULL;
}

static void test_helper_desktop_resize(VncConnection *conn,
                                       int width, int height,
                                       gpointer opaque)
{
    struct GVncTest *test = opaque;
    const VncPixelFormat *remoteFormat;
    VncPixelFormat localFormat = {
        .bits_per_pixel = 32,
        .depth = 32,
        .byte_order = G_BYTE_ORDER,
        .true_color_flag = TRUE,
        .red_max = 255,
        .green_max = 255,
        .blue_max = 255,
        .red_shift = 0,
        .green_shift = 8,
        .blue_shift = 16,
    };
    VncBaseFramebuffer *fb;


    VNC_DEBUG("Resize %dx%d", width, height);
    remoteFormat = vnc_connection_get_pixel_format(conn);

    /* We'll fix our local copy as rgb888 */
    test->pixels = g_new0(guint8, width * height * 4);

    fb = vnc_base_framebuffer_new(test->pixels, width, height, width * 4,
                                  &localFormat, remoteFormat);

    vnc_connection_set_framebuffer(conn, VNC_FRAMEBUFFER(fb));

    g_object_unref(fb);
}


static void test_helper_initialized(VncConnection *conn,
                                    gpointer opaque)
{
    struct GVncTest *test = opaque;
    gint32 encodings[] = {  VNC_CONNECTION_ENCODING_ZRLE,
                            VNC_CONNECTION_ENCODING_HEXTILE,
                            VNC_CONNECTION_ENCODING_RRE,
                            VNC_CONNECTION_ENCODING_COPY_RECT,
                            VNC_CONNECTION_ENCODING_RAW };
    gint32 *encodingsp;
    int n_encodings;

    test_helper_desktop_resize(conn,
                               vnc_connection_get_width(conn),
                               vnc_connection_get_height(conn),
                               test);

    encodingsp = encodings;
    n_encodings = G_N_ELEMENTS(encodings);

    VNC_DEBUG("Sending %d encodings", n_encodings);
    if (!vnc_connection_set_encodings(conn, n_encodings, encodingsp))
        goto error;

    VNC_DEBUG("Requesting first framebuffer update");
    if (!vnc_connection_framebuffer_update_request(test->conn,
                                                   0, 0, 0,
                                                   vnc_connection_get_width(test->conn),
                                                   vnc_connection_get_height(test->conn)))
        vnc_connection_shutdown(test->conn);

    test->connected = TRUE;
    return;

 error:
    vnc_connection_shutdown(conn);
}

static void test_helper_auth_choose_type(VncConnection *conn,
                                         GValueArray *types G_GNUC_UNUSED,
                                         gpointer opaque G_GNUC_UNUSED)
{
    vnc_connection_set_auth_type(conn, VNC_CONNECTION_AUTH_NONE);
}


static void test_helper_disconnected(VncConnection *conn G_GNUC_UNUSED,
                                     gpointer opaque)
{
    struct GVncTest *test = opaque;
    g_main_quit(test->loop);
}

static void test_helper_error(VncConnection *conn G_GNUC_UNUSED,
                              const char *str,
                              gpointer opaque)
{
    struct GVncTest *test = opaque;
    test->error = g_strdup(str);
}

static void test_common_bounds_server(GInputStream *is G_GNUC_UNUSED,
                                      GOutputStream *os)
{
    /* Frame buffer width / height */
    test_send_u16(os, 100);
    test_send_u16(os, 100);

    /* BPP, depth, endian, true color */
    test_send_u8(os, 32);
    test_send_u8(os, 8);
    test_send_u8(os, 1);
    test_send_u8(os, 1);

    /* RGB max + shift*/
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u8(os, 0);
    test_send_u8(os, 8);
    test_send_u8(os, 16);

    guint8 pad[3] = {0};
    test_send_bytes(os, pad, G_N_ELEMENTS(pad));

    /* name */
    guint8 name[] = { 'T', 'e', 's', 't' };
    test_send_u32(os, G_N_ELEMENTS(name));
    test_send_bytes(os, name, G_N_ELEMENTS(name));
}

static void test_rre_bounds_server(GInputStream *is, GOutputStream *os)
{
    test_common_bounds_server(is, os);

    /* Message type & pad */
    test_send_u8(os, 0);
    test_send_u8(os, 0);

    /* num rect */
    test_send_u16(os, 1);
    /* x, y, w, h */
    test_send_u16(os, 90);
    test_send_u16(os, 90);
    test_send_u16(os, 10);
    test_send_u16(os, 10);

    /* encoding=rre */
    test_send_s32(os, 2);

    /* num rect */
    test_send_u32(os, 1);

    /* bg pix, fg pix */
    test_send_u32(os, 0x41414141);
    test_send_u32(os, 0x42424242);

    /* x, y, w, h */
    allowfail = TRUE;
    test_send_u16(os, 10);
    test_send_u16(os, 10000);
    test_send_u16(os, 1);
    test_send_u16(os, 1);
    allowfail = FALSE;
}


static void test_hextile_bounds_server(GInputStream *is, GOutputStream *os)
{
    test_common_bounds_server(is, os);

    /* Message type & pad */
    test_send_u8(os, 0);
    test_send_u8(os, 0);

    /* num rect */
    test_send_u16(os, 1);
    /* x, y, w, h */
    test_send_u16(os, 90);
    test_send_u16(os, 90);
    test_send_u16(os, 10);
    test_send_u16(os, 10);

    /* encoding=hextile */
    test_send_s32(os, 5);

    /* tile type */
    test_send_u8(os, 0x18);

    /* num rect */
    test_send_u8(os, 1);

    /* fg pix */
    test_send_u32(os, 0x12345678);

    /* x, y */
    allowfail = TRUE;
    test_send_u8(os, 0xff);
    test_send_u8(os, 0xff);
    allowfail = FALSE;
}


static void test_copyrect_bounds_server(GInputStream *is, GOutputStream *os)
{
    test_common_bounds_server(is, os);

    /* Message type & pad */
    test_send_u8(os, 0);
    test_send_u8(os, 0);

    /* num rect */
    test_send_u16(os, 1);
    /* x, y, w, h */
    test_send_u16(os, 90);
    test_send_u16(os, 90);
    test_send_u16(os, 10);
    test_send_u16(os, 10);

    /* encoding=copyrect */
    test_send_s32(os, 1);

    /* src x, y */
    allowfail = TRUE;
    test_send_u16(os, 91);
    test_send_u16(os, 91);
    allowfail = FALSE;
}


static void test_unexpected_cmap_server(GInputStream *is, GOutputStream *os)
{
    /* Frame buffer width / height */
    test_send_u16(os, 100);
    test_send_u16(os, 100);

    /* BPP, depth, endian, true color */
    test_send_u8(os, 32);
    test_send_u8(os, 8);
    test_send_u8(os, 1);
    test_send_u8(os, 1);

    /* RGB max + shift*/
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u8(os, 0);
    test_send_u8(os, 8);
    test_send_u8(os, 16);

    guint8 pad[3] = {0};
    test_send_bytes(os, pad, G_N_ELEMENTS(pad));

    /* name */
    guint8 name[] = { 'T', 'e', 's', 't' };
    test_send_u32(os, G_N_ELEMENTS(name));
    test_send_bytes(os, name, G_N_ELEMENTS(name));

    /* n-encodings */
    test_recv_u8(is, 2);
    /* pad */
    test_recv_u8(is, 0);
    /* num encodings */
    test_recv_u16(is, 5);

    /* encodings */
    test_recv_s32(is, 16);
    test_recv_s32(is, 5);
    test_recv_s32(is, 2);
    test_recv_s32(is, 1);
    test_recv_s32(is, 0);

    /* update request */
    test_recv_u8(is, 3);
    /* ! incremental */
    test_recv_u8(is, 0);

    /* x, y, w, h */
    test_recv_u16(is, 0);
    test_recv_u16(is, 0);
    test_recv_u16(is, 100);
    test_recv_u16(is, 100);

    /* set color map */
    test_send_u8(os, 1);
    /* pad */
    test_send_u8(os, 0);

    allowfail = TRUE;
    /* first color, ncolors */
    test_send_u16(os, 0);
    test_send_u16(os, 1);

    /* r,g,b */
    test_send_u16(os, 128);
    test_send_u16(os, 128);
    test_send_u16(os, 128);
    allowfail = FALSE;
}


static void test_overflow_cmap_server(GInputStream *is, GOutputStream *os)
{
    /* Frame buffer width / height */
    test_send_u16(os, 100);
    test_send_u16(os, 100);

    /* BPP, depth, endian, true color */
    test_send_u8(os, 32);
    test_send_u8(os, 8);
    test_send_u8(os, 1);
    test_send_u8(os, 0);

    /* RGB max + shift*/
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u16(os, 255);
    test_send_u8(os, 0);
    test_send_u8(os, 8);
    test_send_u8(os, 16);

    guint8 pad[3] = {0};
    test_send_bytes(os, pad, G_N_ELEMENTS(pad));

    /* name */
    guint8 name[] = { 'T', 'e', 's', 't' };
    test_send_u32(os, G_N_ELEMENTS(name));
    test_send_bytes(os, name, G_N_ELEMENTS(name));

    /* n-encodings */
    test_recv_u8(is, 2);
    /* pad */
    test_recv_u8(is, 0);
    /* num encodings */
    test_recv_u16(is, 5);

    /* encodings */
    test_recv_s32(is, 16);
    test_recv_s32(is, 5);
    test_recv_s32(is, 2);
    test_recv_s32(is, 1);
    test_recv_s32(is, 0);

    /* update request */
    test_recv_u8(is, 3);
    /* ! incremental */
    test_recv_u8(is, 0);

    /* x, y, w, h */
    test_recv_u16(is, 0);
    test_recv_u16(is, 0);
    test_recv_u16(is, 100);
    test_recv_u16(is, 100);

    /* set color map */
    test_send_u8(os, 1);
    /* pad */
    test_send_u8(os, 0);
    /* first color, ncolors */
    test_send_u16(os, 65535);
    test_send_u16(os, 2);

    allowfail = TRUE;
    /* r,g,b */
    for (int i = 0 ; i < 2; i++) {
        test_send_u16(os, i);
        test_send_u16(os, i);
        test_send_u16(os, i);
    }
    allowfail = FALSE;
}


static void test_validation(void (*test_func)(GInputStream *, GOutputStream *))
{
    struct GVncTest *test;
    char *port;
    GThread *th;

    test = g_new0(struct GVncTest, 1);
    test->test_func = test_func;

    g_mutex_init(&test->lock);
    g_mutex_init(&test->clock);
    g_cond_init(&test->cond);
    g_mutex_lock(&test->lock);

    th = g_thread_new("rre-server", test_helper_server, test);

    g_mutex_lock(&test->lock);
    port = g_strdup_printf("%d", test->port);

    test->conn = vnc_connection_new();

    g_signal_connect(test->conn, "vnc-initialized",
                     G_CALLBACK(test_helper_initialized), test);
    g_signal_connect(test->conn, "vnc-disconnected",
                     G_CALLBACK(test_helper_disconnected), test);
    g_signal_connect(test->conn, "vnc-auth-choose-type",
                     G_CALLBACK(test_helper_auth_choose_type), test);
    g_signal_connect(test->conn, "vnc-desktop-resize",
                     G_CALLBACK(test_helper_desktop_resize), test);
    g_signal_connect(test->conn, "vnc-error",
                     G_CALLBACK(test_helper_error), test);

    vnc_connection_open_host(test->conn, "127.0.0.1", port);

    test->loop = g_main_loop_new(g_main_context_default(), FALSE);

    g_main_loop_run(test->loop);

    g_mutex_lock(&test->clock);
    test->quit = TRUE;
    g_mutex_unlock(&test->clock);
    g_cond_signal(&test->cond);

    g_thread_join(th);

    vnc_connection_shutdown(test->conn);
    g_object_unref(test->conn);
    g_free(test->pixels);
    g_main_loop_unref(test->loop);

    g_assert(test->error);
    if (debug)
        g_printerr("Got err %s\n", test->error);
    g_free(test->error);

    g_free(port);
    g_free(test);
}

static void test_validation_rre(void)
{
    test_validation(test_rre_bounds_server);
}

static void test_validation_hextile(void)
{
    test_validation(test_hextile_bounds_server);
}

static void test_validation_copyrect(void)
{
    test_validation(test_copyrect_bounds_server);
}

static void test_validation_unexpected_cmap(void)
{
    test_validation(test_unexpected_cmap_server);
}

static void test_validation_overflow_cmap(void)
{
    test_validation(test_overflow_cmap_server);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    if (getenv("GTK_VNC_DEBUG")) {
        debug = TRUE;
        vnc_util_set_debug(TRUE);
    }

    g_test_add_func("/conn/validation/rre", test_validation_rre);
    g_test_add_func("/conn/validation/copyrect", test_validation_copyrect);
    g_test_add_func("/conn/validation/hextile", test_validation_hextile);
    g_test_add_func("/conn/validation/unexpectedcmap", test_validation_unexpected_cmap);
    g_test_add_func("/conn/validation/overflowcmap", test_validation_overflow_cmap);

    return g_test_run();
}
/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
