/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the DP AUX channel.
 *
 * Follows Linux 6.6's drivers/gpu/drm/i915/display/intel_dp_aux.c
 * (much of it first written after v4.4); the notices are in NOTICE,
 * UPSTREAM says which functions correspond.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

ulong
ilk_aux_ctl_reg(int port)
{
	return port == 0 ? DP_AUX_CH_CTL(AUX_CH_A) : PCH_DP_AUX_CH_CTL(port);
}

/*
 * one AUX transaction: send n bytes, receive up to nr (intel_dp_aux_ch);
 * the bytes received or -1
 */
int
intel_dp_aux_xfer(int port, uchar *send, int n, uchar *recv, int nr)
{
	ulong ctl;
	u32int st, sendctl, w;
	int i, j, div, try, got;
	vlong t;

	ctl = ilk_aux_ctl_reg(port);
	for(try = 0; try < 3 && (rd(ctl) & DP_AUX_CH_CTL_SEND_BUSY); try++)
		sleep(1);
	if(try == 3)
		return -1;
	if(port == 0)
		div = (cdclk + 1000) / 2000;
	else
		div = ((rd(PCH_RAWCLK_FREQ) & 0x3FF) + 1) / 2;
	sendctl = DP_AUX_CH_CTL_SEND_BUSY | DP_AUX_CH_CTL_DONE |
		DP_AUX_CH_CTL_TIME_OUT_ERROR |
		(port == 0 ? DP_AUX_CH_CTL_TIME_OUT_600us : 0) |
		DP_AUX_CH_CTL_RECEIVE_ERROR | DP_AUX_CH_CTL_MESSAGE_SIZE(n) |
			DP_AUX_CH_CTL_PRECHARGE_2US(5) | div;
	st = 0;
	for(try = 0; try < 5; try++){
		for(i = 0; i < n; i += 4){
			w = 0;
			for(j = 0; j < 4; j++)
				w |= (i+j < n ? send[i+j] : 0) << (24 - 8*j);
			wr(ctl + 4 + i, w);
		}
		wr(ctl, sendctl);
		for(t = nsec(); ((st = rd(ctl)) & DP_AUX_CH_CTL_SEND_BUSY) && nsec() - t < 10*1000000LL; )
			sleep(0);
		wr(ctl, st | DP_AUX_CH_CTL_DONE | DP_AUX_CH_CTL_TIME_OUT_ERROR |
			DP_AUX_CH_CTL_RECEIVE_ERROR);
		if(st & DP_AUX_CH_CTL_TIME_OUT_ERROR)
			continue;
		if(st & DP_AUX_CH_CTL_RECEIVE_ERROR){
			microwait(500);
			continue;
		}
		if(st & DP_AUX_CH_CTL_DONE)
			break;
	}
	if((st & DP_AUX_CH_CTL_DONE) == 0 || (st & (DP_AUX_CH_CTL_TIME_OUT_ERROR|DP_AUX_CH_CTL_RECEIVE_ERROR)))
		return -1;
	got = REG_FIELD_GET(DP_AUX_CH_CTL_MESSAGE_SIZE_MASK, st);
	if(got > nr)
		got = nr;
	for(i = 0; i < got; i += 4){
		w = rd(ctl + 4 + i);
		for(j = 0; j < 4 && i+j < got; j++)
			recv[i+j] = w >> (24 - 8*j);
	}
	return got;
}

/*
 * an AUX message (intel_dp_aux_transfer): request, address, size bytes
 * of data; the reply nibble, or -1; reads fill buf
 */
int
intel_dp_aux_transfer(int port, int req, int addr, uchar *buf, int size)
{
	uchar tx[20], rx[20];
	int n, nt, r;

	tx[0] = req<<4 | (addr>>16 & 0xF);
	tx[1] = addr>>8;
	tx[2] = addr;
	tx[3] = size - 1;
	if(req & 1){	/* reads */
		nt = size ? 4 : 3;
		if((n = intel_dp_aux_xfer(port, tx, nt, rx, size+1)) <= 0)
			return -1;
		r = rx[0]>>4;
		if(n-1 > 0 && buf != nil)
			memmove(buf, rx+1, n-1 < size ? n-1 : size);
		return r;
	}
	nt = size ? 4 + size : 3;
	if(size)
		memmove(tx+4, buf, size);
	if((n = intel_dp_aux_xfer(port, tx, nt, rx, 2)) <= 0)
		return -1;
	return rx[0]>>4;
}

/*
 * EDID bytes from offset over AUX: the offset written, then read, 16
 * bytes a message (drm_dp_i2c_xfer: a bare address to start each
 * message, one without MOT to end)
 */
int
auxedid(int port, int off, uchar *buf, int n)
{
	uchar o;
	int i, k, r;

	o = off;
	r = -1;
	if(drm_dp_i2c_xfer(port, DP_AUX_I2C_WRITE|DP_AUX_I2C_MOT, nil, 0) < 0 || drm_dp_i2c_xfer(port, DP_AUX_I2C_WRITE|DP_AUX_I2C_MOT, &o, 1) < 0)
		goto End;
	if(drm_dp_i2c_xfer(port, DP_AUX_I2C_READ|DP_AUX_I2C_MOT, nil, 0) < 0)
		goto End;
	for(i = 0; i < n; i += k){
		k = n - i < 16 ? n - i : 16;
		if(drm_dp_i2c_xfer(port, DP_AUX_I2C_READ|DP_AUX_I2C_MOT, buf+i, k) < 0)
			goto End;
	}
	r = 0;
End:
	drm_dp_i2c_xfer(port, DP_AUX_I2C_READ, nil, 0);
	return r;
}
