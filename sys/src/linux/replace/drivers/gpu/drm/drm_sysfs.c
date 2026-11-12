// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: drm_sysfs.c (ours: Linux's is GPL).
 *
 * No sysfs: the devices of the minors are plain structs, the
 * connectors are the file server's outputs/ files, and hotplug events
 * go to the file server (kpi_drm_hotplug, kpi/drm.c), which wakes the
 * readers of its files.
 */
#include <linux/device.h>
#include <linux/slab.h>
#include <drm/drm_connector.h>
#include <drm/drm_device.h>
#include <drm/drm_file.h>
#include <drm/drm_print.h>
#include <drm/drm_property.h>
#include <drm/drm_sysfs.h>

#include "drm_internal.h"

void	kpi_drm_hotplug(struct drm_device *dev, struct drm_connector *connector);

int drm_sysfs_init(void)
{
	return 0;
}

void drm_sysfs_destroy(void)
{
}

static void minor_release(struct device *kdev)
{
	kfree(kdev);
}

struct device *drm_sysfs_minor_alloc(struct drm_minor *minor)
{
	struct device *kdev;

	kdev = kzalloc(sizeof(*kdev), GFP_KERNEL);
	if (!kdev)
		return ERR_PTR(-ENOMEM);
	device_initialize(kdev);
	kdev->parent = minor->dev->dev;
	kdev->release = minor_release;
	dev_set_drvdata(kdev, minor);
	dev_set_name(kdev, "card%d", minor->index);
	return kdev;
}

/*
 * a connector's device, as Linux makes it (its name card0-eDP-1): the
 * drivers name things after it (intel_dp's AUX bus) and read its parent
 */
static void connector_release(struct device *kdev)
{
	kfree(kdev->kobj.name);
	kfree(kdev);
}

int drm_sysfs_connector_add(struct drm_connector *connector)
{
	struct drm_device *dev = connector->dev;
	struct device *kdev;

	if (connector->kdev)
		return 0;
	kdev = kzalloc(sizeof(*kdev), GFP_KERNEL);
	if (!kdev)
		return -ENOMEM;
	/* the connector's device, a child of its card's: no sysfs here, a name and the data */
	device_initialize(kdev);
	dev_set_name(kdev, "card%d-%s", dev->primary->index, connector->name);
	kdev->release = connector_release;
	kdev->driver_data = connector;
	kdev->parent = connector->dev->primary->kdev;
	connector->kdev = kdev;
	return 0;
}

int drm_sysfs_connector_add_late(struct drm_connector *connector)
{
	return 0;
}

void drm_sysfs_connector_remove(struct drm_connector *connector)
{
	if (!connector->kdev)
		return;
	connector_release(connector->kdev);
	connector->kdev = NULL;
}

void drm_sysfs_connector_remove_early(struct drm_connector *connector)
{
}

void drm_sysfs_lease_event(struct drm_device *dev)
{
}

void drm_sysfs_hotplug_event(struct drm_device *dev)
{
	kpi_drm_hotplug(dev, NULL);
}

void drm_sysfs_connector_hotplug_event(struct drm_connector *connector)
{
	kpi_drm_hotplug(connector->dev, connector);
}

void drm_sysfs_connector_property_event(struct drm_connector *connector, struct drm_property *property)
{
	kpi_drm_hotplug(connector->dev, connector);
}

int drm_class_device_register(struct device *dev)
{
	return 0;
}

void drm_class_device_unregister(struct device *dev)
{
}
