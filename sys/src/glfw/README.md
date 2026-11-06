# GLFW on 9front

GLFW 3.5.1 (zlib, `vendor/LICENSE.md`) with a platform of its own for 9front,
../9front-ui PLAN §6.1 (stage 4b): `libglfw3.a` in `/$objtype/lib.gcc/ape`, the
headers in `/sys/include/ape/GLFW`. Programs are APE programs:

    x86_64-pc-plan9-gcc -o prog prog.c -lglfw3 -ldraw -l9 -lfmt -lutf -lm

and with OpenGL or Vulkan (Mesa's static libvulkan.a has EGL, GL and the Vulkan
loader; GLFW refers to them weakly, `-u` pulls them in; g++ for Mesa's C++):

    x86_64-pc-plan9-g++ -o prog prog.o -Wl,-u,eglGetDisplay -Wl,-u,vkGetInstanceProcAddr \
        -lglfw3 -lvulkan -ldraw -l9 -lfmt -lutf -lm

- `plan9`: the platform `plan9` (`GLFW_PLATFORM_PLAN9`): the process's window of wsys
  or rio (the screen without either) through draw(3) (`plan9_draw.c`, APE's libdraw);
  size, place, hidden and current through `/dev/wctl` (written; read by a thread: the
  window changed), mouse `/dev/mouse` (`r`: resized; pointer warped by `m x y`),
  keyboard `/dev/kbd` (kbd(3): key down and up, characters from `c`; a key's scancode
  is its rune), title `/dev/label`, clipboard `/dev/snarf`, cursors `/dev/cursor`
  (`plan9_cursor.c`: 16x16 of two bits, an image's scaled down and made dark or light,
  the standard cursors drawn there), one monitor (the screen). OpenGL and GLES: EGL's
  plan9 platform (zink), the native window `""`; Vulkan: `VK_PLAN9_surface`.
  `plan9_module.c` answers GLFW's dlopen of libEGL, libGL and libvulkan from the
  program's own (weak references). One window a process; readers of mouse, keyboard
  and wctl are threads, `glfwWaitEvents` waits for them (or `glfwPostEmptyEvent`).
- `overlay`: GLFW's files changed for 9front (marked "9front:"): `include/GLFW/glfw3.h`
  (`GLFW_PLATFORM_PLAN9`), `src/platform.c` (the platform in the table), `src/platform.h`
  (its state in GLFW's structures).
- `vendor`: GLFW's files that `SOURCES` names and the headers they include, made by
  `scripts/glfw/vendor.sh` from the release (`VERSION`; its zip in ../9front-ui/src)
  with the overlay over it; `build.json` too.

Not yet: joysticks, more than one window, window icons, gamma ramps.
Tests: `tests/glfw/run.sh` (VESA, under wsys), `tests/gpu/amdglfw.rc` (OpenGL, Vulkan).
