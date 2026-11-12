/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm_dp_mst_topology_internal.h (ours: Linux's is GPL) */
#ifndef _KPI_DRM_DP_MST_TOPOLOGY_INTERNAL_H
#define _KPI_DRM_DP_MST_TOPOLOGY_INTERNAL_H

/* what drm_dp_mst_topology.c shares with its tests */
#include <drm/display/drm_dp_mst_helper.h>
void	drm_dp_encode_sideband_req(const struct drm_dp_sideband_msg_req_body *req, struct drm_dp_sideband_msg_tx *raw);
int	drm_dp_decode_sideband_req(const struct drm_dp_sideband_msg_tx *raw, struct drm_dp_sideband_msg_req_body *req);
void	drm_dp_dump_sideband_msg_req_body(const struct drm_dp_sideband_msg_req_body *req, int indent, struct drm_printer *printer);

#endif
