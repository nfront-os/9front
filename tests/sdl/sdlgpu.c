/*
 * SPDX-License-Identifier: MIT
 *
 * sdlgpu: SDL3's GPU API (SDL_gpu, its Vulkan backend: sys/src/sdl3) on
 * 9front, in a window of wsys or rio: a device, the window claimed (a
 * swapchain of the plan9 WSI), n frames each a render pass that clears the
 * swapchain's texture green, then the window's pixels (/dev/window,
 * image(6)) green.  No shaders: a clear needs none.  Links -lvulkan.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: sdlgpu [frames]
 */
#include <SDL3/SDL.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { W = 320, H = 240 };

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)

/* the whole of a file */
static char *slurp(const char *path, long *len)
{
    char *b = NULL;
    long n = 0, m;
    int fd;

    if ((fd = open(path, O_RDONLY)) < 0) {
        return NULL;
    }
    for (;;) {
        b = realloc(b, n + 65536);
        if ((m = read(fd, b + n, 65536)) <= 0) {
            break;
        }
        n += m;
    }
    close(fd);
    *len = n;
    return b;
}

/* the window's pixel at x y inside its border of 4, from /dev/window (as sdltest) */
static int pixel(int x, int y, unsigned *rgb)
{
    char *b, f[13];
    long len, off;
    int minx, miny, maxx;
    unsigned char *p;

    if (!(b = slurp("/dev/window", &len)) || len < 60) {
        free(b);
        return -1;
    }
    memcpy(f, b, 12);
    f[12] = 0;
    if (!strstr(f, "x8r8g8b8") && !strstr(f, "a8r8g8b8")) {
        free(b);
        return -2;
    }
    minx = atoi(b + 12);
    miny = atoi(b + 24);
    maxx = atoi(b + 36);
    x += minx + 4;
    y += miny + 4;
    off = 60 + ((long)(y - miny) * (maxx - minx) + (x - minx)) * 4;
    if (off + 4 > len) {
        free(b);
        return -3;
    }
    p = (unsigned char *)b + off;
    *rgb = (unsigned)(p[0] | p[1] << 8 | p[2] << 16);
    free(b);
    return 0;
}

static void checkwin(const char *what, int x, int y, unsigned want)
{
    unsigned got;
    int r = pixel(x, y, &got);

    if (r < 0) {
        FAIL("%s: /dev/window at %d,%d: %d", what, x, y, r);
    } else if (got != want) {
        FAIL("%s: window at %d,%d %06x, want %06x", what, x, y, got, want);
    } else {
        OK("%s: window at %d,%d %06x", what, x, y, got);
    }
}

int main(int argc, char **argv)
{
    int frames = argc > 1 ? atoi(argv[1]) : 60, i, shown = 0;
    SDL_GPUDevice *dev;
    SDL_Window *win;
    SDL_Event ev;
    Uint64 t0;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("FAIL SDL_Init: %s\nsdlgpu: failed\n", SDL_GetError());
        return 1;
    }
    if (!(win = SDL_CreateWindow("sdlgpu", W, H, SDL_WINDOW_VULKAN))) {
        printf("FAIL SDL_CreateWindow: %s\nsdlgpu: failed\n", SDL_GetError());
        return 1;
    }
    if (!(dev = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL))) {
        printf("FAIL SDL_CreateGPUDevice: %s\nsdlgpu: failed\n", SDL_GetError());
        return 1;
    }
    OK("device, driver %s", SDL_GetGPUDeviceDriver(dev));
    if (!SDL_ClaimWindowForGPUDevice(dev, win)) {
        printf("FAIL SDL_ClaimWindowForGPUDevice: %s\nsdlgpu: failed\n", SDL_GetError());
        return 1;
    }
    OK("the window claimed, swapchain %s", SDL_GetGPUSwapchainTextureFormat(dev, win) != SDL_GPU_TEXTUREFORMAT_INVALID ? "there" : "none");
    t0 = SDL_GetTicks();
    for (i = 0; i < frames; i++) {
        SDL_GPUCommandBuffer *cb;
        SDL_GPUTexture *tex = NULL;
        SDL_GPUColorTargetInfo ct;
        Uint32 w, h;

        while (SDL_PollEvent(&ev)) {
        }
        if (!(cb = SDL_AcquireGPUCommandBuffer(dev))) {
            FAIL("SDL_AcquireGPUCommandBuffer: %s", SDL_GetError());
            break;
        }
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cb, win, &tex, &w, &h)) {
            FAIL("SDL_WaitAndAcquireGPUSwapchainTexture: %s", SDL_GetError());
            SDL_CancelGPUCommandBuffer(cb);
            break;
        }
        if (tex != NULL) {
            SDL_GPURenderPass *rp;

            memset(&ct, 0, sizeof ct);
            ct.texture = tex;
            ct.clear_color = (SDL_FColor){ 0.0f, 1.0f, 0.0f, 1.0f };
            ct.load_op = SDL_GPU_LOADOP_CLEAR;
            ct.store_op = SDL_GPU_STOREOP_STORE;
            rp = SDL_BeginGPURenderPass(cb, &ct, 1, NULL);
            SDL_EndGPURenderPass(rp);
            shown++;
        }
        if (!SDL_SubmitGPUCommandBuffer(cb)) {
            FAIL("SDL_SubmitGPUCommandBuffer: %s", SDL_GetError());
            break;
        }
    }
    SDL_WaitForGPUIdle(dev);
    if (shown > 0) {
        OK("%d frames of %d cleared in %.1f s", shown, frames, (SDL_GetTicks() - t0) / 1000.0);
    } else {
        FAIL("no swapchain texture in %d frames", frames);
    }
    SDL_Delay(300);
    checkwin("cleared green", W / 2, H / 2, 0x00ff00);
    checkwin("cleared green", 5, 5, 0x00ff00);
    SDL_ReleaseWindowFromGPUDevice(dev, win);
    SDL_DestroyGPUDevice(dev);
    SDL_DestroyWindow(win);
    SDL_Quit();
    printf(nfail ? "sdlgpu: failed\n" : "sdlgpu: all passed\n");
    return nfail != 0;
}
