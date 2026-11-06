# SDL3 on 9front

SDL 3.4.20 (zlib, `vendor/LICENSE.txt`) with drivers of its own for 9front,
../9front-ui PLAN §6.1 (stage 4b): `libSDL3.a` in `/$objtype/lib.gcc/ape`,
the headers in `/sys/include/ape/SDL3`. Programs are APE programs:

    x86_64-pc-plan9-gcc -o prog prog.c -lSDL3 -ldraw -l9 -lfmt -lutf -lm

- `plan9/video`: the video driver `plan9`, in SDL's slot for drivers outside its
  tree (`SDL_VIDEO_DRIVER_PRIVATE`): the process's window of wsys or rio (the screen
  without either) through draw(3) (`p9draw.c`, APE's libdraw), the window
  framebuffer loaded into it; mouse `/dev/mouse` (`r`: resized), keyboard
  `/dev/kbd` (kbd(3): key down and up, text from `c`), title `/dev/label`,
  clipboard `/dev/snarf`, size `/dev/wctl`. One window a process. Vulkan:
  `VK_PLAN9_surface`, the static libvulkan.a by a weak reference (a program with
  SDL's Vulkan links `-lvulkan`).
- `plan9/audio`: the audio driver `plan9` (`SDL_AUDIO_DRIVER_PRIVATE`): `/dev/audio`,
  16-bit stereo at 44100 Hz.
- `plan9/include/build_config/SDL_build_config.h`: what SDL's CMake found for APE
  (a unix without X11, `SDL_UNIX_CONSOLE_BUILD`), changed where it says "9front:".
- `overlay`: SDL's files changed for 9front (marked "9front:"): `SDL.c` (no GTK),
  `time/unix/SDL_systime.c` (no langinfo.h, no tm_gmtoff), `thread/pthread/SDL_systhread.c`
  (no SIGWINCH, no POSIX scheduling).
- `vendor`: SDL's files that `SOURCES` names and the headers they include, made by
  `scripts/sdl3/vendor.sh` from the release (`VERSION`; its tarball in
  ../9front-ui/src) with the overlay over it; `build.json` too.

Not yet: OpenGL (EGL's plan9 platform), joysticks, cursors, more than one window,
SDL's GPU API.
