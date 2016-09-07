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

#include "vncaudiosample.h"

GType vnc_audio_sample_get_type(void)
{
    static GType audio_sample_type = 0;

    if (G_UNLIKELY(audio_sample_type == 0)) {
        audio_sample_type = g_boxed_type_register_static
            ("VncAudioSample",
             (GBoxedCopyFunc)vnc_audio_sample_copy,
             (GBoxedFreeFunc)vnc_audio_sample_free);
    }

    return audio_sample_type;
}


/**
 * vnc_audio_sample_new:
 * @capacity: the sample size in bytes
 *
 * Allocate a new audio sample able to store @capacity
 * bytes of audio data.
 *
 * The returned sample must be freed with
 * vnc_audio_sample_free when no longer required.
 *
 * Returns: (transfer full): the new audio sample.
 */
VncAudioSample *vnc_audio_sample_new(guint32 capacity)
{
    VncAudioSample *sample;

    sample = g_slice_new0(VncAudioSample);
    sample->length = 0;
    sample->capacity = capacity;
    sample->data = g_new0(guint8, capacity);

    return sample;
}


/**
 * vnc_audio_sample_copy:
 * @sample: the sample to copy
 *
 * Allocate a new audio sample, initializing it with a copy
 * of the data in @sample.
 *
 * The returned sample must be freed with
 * vnc_audio_sample_free when no longer required.
 *
 * Returns: (transfer full): the new audio sample.
 */
VncAudioSample *vnc_audio_sample_copy(VncAudioSample *sample)
{
    VncAudioSample *ret;

    ret = g_slice_dup(VncAudioSample, sample);
    ret->data = g_new0(guint8, sample->capacity);
    memcpy(ret->data, sample->data, sample->length);

    return ret;
}


/**
 * vnc_audio_sample_free:
 * @sample: the sample to free
 *
 * Release memory associated with the audio sample
 * @sample
 */
void vnc_audio_sample_free(VncAudioSample *sample)
{
    g_free(sample->data);
    g_slice_free(VncAudioSample, sample);
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  indent-tabs-mode: nil
 * End:
 */
