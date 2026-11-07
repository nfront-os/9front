/*
 * SPDX-License-Identifier: MIT
 *
 * Mesa's Wayland platform (Vulkan's WSI, EGL) refers to libwayland-client
 * weakly on 9front (included in every file: scripts/mesa/build.sh):
 * libvulkan.a is linked whole into every Vulkan and GL program, most of
 * which speak no Wayland and link no libwayland.  A program that does
 * (its own wl_display for VK_KHR_wayland_surface or EGL's Wayland
 * platform) links -lwayland-client -lffi, which then gives these too: its
 * calls to wl_display_connect and the core interfaces take
 * wayland-client.o, wayland-util.o and wayland-protocol.o out of the
 * archive.  The names are those Mesa uses (build.sh fails on another).
 */
#pragma weak wl_display_connect
#pragma weak wl_display_create_queue_with_name
#pragma weak wl_display_disconnect
#pragma weak wl_display_dispatch_pending
#pragma weak wl_display_dispatch_queue
#pragma weak wl_display_dispatch_queue_pending
#pragma weak wl_display_dispatch_queue_timeout
#pragma weak wl_display_flush
#pragma weak wl_display_roundtrip_queue
#pragma weak wl_event_queue_destroy
#pragma weak wl_list_init
#pragma weak wl_list_insert
#pragma weak wl_list_remove
#pragma weak wl_proxy_add_listener
#pragma weak wl_proxy_create_wrapper
#pragma weak wl_proxy_destroy
#pragma weak wl_proxy_get_id
#pragma weak wl_proxy_get_version
#pragma weak wl_proxy_marshal_flags
#pragma weak wl_proxy_set_queue
#pragma weak wl_proxy_wrapper_destroy
#pragma weak wl_buffer_interface
#pragma weak wl_callback_interface
#pragma weak wl_display_interface
#pragma weak wl_fixes_interface
#pragma weak wl_output_interface
#pragma weak wl_registry_interface
#pragma weak wl_shm_interface
#pragma weak wl_shm_pool_interface
#pragma weak wl_surface_interface
