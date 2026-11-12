// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: DMI quirk tables against the machine's SMBIOS
 * strings.  The Plan 9 side reads the tables (#P/smbios, p9smbios.c);
 * here which structure and field each DMI string is (SMBIOS: type 0
 * BIOS, 1 system, 2 baseboard, 3 chassis).  Without tables nothing
 * matches, as on a machine without DMI.
 */
#include <linux/dmi.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <kpi/p9.h>

static const struct {
	int	type;
	int	off;	/* of the byte naming the string */
} where[DMI_STRING_MAX] = {
	[DMI_BIOS_VENDOR] = { 0, 0x04 },
	[DMI_BIOS_VERSION] = { 0, 0x05 },
	[DMI_BIOS_DATE] = { 0, 0x08 },
	[DMI_SYS_VENDOR] = { 1, 0x04 },
	[DMI_PRODUCT_NAME] = { 1, 0x05 },
	[DMI_PRODUCT_VERSION] = { 1, 0x06 },
	[DMI_PRODUCT_SERIAL] = { 1, 0x07 },
	[DMI_PRODUCT_SKU] = { 1, 0x19 },
	[DMI_PRODUCT_FAMILY] = { 1, 0x1A },
	[DMI_BOARD_VENDOR] = { 2, 0x04 },
	[DMI_BOARD_NAME] = { 2, 0x05 },
	[DMI_BOARD_VERSION] = { 2, 0x06 },
	[DMI_BOARD_SERIAL] = { 2, 0x07 },
	[DMI_BOARD_ASSET_TAG] = { 2, 0x08 },
	[DMI_CHASSIS_VENDOR] = { 3, 0x04 },
};

static DEFINE_MUTEX(dmilock);
static bool dmidone;
static const char *dmistr[DMI_STRING_MAX];
static char dmiuuid[37], dmichassis[4];

/* a string of only spaces is none */
static const char *
blank(const char *s)
{
	const char *p;

	if (!s)
		return NULL;
	for (p = s; *p == ' '; p++)
		;
	return *p ? s : "";
}

/* the system's UUID (type 1, 16 bytes at 8); from SMBIOS 2.6 its first three fields are little-endian */
static const char *
uuid(void)
{
	u8 u[16];
	int i, zero, ones;

	if (p9_smbios_bytes(1, 0x08, u, sizeof(u)) != sizeof(u))
		return NULL;
	zero = ones = 1;
	for (i = 0; i < 16; i++) {
		zero &= u[i] == 0x00;
		ones &= u[i] == 0xFF;
	}
	if (zero || ones)
		return NULL;	/* none, or not set */
	if (p9_smbios_version() >= 0x0206)
		snprintf(dmiuuid, sizeof(dmiuuid),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			u[3], u[2], u[1], u[0], u[5], u[4], u[7], u[6],
			u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
	else
		snprintf(dmiuuid, sizeof(dmiuuid),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			u[0], u[1], u[2], u[3], u[4], u[5], u[6], u[7],
			u[8], u[9], u[10], u[11], u[12], u[13], u[14], u[15]);
	return dmiuuid;
}

static void
dmiload(void)
{
	u8 t;
	int f;

	mutex_lock(&dmilock);
	if (!dmidone) {
		for (f = DMI_NONE + 1; f < DMI_STRING_MAX; f++)
			if (where[f].off)
				dmistr[f] = blank(p9_smbios_string(where[f].type, where[f].off));
		dmistr[DMI_PRODUCT_UUID] = uuid();
		/* the chassis type as a number (its top bit: a lock) */
		if (p9_smbios_bytes(3, 0x05, &t, 1) == 1) {
			snprintf(dmichassis, sizeof(dmichassis), "%u", t & 0x7F);
			dmistr[DMI_CHASSIS_TYPE] = dmichassis;
		}
		dmidone = true;
	}
	mutex_unlock(&dmilock);
}

const char *
dmi_get_system_info(int field)
{
	if (field <= DMI_NONE || field >= DMI_STRING_MAX)
		return NULL;
	dmiload();
	return dmistr[field];
}

static bool
matches(const struct dmi_system_id *d)
{
	const struct dmi_strmatch *m;
	const char *s;
	int i;

	for (i = 0; i < (int)ARRAY_SIZE(d->matches); i++) {
		m = &d->matches[i];
		if (m->slot == DMI_NONE)
			continue;
		s = dmi_get_system_info(m->slot);
		if (!s)
			return false;
		if (m->exact_match ? strcmp(s, m->substr) != 0 : !strstr(s, m->substr))
			return false;
	}
	return true;
}

static bool
islast(const struct dmi_system_id *d)
{
	return !d->ident && !d->matches[0].slot && !d->callback;
}

int
dmi_check_system(const struct dmi_system_id *list)
{
	const struct dmi_system_id *d;
	int n = 0;

	for (d = list; !islast(d); d++)
		if (matches(d)) {
			n++;
			if (d->callback && d->callback(d))
				break;
		}
	return n;
}

const struct dmi_system_id *
dmi_first_match(const struct dmi_system_id *list)
{
	const struct dmi_system_id *d;

	for (d = list; !islast(d); d++)
		if (matches(d))
			return d;
	return NULL;
}

/*
 * Linux 6.14 on: per-panel backlight fixes (minimum brightness, PWM),
 * found by DMI and the panel's EDID (drm_panel_backlight_quirks.c, GPL).
 * We know of no panel that needs one: amdgpu_dm takes the panel's own
 * values.
 */
#include <drm/drm_utils.h>

const struct drm_panel_backlight_quirk *
drm_get_panel_backlight_quirk(const struct drm_edid *edid)
{
	return NULL;
}
