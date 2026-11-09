/*
 * SPDX-License-Identifier: MIT
 *
 * zxdg_decoration_manager_v1: wsys draws a toplevel's border (and moves
 * and sweeps it by that), so a toplevel's decoration is the server's
 * always; a client asking for its own is told so too (the protocol lets
 * the server choose).
 */
#include "dat.h"
#include <xdg-decoration-unstable-v1-server-protocol.h>

static void
resdestroy(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	wl_resource_destroy(r);
}

/* the mode, then the toplevel's configure (the decoration's is part of it) */
static void
decoconfigure(struct wl_resource *r)
{
	Surface *s;

	zxdg_toplevel_decoration_v1_send_configure(r, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
	if((s = wl_resource_get_user_data(r)) != NULL && s->committed)
		configure(s);
}

static void
decosetmode(struct wl_client *c, struct wl_resource *r, uint32_t mode)
{
	(void)c; (void)mode;
	decoconfigure(r);
}

static void
decounsetmode(struct wl_client *c, struct wl_resource *r)
{
	(void)c;
	decoconfigure(r);
}

static void
decodestroyres(struct wl_resource *r)
{
	Surface *s;

	if((s = wl_resource_get_user_data(r)) != NULL)
		s->deco = NULL;
}

static const struct zxdg_toplevel_decoration_v1_interface decoimpl = {
	resdestroy,
	decosetmode,
	decounsetmode,
};

static void
mgrget(struct wl_client *c, struct wl_resource *r, uint32_t id, struct wl_resource *toplevel)
{
	struct wl_resource *d;
	Surface *s;

	if((s = wl_resource_get_user_data(toplevel)) != NULL && s->deco != NULL){
		wl_resource_post_error(r, ZXDG_TOPLEVEL_DECORATION_V1_ERROR_ALREADY_CONSTRUCTED, "a decoration already");
		return;
	}
	if((d = wl_resource_create(c, &zxdg_toplevel_decoration_v1_interface, wl_resource_get_version(r), id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	/* the surface of the toplevel (xdg.c's user data); if it goes first, nil */
	wl_resource_set_implementation(d, &decoimpl, s, decodestroyres);
	if(s != NULL)
		s->deco = d;
	zxdg_toplevel_decoration_v1_send_configure(d, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
}

static const struct zxdg_decoration_manager_v1_interface mgrimpl = {
	resdestroy,
	mgrget,
};

static void
mgrbind(struct wl_client *c, void *data, uint32_t version, uint32_t id)
{
	struct wl_resource *r;

	(void)data;
	if((r = wl_resource_create(c, &zxdg_decoration_manager_v1_interface, version, id)) == NULL){
		wl_client_post_no_memory(c);
		return;
	}
	wl_resource_set_implementation(r, &mgrimpl, NULL, NULL);
}

void
decoinit(void)
{
	wl_global_create(display, &zxdg_decoration_manager_v1_interface, 1, NULL, mgrbind);
}
