/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: drm/i915_hdcp_interface.h (ours: Linux's is GPL) */
#ifndef _KPI_DRM_I915_HDCP_INTERFACE_H
#define _KPI_DRM_I915_HDCP_INTERFACE_H

#include <linux/device.h>
#include <linux/mutex.h>
#include <drm/display/drm_hdcp.h>

/*
 * the interface of i915 to the firmware that does HDCP 2.2 (the ME,
 * or the GSC on Meteor Lake): its types and its messages' layouts
 */
enum hdcp_port_type { HDCP_PORT_TYPE_INVALID, HDCP_PORT_TYPE_INTEGRATED, HDCP_PORT_TYPE_LSPCON, HDCP_PORT_TYPE_CPDP };
enum hdcp_wired_protocol { HDCP_PROTOCOL_INVALID, HDCP_PROTOCOL_HDMI, HDCP_PROTOCOL_DP };
enum hdcp_ddi { HDCP_DDI_INVALID_PORT = 0, HDCP_DDI_B = 1, HDCP_DDI_C, HDCP_DDI_D, HDCP_DDI_E, HDCP_DDI_F, HDCP_DDI_A = 7, HDCP_DDI_RANGE_END = HDCP_DDI_A };
enum hdcp_transcoder { HDCP_INVALID_TRANSCODER = 0, HDCP_TRANSCODER_EDP, HDCP_TRANSCODER_DSI0, HDCP_TRANSCODER_DSI1,
	HDCP_TRANSCODER_A = 0x10, HDCP_TRANSCODER_B, HDCP_TRANSCODER_C, HDCP_TRANSCODER_D };
struct hdcp_port_data {
	enum hdcp_ddi	hdcp_ddi;
	enum hdcp_transcoder	hdcp_transcoder;
	u8	port_type;
	u8	protocol;
	u16	k;
	u32	seq_num_m;
	struct hdcp2_streamid_type	*streams;
};
struct i915_hdcp_ops {
	struct module	*owner;
	int	(*initiate_hdcp2_session)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_ake_init *ake_data);
	int	(*verify_receiver_cert_prepare_km)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_ake_send_cert *rx_cert,
			bool *km_stored, struct hdcp2_ake_no_stored_km *ek_pub_km, size_t *msg_sz);
	int	(*verify_hprime)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_ake_send_hprime *rx_hprime);
	int	(*store_pairing_info)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_ake_send_pairing_info *pairing_info);
	int	(*initiate_locality_check)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_lc_init *lc_init_data);
	int	(*verify_lprime)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_lc_send_lprime *rx_lprime);
	int	(*get_session_key)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_ske_send_eks *ske_data);
	int	(*repeater_check_flow_prepare_ack)(struct device *dev, struct hdcp_port_data *data,
			struct hdcp2_rep_send_receiverid_list *rep_topology, struct hdcp2_rep_send_ack *rep_send_ack);
	int	(*verify_mprime)(struct device *dev, struct hdcp_port_data *data, struct hdcp2_rep_stream_ready *stream_ready);
	int	(*enable_hdcp_authentication)(struct device *dev, struct hdcp_port_data *data);
	int	(*close_hdcp_session)(struct device *dev, struct hdcp_port_data *data);
};
struct i915_hdcp_arbiter { struct device *hdcp_dev; const struct i915_hdcp_ops *ops; struct mutex mutex; };

/* the firmware's messages */
#define HDCP_API_VERSION	0x00010000
#define HDCP_M_LEN		16
#define HDCP_KH_LEN		16
enum fw_hdcp_status { FW_HDCP_STATUS_SUCCESS = 0x0000, FW_HDCP_STATUS_INTERNAL_ERROR = 0x1000, FW_HDCP_STATUS_UNKNOWN_ERROR = 0x1001,
	FW_HDCP_STATUS_INCORRECT_API_VERSION = 0x1002, FW_HDCP_STATUS_INVALID_FUNCTION = 0x1003, FW_HDCP_STATUS_INVALID_BUFFER_LENGTH = 0x1004,
	FW_HDCP_STATUS_INVALID_PARAMS = 0x1005, FW_HDCP_STATUS_AUTHENTICATION_FAILED = 0x1006, FW_HDCP_INVALID_SESSION_STATE = 0x6000,
	FW_HDCP_SRM_FRAGMENT_UNEXPECTED = 0x6001, FW_HDCP_SRM_INVALID_LENGTH = 0x6002 };
enum hdcp_command_id {
	_WIDI_COMMAND_BASE = 0x00030000, WIDI_INITIATE_HDCP2_SESSION = _WIDI_COMMAND_BASE, HDCP_GET_SRM_STATUS, HDCP_SEND_SRM_FRAGMENT,
	_WIRED_COMMAND_BASE = 0x00031000, WIRED_INITIATE_HDCP2_SESSION = _WIRED_COMMAND_BASE, WIRED_VERIFY_RECEIVER_CERT,
	WIRED_AKE_SEND_HPRIME, WIRED_AKE_SEND_PAIRING_INFO, WIRED_INIT_LOCALITY_CHECK, WIRED_VALIDATE_LOCALITY,
	WIRED_GET_SESSION_KEY, WIRED_ENABLE_AUTH, WIRED_VERIFY_REPEATER, WIRED_REPEATER_AUTH_STREAM_REQ, WIRED_CLOSE_SESSION,
	_WIRED_COMMANDS_COUNT,
};
/*
 * the payload's length of each request the driver sends (past the
 * 16-byte header: the port's 4 bytes and the message), by name
 */
#define WIRED_CMD_BUF_LEN_AKE_SEND_HPRIME_IN	36
#define WIRED_CMD_BUF_LEN_CLOSE_SESSION_IN	4
#define WIRED_CMD_BUF_LEN_ENABLE_AUTH_IN	5
#define WIRED_CMD_BUF_LEN_GET_SESSION_KEY_IN	4
#define WIRED_CMD_BUF_LEN_INITIATE_HDCP2_SESSION_IN	5
#define WIRED_CMD_BUF_LEN_INIT_LOCALITY_CHECK_IN	4
#define WIRED_CMD_BUF_LEN_SEND_PAIRING_INFO_IN	20
#define WIRED_CMD_BUF_LEN_VALIDATE_LOCALITY_IN	36
#define WIRED_CMD_BUF_LEN_VERIFY_RECEIVER_CERT_IN	537
#define WIRED_CMD_BUF_LEN_VERIFY_REPEATER_IN	180
union encrypted_buff {
	u8	e_kpub_km[HDCP_2_2_E_KPUB_KM_LEN];
	u8	e_kh_km_m[HDCP_2_2_E_KH_KM_M_LEN];
	struct { u8 e_kh_km[HDCP_KH_LEN]; u8 m[HDCP_M_LEN]; } __packed;
};
struct hdcp_cmd_header { u32 api_version; u32 command_id; enum fw_hdcp_status status; u32 buffer_len; } __packed;
struct hdcp_cmd_no_data { struct hdcp_cmd_header header; } __packed;
struct hdcp_port_id { u8 integrated_port_type; u8 physical_port; u8 attached_transcoder; u8 reserved; } __packed;
#define __KPI_WIRED(n, ...)	struct wired_cmd_##n { struct hdcp_cmd_header header; struct hdcp_port_id port; __VA_ARGS__ } __packed
__KPI_WIRED(initiate_hdcp2_session_in, u8 protocol;);
__KPI_WIRED(initiate_hdcp2_session_out, u8 r_tx[HDCP_2_2_RTX_LEN]; struct hdcp2_tx_caps tx_caps;);
__KPI_WIRED(close_session_in, );
__KPI_WIRED(close_session_out, );
__KPI_WIRED(verify_receiver_cert_in, struct hdcp2_cert_rx cert_rx; u8 r_rx[HDCP_2_2_RRX_LEN]; u8 rx_caps[HDCP_2_2_RXCAPS_LEN];);
__KPI_WIRED(verify_receiver_cert_out, u8 km_stored; u8 reserved[3]; union encrypted_buff ekm_buff;);
__KPI_WIRED(ake_send_hprime_in, u8 h_prime[HDCP_2_2_H_PRIME_LEN];);
__KPI_WIRED(ake_send_hprime_out, );
__KPI_WIRED(ake_send_pairing_info_in, u8 e_kh_km[HDCP_2_2_E_KH_KM_LEN];);
__KPI_WIRED(ake_send_pairing_info_out, );
__KPI_WIRED(init_locality_check_in, );
__KPI_WIRED(init_locality_check_out, u8 r_n[HDCP_2_2_RN_LEN];);
__KPI_WIRED(validate_locality_in, u8 l_prime[HDCP_2_2_L_PRIME_LEN];);
__KPI_WIRED(validate_locality_out, );
__KPI_WIRED(get_session_key_in, );
__KPI_WIRED(get_session_key_out, u8 e_dkey_ks[HDCP_2_2_E_DKEY_KS_LEN]; u8 r_iv[HDCP_2_2_RIV_LEN];);
__KPI_WIRED(enable_auth_in, u8 stream_type;);
__KPI_WIRED(enable_auth_out, );
__KPI_WIRED(verify_repeater_in, u8 rx_info[HDCP_2_2_RXINFO_LEN]; u8 seq_num_v[HDCP_2_2_SEQ_NUM_LEN];
	u8 v_prime[HDCP_2_2_V_PRIME_HALF_LEN]; u8 receiver_ids[HDCP_2_2_RECEIVER_IDS_MAX_LEN];);
__KPI_WIRED(verify_repeater_out, u8 content_type_supported; u8 v[HDCP_2_2_V_PRIME_HALF_LEN];);
__KPI_WIRED(repeater_auth_stream_req_in, u8 seq_num_m[HDCP_2_2_SEQ_NUM_LEN]; u8 m_prime[HDCP_2_2_MPRIME_LEN]; __be16 k;
	struct hdcp2_streamid_type streams[];);
__KPI_WIRED(repeater_auth_stream_req_out, );

#endif
