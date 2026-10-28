/* Copyright 2003 Tungsten Graphics, Inc., Cedar Park, Texas.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * From Linux 6.6, drivers/gpu/drm/i915/i915_reg.h,
 * for aux/intelgpu: the definitions it uses, as they are there.
 */

#ifndef _INTELGPU_I915_REG_H_
#define _INTELGPU_I915_REG_H_

#define RENDER_RING_BASE	0x02000
#define BLT_RING_BASE		0x22000
#define HSW_GTT_CACHE_EN	_MMIO(0x4024)
#define   GTT_CACHE_EN_ALL	0xF0007FFF
#define INSTPM	        _MMIO(0x20c0)
#define   INSTPM_FORCE_ORDERING				(1 << 7) /* GEN6+ */
#define GT_CONTEXT_SWITCH_INTERRUPT		(1 <<  8)
#define GT_RENDER_USER_INTERRUPT		(1 <<  0)
#define GEN7_FF_THREAD_MODE		_MMIO(0x20a0)
#define   GEN8_FF_DS_REF_CNT_FFME	(1 << 19)
#define   GEN7_FF_VS_REF_CNT_FFME	(1 << 15)
#define _TRANS_HTOTAL_A		0x60000
#define _TRANS_HBLANK_A		0x60004
#define _TRANS_HSYNC_A		0x60008
#define _TRANS_VTOTAL_A		0x6000c
#define _TRANS_VBLANK_A		0x60010
#define _TRANS_VSYNC_A		0x60014
#define _PIPEASRC		0x6001c
#define _TRANS_VSYNCSHIFT_A	0x60028
#define _TRANS_MULT_A		0x6002c
#define TRANS_HTOTAL(trans)	_MMIO_TRANS2((trans), _TRANS_HTOTAL_A)
#define TRANS_HBLANK(trans)	_MMIO_TRANS2((trans), _TRANS_HBLANK_A)
#define TRANS_HSYNC(trans)	_MMIO_TRANS2((trans), _TRANS_HSYNC_A)
#define TRANS_VTOTAL(trans)	_MMIO_TRANS2((trans), _TRANS_VTOTAL_A)
#define TRANS_VBLANK(trans)	_MMIO_TRANS2((trans), _TRANS_VBLANK_A)
#define TRANS_VSYNC(trans)	_MMIO_TRANS2((trans), _TRANS_VSYNC_A)
#define TRANS_VSYNCSHIFT(trans)	_MMIO_TRANS2((trans), _TRANS_VSYNCSHIFT_A)
#define PIPESRC(pipe)		_MMIO_TRANS2((pipe), _PIPEASRC)
#define TRANS_MULT(trans)	_MMIO_TRANS2((trans), _TRANS_MULT_A)
#define _TRANSACONF		0x70008
#define   TRANSCONF_ENABLE			REG_BIT(31)
#define   TRANSCONF_STATE_ENABLE			REG_BIT(30) /* i965+ */
#define TRANSCONF(trans)	_MMIO_PIPE2((trans), _TRANSACONF)
#define _PIPE_MISC_A			0x70030
#define _PIPE_MISC_B			0x71030
#define PIPE_MISC(pipe)			_MMIO_PIPE(pipe, _PIPE_MISC_A, _PIPE_MISC_B)
#define _WM0_PIPEA_ILK		0x45100
#define _WM0_PIPEB_ILK		0x45104
#define _WM0_PIPEC_IVB		0x45200
#define WM0_PIPE_ILK(pipe)	_MMIO_PIPE3((pipe), _WM0_PIPEA_ILK, \
					    _WM0_PIPEB_ILK, _WM0_PIPEC_IVB)
#define WM1_LP_ILK		_MMIO(0x45108)
#define WM2_LP_ILK		_MMIO(0x4510c)
#define WM3_LP_ILK		_MMIO(0x45110)
#define WM1S_LP_ILK		_MMIO(0x45120)
#define _DSPACNTR				0x70180
#define   DISP_ENABLE			REG_BIT(31)
#define   DISP_FORMAT_MASK		REG_GENMASK(29, 26)
#define   DISP_FORMAT_BGRX888		REG_FIELD_PREP(DISP_FORMAT_MASK, 6)
#define   DISP_TILED			REG_BIT(10)
#define _DSPASTRIDE				0x70188
#define _DSPASURF				0x7019C /* 965+ only */
#define _DSPAOFFSET				0x701A4 /* HSW */
#define _DSPASURFLIVE				0x701AC
#define DSPCNTR(plane)		_MMIO_PIPE2(plane, _DSPACNTR)
#define DSPSTRIDE(plane)	_MMIO_PIPE2(plane, _DSPASTRIDE)
#define DSPSURF(plane)		_MMIO_PIPE2(plane, _DSPASURF)
#define DSPOFFSET(plane)	_MMIO_PIPE2(plane, _DSPAOFFSET)
#define DSPSURFLIVE(plane)	_MMIO_PIPE2(plane, _DSPASURFLIVE)
#define _PIPEA_LINK_M1		0x60040
#define _PIPEA_LINK_N1		0x60044
#define PIPE_LINK_M1(tran) _MMIO_TRANS2(tran, _PIPEA_LINK_M1)
#define PIPE_LINK_N1(tran) _MMIO_TRANS2(tran, _PIPEA_LINK_N1)
#define _GAMMA_MODE_A		0x4a480
#define _GAMMA_MODE_B		0x4ac80
#define GAMMA_MODE(pipe) _MMIO_PIPE(pipe, _GAMMA_MODE_A, _GAMMA_MODE_B)
#define GEN8_MASTER_IRQ			_MMIO(0x44200)
#define  GEN8_MASTER_IRQ_CONTROL	(1 << 31)
#define GEN8_GT_IMR(which) _MMIO(0x44304 + (0x10 * (which)))
#define GEN8_GT_IIR(which) _MMIO(0x44308 + (0x10 * (which)))
#define GEN8_GT_IER(which) _MMIO(0x4430c + (0x10 * (which)))
#define GEN8_RCS_IRQ_SHIFT 0
#define GEN8_BCS_IRQ_SHIFT 16
#define FUSE_STRAP		_MMIO(0x42014)
#define SDEISR  _MMIO(0xc4000)
#define PCH_RAWCLK_FREQ         _MMIO(0xc6204)
#define _PCH_DPB_AUX_CH_CTL	0xe4110
#define _PCH_DPC_AUX_CH_CTL	0xe4210
#define PCH_DP_AUX_CH_CTL(aux_ch)		_MMIO_PORT((aux_ch) - AUX_CH_B, _PCH_DPB_AUX_CH_CTL, _PCH_DPC_AUX_CH_CTL)
#define HSW_PWR_WELL_CTL2			_MMIO(0x45404)
#define   HSW_PWR_WELL_CTL_REQ(pw_idx)		(0x2 << ((pw_idx) * 2))
#define   HSW_PWR_WELL_CTL_STATE(pw_idx)	(0x1 << ((pw_idx) * 2))
#define   HSW_PW_CTL_IDX_GLOBAL			15
#define _TRANS_DDI_FUNC_CTL_A		0x60400
#define TRANS_DDI_FUNC_CTL(tran) _MMIO_TRANS2(tran, _TRANS_DDI_FUNC_CTL_A)
#define  TRANS_DDI_FUNC_ENABLE		(1 << 31)
#define  TRANS_DDI_MODE_SELECT_DVI	(1 << 24)
#define  TRANS_DDI_BPC_8		(0 << 20)
#define  TRANS_DDI_PVSYNC		(1 << 17)
#define  TRANS_DDI_PHSYNC		(1 << 16)
#define  TRANS_DDI_EDP_INPUT_MASK	(7 << 12)
#define _DDI_BUF_CTL_A				0x64000
#define _DDI_BUF_CTL_B				0x64100
#define DDI_BUF_CTL(port) _MMIO_PORT(port, _DDI_BUF_CTL_A, _DDI_BUF_CTL_B)
#define  DDI_BUF_CTL_ENABLE			(1 << 31)
#define  DDI_BUF_IS_IDLE			(1 << 7)
#define  DDI_INIT_DISPLAY_DETECTED		(1 << 0)
#define _DDI_BUF_TRANS_A		0x64E00
#define _DDI_BUF_TRANS_B		0x64E60
#define DDI_BUF_TRANS_LO(port, i)	_MMIO(_PORT(port, _DDI_BUF_TRANS_A, _DDI_BUF_TRANS_B) + (i) * 8)
#define DDI_BUF_TRANS_HI(port, i)	_MMIO(_PORT(port, _DDI_BUF_TRANS_A, _DDI_BUF_TRANS_B) + (i) * 8 + 4)
#define _WRPLL_CTL1			0x46040
#define _WRPLL_CTL2			0x46060
#define WRPLL_CTL(pll)			_MMIO_PIPE(pll, _WRPLL_CTL1, _WRPLL_CTL2)
#define  WRPLL_PLL_ENABLE		(1 << 31)
#define  WRPLL_REF_LCPLL		(3 << 28)
#define _PORT_CLK_SEL_A			0x46100
#define _PORT_CLK_SEL_B			0x46104
#define PORT_CLK_SEL(port) _MMIO_PORT(port, _PORT_CLK_SEL_A, _PORT_CLK_SEL_B)
#define  PORT_CLK_SEL_MASK		REG_GENMASK(31, 29)
#define  PORT_CLK_SEL_WRPLL(pll)	REG_FIELD_PREP(PORT_CLK_SEL_MASK, 4 + (pll))
#define  PORT_CLK_SEL_NONE		REG_FIELD_PREP(PORT_CLK_SEL_MASK, 7)
#define _TRANS_CLK_SEL_A		0x46140
#define _TRANS_CLK_SEL_B		0x46144
#define TRANS_CLK_SEL(tran) _MMIO_TRANS(tran, _TRANS_CLK_SEL_A, _TRANS_CLK_SEL_B)
#define  TRANS_CLK_SEL_PORT(x)		(((x) + 1) << 29)
#define LCPLL_CTL			_MMIO(0x130040)
#define SFUSE_STRAP			_MMIO(0xc2014)

#endif
