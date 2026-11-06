/*
  SPDX-License-Identifier: Zlib

  SDL's audio driver for 9front ("plan9"): /dev/audio (audio(3)), signed
  16-bit little-endian stereo at 44100 Hz; SDL converts the program's
  format to it.  A write blocks until the device takes it, so it happens
  in WaitDevice: SDL calls PlayDevice holding the device's lock, and a
  thread blocked there with the lock (taken again right after each write)
  starves every other caller of the device, SDL_DestroyAudioStream and
  SDL_Quit too.  PlayDevice only notes the buffer.  Recording the same
  way: WaitRecordingDevice reads, RecordDevice hands over what it got.
*/
#include "SDL_internal.h"

#ifdef SDL_AUDIO_DRIVER_PLAN9

#include "audio/SDL_sysaudio.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

struct SDL_PrivateAudioData {
    int fd;
    Uint8 *mixbuf;
    int pending;    /* bytes of mixbuf PlayDevice left for WaitDevice */
    Uint8 *recbuf;  /* recording: what WaitRecordingDevice read */
    int got;
};

/* the device thread, without the lock: writes what PlayDevice left */
static bool PLAN9AUDIO_WaitDevice(SDL_AudioDevice *device)
{
    struct SDL_PrivateAudioData *h = device->hidden;
    const Uint8 *p = h->mixbuf;
    ssize_t n;

    while (h->pending > 0) {
        if ((n = write(h->fd, p, (size_t)h->pending)) <= 0) {
            h->pending = 0;
            return false;
        }
        p += n;
        h->pending -= (int)n;
    }
    return true;
}

/* the device thread, without the lock: reads the next piece of the recording */
static bool PLAN9AUDIO_WaitRecordingDevice(SDL_AudioDevice *device)
{
    struct SDL_PrivateAudioData *h = device->hidden;
    ssize_t n;

    if (h->got > 0) {
        return true;
    }
    if ((n = read(h->fd, h->recbuf, (size_t)device->buffer_size)) < 0) {
        return false;
    }
    h->got = (int)n;
    return true;
}

/* with the lock held: must not block */
static bool PLAN9AUDIO_PlayDevice(SDL_AudioDevice *device, const Uint8 *buffer, int size)
{
    SDL_assert(buffer == device->hidden->mixbuf);
    device->hidden->pending = size;
    return true;
}

static Uint8 *PLAN9AUDIO_GetDeviceBuf(SDL_AudioDevice *device, int *size)
{
    (void)size;
    return device->hidden->mixbuf;
}

/* with the lock held: must not block */
static int PLAN9AUDIO_RecordDevice(SDL_AudioDevice *device, void *buffer, int buflen)
{
    struct SDL_PrivateAudioData *h = device->hidden;
    int n = h->got < buflen ? h->got : buflen;

    SDL_memcpy(buffer, h->recbuf, n);
    h->got = 0;
    return n;
}

static void PLAN9AUDIO_FlushRecording(SDL_AudioDevice *device)
{
    device->hidden->got = 0;
}

static void PLAN9AUDIO_CloseDevice(SDL_AudioDevice *device)
{
    if (device->hidden) {
        if (device->hidden->fd >= 0) {
            close(device->hidden->fd);
        }
        SDL_free(device->hidden->mixbuf);
        SDL_free(device->hidden->recbuf);
        SDL_free(device->hidden);
        device->hidden = NULL;
    }
}

static bool PLAN9AUDIO_OpenDevice(SDL_AudioDevice *device)
{
    if (!(device->hidden = (struct SDL_PrivateAudioData *)SDL_calloc(1, sizeof(*device->hidden)))) {
        return false;
    }
    device->hidden->fd = open("/dev/audio", device->recording ? O_RDONLY : O_WRONLY);
    if (device->hidden->fd < 0) {
        return SDL_SetError("plan9: /dev/audio: %s", strerror(errno));
    }
    /* what audio(3) takes by default */
    device->spec.format = SDL_AUDIO_S16LE;
    device->spec.channels = 2;
    device->spec.freq = 44100;
    SDL_UpdatedAudioDeviceFormat(device);
    if (device->recording) {
        if (!(device->hidden->recbuf = (Uint8 *)SDL_malloc(device->buffer_size))) {
            return false;
        }
    } else {
        if (!(device->hidden->mixbuf = (Uint8 *)SDL_malloc(device->buffer_size))) {
            return false;
        }
        SDL_memset(device->hidden->mixbuf, device->silence_value, device->buffer_size);
    }
    return true;
}

static void PLAN9AUDIO_DetectDevices(SDL_AudioDevice **default_playback, SDL_AudioDevice **default_recording)
{
    *default_playback = SDL_AddAudioDevice(false, DEFAULT_PLAYBACK_DEVNAME, NULL, (void *)0x1);
    *default_recording = SDL_AddAudioDevice(true, DEFAULT_RECORDING_DEVNAME, NULL, (void *)0x2);
}

static bool PLAN9AUDIO_Init(SDL_AudioDriverImpl *impl)
{
    if (access("/dev/audio", F_OK) < 0) {
        return false;
    }
    impl->OpenDevice = PLAN9AUDIO_OpenDevice;
    impl->WaitDevice = PLAN9AUDIO_WaitDevice;
    impl->WaitRecordingDevice = PLAN9AUDIO_WaitRecordingDevice;
    impl->PlayDevice = PLAN9AUDIO_PlayDevice;
    impl->GetDeviceBuf = PLAN9AUDIO_GetDeviceBuf;
    impl->RecordDevice = PLAN9AUDIO_RecordDevice;
    impl->FlushRecording = PLAN9AUDIO_FlushRecording;
    impl->CloseDevice = PLAN9AUDIO_CloseDevice;
    impl->DetectDevices = PLAN9AUDIO_DetectDevices;
    impl->HasRecordingSupport = true;
    return true;
}

/* SDL's slot for a driver outside its tree (SDL_AUDIO_DRIVER_PRIVATE): no change to SDL_audio.c */
AudioBootStrap PRIVATEAUDIO_bootstrap = {
    "plan9", "9front's /dev/audio", PLAN9AUDIO_Init, false, false
};

#endif /* SDL_AUDIO_DRIVER_PLAN9 */
