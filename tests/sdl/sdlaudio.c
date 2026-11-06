/*
 * SPDX-License-Identifier: MIT
 *
 * sdlaudio: SDL3's audio on 9front alone (no window): a tone of a second
 * through the default playback device, each step with its time, so that
 * one that waits shows.
 */
#include <SDL3/SDL.h>
#include <stdio.h>

static Sint16 tone[22050];

#define STEP(what) printf("%6llu ms %s\n", (unsigned long long)SDL_GetTicks(), what), fflush(stdout)

int main(void)
{
    SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, 22050 };
    SDL_AudioStream *as;
    int i;

    for (i = 0; i < (int)SDL_arraysize(tone); i++) {
        tone[i] = (Sint16)(i % 50 < 25 ? 3000 : -3000);
    }
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        printf("FAIL SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    STEP("init");
    if (!(as = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL))) {
        printf("FAIL open: %s\n", SDL_GetError());
        return 1;
    }
    STEP("opened");
    SDL_PutAudioStreamData(as, tone, sizeof tone);
    SDL_ResumeAudioStreamDevice(as);
    STEP("playing");
    SDL_Delay(1200);
    STEP("played");
    SDL_DestroyAudioStream(as);
    STEP("stream destroyed");
    SDL_Quit();
    STEP("quit");
    printf("sdlaudio: done\n");
    return 0;
}
