/*
 * GTK VNC Widget
 *
 * Copyright (C) 2010 Daniel P. Berrange <dan@berrange.com>
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
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "vncgrabsequence.h"

GType vnc_grab_sequence_get_type(void)
{
    static GType grab_sequence_type = 0;

    if (G_UNLIKELY(grab_sequence_type == 0)) {
        grab_sequence_type = g_boxed_type_register_static
            ("VncGrabSequence",
             (GBoxedCopyFunc)vnc_grab_sequence_copy,
             (GBoxedFreeFunc)vnc_grab_sequence_free);
    }

    return grab_sequence_type;
}


/**
 * vnc_grab_sequence_new:
 * @nkeysyms: length of @keysyms
 * @keysyms: (transfer none)(array length=nkeysyms): the keysym values
 *
 * Creates a new grab sequence from a list of keysym values
 *
 * Returns: (transfer full): a new grab sequence object
 */
VncGrabSequence *vnc_grab_sequence_new(guint nkeysyms, guint *keysyms)
{
    VncGrabSequence *sequence;

    sequence = g_slice_new0(VncGrabSequence);
    sequence->nkeysyms = nkeysyms;
    sequence->keysyms = g_new0(guint, nkeysyms);
    memcpy(sequence->keysyms, keysyms, sizeof(guint)*nkeysyms);

    return sequence;
}


/**
 * vnc_grab_sequence_new_from_string:
 * @str: (transfer none): string of keysym names
 *
 * Creates a new grab sequence from a list of keysym names
 *
 * The returned struct must be freed by calling
 * vnc_grab_sequence_free when no longer required
 *
 * Returns: (transfer full): a new grab sequence object
 */
VncGrabSequence *vnc_grab_sequence_new_from_string(const gchar *str)
{
    gchar **keysymstr;
    int i;
    VncGrabSequence *sequence;

    sequence = g_slice_new0(VncGrabSequence);

    keysymstr = g_strsplit(str, "+", 5);

    sequence->nkeysyms = 0;
    while (keysymstr[sequence->nkeysyms])
        sequence->nkeysyms++;

    sequence->keysyms = g_new0(guint, sequence->nkeysyms);
    for (i = 0 ; i < sequence->nkeysyms ; i++)
        sequence->keysyms[i] =
            (guint)gdk_keyval_from_name(keysymstr[i]);

    g_strfreev(keysymstr);

    return sequence;

}


/**
 * vnc_grab_sequence_copy:
 * @sequence: (transfer none): the grab sequence
 *
 * Allocate a new grab sequence struct, initalizing it
 * with a copy of data  from @sequence
 *
 * The returned struct must be freed by calling
 * vnc_grab_sequence_free when no longer required
 *
 * Returns: (transfer full): the grab sequence
 */
VncGrabSequence *vnc_grab_sequence_copy(VncGrabSequence *sequence)
{
    VncGrabSequence *ret;

    ret = g_slice_dup(VncGrabSequence, sequence);
    ret->keysyms = g_new0(guint, sequence->nkeysyms);
    memcpy(ret->keysyms, sequence->keysyms,
           sizeof(guint) * ret->nkeysyms);

    return ret;
}


/**
 * vnc_grab_sequence_free:
 * @sequence: (transfer none): the grab sequence
 *
 * Release memory associated with the grab sequence
 * @sequence.
 */
void vnc_grab_sequence_free(VncGrabSequence *sequence)
{
    g_slice_free(VncGrabSequence, sequence);
}


/**
 * vnc_grab_sequence_as_string:
 * @sequence: (transfer none): the grab sequence
 *
 * Convert the grab sequence to a string of keysym names
 *
 * Returns: (transfer full): the grab sequence as a string
 */
gchar *vnc_grab_sequence_as_string(VncGrabSequence *sequence)
{
    GString *str = g_string_new("");
    int i;

    for (i = 0 ; i < sequence->nkeysyms ; i++) {
        if (i > 0)
            g_string_append_c(str, '+');
        g_string_append(str, gdk_keyval_name(sequence->keysyms[i]));
    }

    return g_string_free(str, FALSE);

}

/**
 * vnc_grab_sequence_get_nth:
 * @sequence: (transfer none): the grab sequence
 * @n: the index of the key symbol to obtain
 *
 * Obtain the nth key symbol in the sequence
 *
 * Returns: the nth key symbol
 */
guint vnc_grab_sequence_get_nth(VncGrabSequence *sequence,
                                guint n)
{
    if (n > sequence->nkeysyms)
        return GDK_KEY_VoidSymbol;

    return sequence->keysyms[n];
}


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
