/*
 * Copyright (C) 2016 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */


/* Instructions, enums and structures for XE3.
 *
 * This file has been generated, do not hand edit.
 */

#ifndef GFX30_XE3_PACK_H
#define GFX30_XE3_PACK_H

#ifndef __OPENCL_VERSION__
#include <stdio.h>
#include "util/bitpack_helpers.h"
#include "genX_helpers.h"
#else
#include "genX_cl_helpers.h"
#endif


enum GFX30_3D_Color_Buffer_Blend_Factor {
   BLENDFACTOR_ONE                      =      1,
   BLENDFACTOR_SRC_COLOR                =      2,
   BLENDFACTOR_SRC_ALPHA                =      3,
   BLENDFACTOR_DST_ALPHA                =      4,
   BLENDFACTOR_DST_COLOR                =      5,
   BLENDFACTOR_SRC_ALPHA_SATURATE       =      6,
   BLENDFACTOR_CONST_COLOR              =      7,
   BLENDFACTOR_CONST_ALPHA              =      8,
   BLENDFACTOR_SRC1_COLOR               =      9,
   BLENDFACTOR_SRC1_ALPHA               =     10,
   BLENDFACTOR_ZERO                     =     17,
   BLENDFACTOR_INV_SRC_COLOR            =     18,
   BLENDFACTOR_INV_SRC_ALPHA            =     19,
   BLENDFACTOR_INV_DST_ALPHA            =     20,
   BLENDFACTOR_INV_DST_COLOR            =     21,
   BLENDFACTOR_INV_CONST_COLOR          =     23,
   BLENDFACTOR_INV_CONST_ALPHA          =     24,
   BLENDFACTOR_INV_SRC1_COLOR           =     25,
   BLENDFACTOR_INV_SRC1_ALPHA           =     26,
};

enum GFX30_3D_Color_Buffer_Blend_Function {
   BLENDFUNCTION_ADD                    =      0,
   BLENDFUNCTION_SUBTRACT               =      1,
   BLENDFUNCTION_REVERSE_SUBTRACT       =      2,
   BLENDFUNCTION_MIN                    =      3,
   BLENDFUNCTION_MAX                    =      4,
};

enum GFX30_3D_Compare_Function {
   COMPAREFUNCTION_ALWAYS               =      0,
   COMPAREFUNCTION_NEVER                =      1,
   COMPAREFUNCTION_LESS                 =      2,
   COMPAREFUNCTION_EQUAL                =      3,
   COMPAREFUNCTION_LEQUAL               =      4,
   COMPAREFUNCTION_GREATER              =      5,
   COMPAREFUNCTION_NOTEQUAL             =      6,
   COMPAREFUNCTION_GEQUAL               =      7,
};

enum GFX30_3D_Logic_Op_Function {
   LOGICOP_CLEAR                        =      0,
   LOGICOP_NOR                          =      1,
   LOGICOP_AND_INVERTED                 =      2,
   LOGICOP_COPY_INVERTED                =      3,
   LOGICOP_AND_REVERSE                  =      4,
   LOGICOP_INVERT                       =      5,
   LOGICOP_XOR                          =      6,
   LOGICOP_NAND                         =      7,
   LOGICOP_AND                          =      8,
   LOGICOP_EQUIV                        =      9,
   LOGICOP_NOOP                         =     10,
   LOGICOP_OR_INVERTED                  =     11,
   LOGICOP_COPY                         =     12,
   LOGICOP_OR_REVERSE                   =     13,
   LOGICOP_OR                           =     14,
   LOGICOP_SET                          =     15,
};

enum GFX30_3D_Prim_Topo_Type {
   _3DPRIM_POINTLIST                    =      1,
   _3DPRIM_LINELIST                     =      2,
   _3DPRIM_LINESTRIP                    =      3,
   _3DPRIM_TRILIST                      =      4,
   _3DPRIM_TRISTRIP                     =      5,
   _3DPRIM_TRIFAN                       =      6,
   _3DPRIM_QUADLIST                     =      7,
   _3DPRIM_QUADSTRIP                    =      8,
   _3DPRIM_LINELIST_ADJ                 =      9,
   _3DPRIM_LINESTRIP_ADJ                =     10,
   _3DPRIM_TRILIST_ADJ                  =     11,
   _3DPRIM_TRISTRIP_ADJ                 =     12,
   _3DPRIM_TRISTRIP_REVERSE             =     13,
   _3DPRIM_POLYGON                      =     14,
   _3DPRIM_RECTLIST                     =     15,
   _3DPRIM_LINELOOP                     =     16,
   _3DPRIM_POINTLIST_BF                 =     17,
   _3DPRIM_LINESTRIP_CONT               =     18,
   _3DPRIM_LINESTRIP_BF                 =     19,
   _3DPRIM_LINESTRIP_CONT_BF            =     20,
   _3DPRIM_TRIFAN_NOSTIPPLE             =     22,
   _3DPRIM_PATCHLIST_1                  =     32,
   _3DPRIM_PATCHLIST_2                  =     33,
   _3DPRIM_PATCHLIST_3                  =     34,
   _3DPRIM_PATCHLIST_4                  =     35,
   _3DPRIM_PATCHLIST_5                  =     36,
   _3DPRIM_PATCHLIST_6                  =     37,
   _3DPRIM_PATCHLIST_7                  =     38,
   _3DPRIM_PATCHLIST_8                  =     39,
   _3DPRIM_PATCHLIST_9                  =     40,
   _3DPRIM_PATCHLIST_10                 =     41,
   _3DPRIM_PATCHLIST_11                 =     42,
   _3DPRIM_PATCHLIST_12                 =     43,
   _3DPRIM_PATCHLIST_13                 =     44,
   _3DPRIM_PATCHLIST_14                 =     45,
   _3DPRIM_PATCHLIST_15                 =     46,
   _3DPRIM_PATCHLIST_16                 =     47,
   _3DPRIM_PATCHLIST_17                 =     48,
   _3DPRIM_PATCHLIST_18                 =     49,
   _3DPRIM_PATCHLIST_19                 =     50,
   _3DPRIM_PATCHLIST_20                 =     51,
   _3DPRIM_PATCHLIST_21                 =     52,
   _3DPRIM_PATCHLIST_22                 =     53,
   _3DPRIM_PATCHLIST_23                 =     54,
   _3DPRIM_PATCHLIST_24                 =     55,
   _3DPRIM_PATCHLIST_25                 =     56,
   _3DPRIM_PATCHLIST_26                 =     57,
   _3DPRIM_PATCHLIST_27                 =     58,
   _3DPRIM_PATCHLIST_28                 =     59,
   _3DPRIM_PATCHLIST_29                 =     60,
   _3DPRIM_PATCHLIST_30                 =     61,
   _3DPRIM_PATCHLIST_31                 =     62,
   _3DPRIM_PATCHLIST_32                 =     63,
};

enum GFX30_3D_Stencil_Operation {
   STENCILOP_KEEP                       =      0,
   STENCILOP_ZERO                       =      1,
   STENCILOP_REPLACE                    =      2,
   STENCILOP_INCRSAT                    =      3,
   STENCILOP_DECRSAT                    =      4,
   STENCILOP_INCR                       =      5,
   STENCILOP_DECR                       =      6,
   STENCILOP_INVERT                     =      7,
};

enum GFX30_3D_Vertex_Component_Control {
   VFCOMP_NOSTORE                       =      0,
   VFCOMP_STORE_SRC                     =      1,
   VFCOMP_STORE_0                       =      2,
   VFCOMP_STORE_1_FP                    =      3,
   VFCOMP_STORE_1_INT                   =      4,
   VFCOMP_STORE_PID                     =      7,
};

enum GFX30_Atomic_OPCODE {
   MI_ATOMIC_OP_AND                     =      1,
   MI_ATOMIC_OP_OR                      =      2,
   MI_ATOMIC_OP_XOR                     =      3,
   MI_ATOMIC_OP_MOVE                    =      4,
   MI_ATOMIC_OP_INC                     =      5,
   MI_ATOMIC_OP_DEC                     =      6,
   MI_ATOMIC_OP_ADD                     =      7,
   MI_ATOMIC_OP_SUB                     =      8,
   MI_ATOMIC_OP_RSUB                    =      9,
   MI_ATOMIC_OP_IMAX                    =     10,
   MI_ATOMIC_OP_IMIN                    =     11,
   MI_ATOMIC_OP_UMAX                    =     12,
   MI_ATOMIC_OP_UMIN                    =     13,
   MI_ATOMIC_OP_CMP_WR                  =     14,
   MI_ATOMIC_OP_PREDEC                  =     15,
   MI_ATOMIC_OP_AND8B                   =     33,
   MI_ATOMIC_OP_OR8B                    =     34,
   MI_ATOMIC_OP_XOR8B                   =     35,
   MI_ATOMIC_OP_MOVE8B                  =     36,
   MI_ATOMIC_OP_INC8B                   =     37,
   MI_ATOMIC_OP_DEC8B                   =     38,
   MI_ATOMIC_OP_ADD8B                   =     39,
   MI_ATOMIC_OP_SUB8B                   =     40,
   MI_ATOMIC_OP_RSUB8B                  =     41,
   MI_ATOMIC_OP_IMAX8B                  =     42,
   MI_ATOMIC_OP_IMIN8B                  =     43,
   MI_ATOMIC_OP_UMAX8B                  =     44,
   MI_ATOMIC_OP_UMIN8B                  =     45,
   MI_ATOMIC_OP_CMP_WR8B                =     46,
   MI_ATOMIC_OP_PREDEC8B                =     47,
   MI_ATOMIC_OP_CMP_WR16B               =     78,
};

enum GFX30_Attribute_Component_Format {
   ACF_DISABLED                         =      0,
   ACF_XY                               =      1,
   ACF_XYZ                              =      2,
   ACF_XYZW                             =      3,
};

enum GFX30_FRAME_BITRATE_UNITS {
   FRAME_BR__32BYTE_UNITS               =      0,
   FRAME_BR__4KBYTE_UNITS               =      1,
};

enum GFX30_L1_CACHE_CONTROL {
   L1CC_WBP                             =      0,
   L1CC_UC                              =      1,
   L1CC_WB                              =      2,
   L1CC_WT                              =      3,
   L1CC_WS                              =      4,
};

enum GFX30_PREF_SLM_ALLOCATION_SIZE {
   SLM_ENCODES_0K                       =      0,
   SLM_ENCODES_16K                      =      1,
   SLM_ENCODES_32K                      =      2,
   SLM_ENCODES_64K                      =      3,
   SLM_ENCODES_96K                      =      4,
   SLM_ENCODES_128K                     =      5,
   SLM_ENCODES_160K                     =      6,
   SLM_ENCODES_192K                     =      7,
   SLM_ENCODES_224K                     =      8,
   SLM_ENCODES_256K                     =      9,
   SLM_ENCODES_384K                     =     10,
};

enum GFX30_RESOURCE_BARRIER_STAGE {
   RESOURCE_BARRIER_STAGE_NONE          =      0,
   RESOURCE_BARRIER_STAGE_TOP           =      1,
   RESOURCE_BARRIER_STAGE_COLOR         =      2,
   RESOURCE_BARRIER_STAGE_GPGPU         =      4,
   RESOURCE_BARRIER_STAGE_COLORANDCOMPUTE =      6,
   RESOURCE_BARRIER_STAGE_GEOM          =     16,
   RESOURCE_BARRIER_STAGE_GEOMETRYANDCOMPUTE =     20,
   RESOURCE_BARRIER_STAGE_RASTER        =     32,
   RESOURCE_BARRIER_STAGE_DEPTH         =     64,
   RESOURCE_BARRIER_STAGE_PIXEL         =    128,
};

enum GFX30_RESOURCE_BARRIER_TYPE {
   RESOURCE_BARRIER_TYPE_IMMEDIATE      =      0,
   RESOURCE_BARRIER_TYPE_SIGNAL         =      1,
   RESOURCE_BARRIER_TYPE_WAIT           =      2,
   RESOURCE_BARRIER_TYPE_UAV            =      3,
};

enum GFX30_STATE_CPS_COMBINER_OPCODE {
   CPS_COMB_OP_PASSTHROUGH              =      0,
   CPS_COMB_OP_OVERRIDE                 =      1,
   CPS_COMB_OP_HIGH_QUALITY             =      2,
   CPS_COMB_OP_LOW_QUALITY              =      3,
   CPS_COMB_OP_RELATIVE                 =      4,
};

enum GFX30_STATE_CPS_SIZE {
   CPSIZE_1                             =      0,
   CPSIZE_2                             =      1,
   CPSIZE_4                             =      2,
};

enum GFX30_STATE_SURFACE_TYPE {
   SURFTYPE_1D                          =      0,
   SURFTYPE_2D                          =      1,
   SURFTYPE_3D                          =      2,
   SURFTYPE_CUBE                        =      3,
   SURFTYPE_BUFFER                      =      4,
   SURFTYPE_RES5                        =      5,
   SURFTYPE_SCRATCH                     =      6,
   SURFTYPE_NULL                        =      7,
};

enum GFX30_ShaderChannelSelect {
   SCS_ZERO                             =      0,
   SCS_ONE                              =      1,
   SCS_RED                              =      4,
   SCS_GREEN                            =      5,
   SCS_BLUE                             =      6,
   SCS_ALPHA                            =      7,
};

enum GFX30_TextureCoordinateMode {
   TCM_WRAP                             =      0,
   TCM_MIRROR                           =      1,
   TCM_CLAMP                            =      2,
   TCM_CUBE                             =      3,
   TCM_CLAMP_BORDER                     =      4,
   TCM_MIRROR_ONCE                      =      5,
   TCM_HALF_BORDER                      =      6,
   TCM_MIRROR_101                       =      7,
};

enum GFX30_UNIFIED_COMPRESSION_FORMAT {
   CMF_R8                               =      0,
   CMF_R8_G8                            =      1,
   CMF_R8_G8_B8_A8                      =      2,
   CMF_R10_G10_B10_A2                   =      3,
   CMF_R11_G11_B10                      =      4,
   CMF_R16                              =      5,
   CMF_R16_G16                          =      6,
   CMF_R16_G16_B16_A16                  =      7,
   CMF_R32                              =      8,
   CMF_R32_G32                          =      9,
   CMF_R32_G32_B32_A32                  =     10,
   CMF_Y16_U16_Y16_V16                  =     11,
   CMF_ML8                              =     15,
};

enum GFX30_WRAP_SHORTEST_ENABLE {
   WSE_X                                =      1,
   WSE_Y                                =      2,
   WSE_XY                               =      3,
   WSE_Z                                =      4,
   WSE_XZ                               =      5,
   WSE_YZ                               =      6,
   WSE_XYZ                              =      7,
   WSE_W                                =      8,
   WSE_XW                               =      9,
   WSE_YW                               =     10,
   WSE_XYW                              =     11,
   WSE_ZW                               =     12,
   WSE_XZW                              =     13,
   WSE_YZW                              =     14,
   WSE_XYZW                             =     15,
};

#define GFX30_3DSTATE_CONSTANT_ALL_DATA_length      2
struct GFX30_3DSTATE_CONSTANT_ALL_DATA {
   uint32_t                             ConstantBufferReadLength;
   __gen_address_type                   PointerToConstantBuffer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_ALL_DATA_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_ALL_DATA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->ConstantBufferReadLength, 0, 4);
   const uint64_t v0_address =
      __gen_address(data, &dw[0], values->PointerToConstantBuffer, v0, 5, 63);
   dw[0] = v0_address;
   dw[1] = (v0_address >> 32) | (v0 >> 32);
}

#define GFX30_3DSTATE_CONSTANT_BODY_length     10
struct GFX30_3DSTATE_CONSTANT_BODY {
   uint32_t                             ReadLength[4];
   __gen_address_type                   Buffer[4];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ReadLength[0], 0, 15) |
      util_bitpack_uint(values->ReadLength[1], 16, 31);

   dw[1] =
      util_bitpack_uint(values->ReadLength[2], 0, 15) |
      util_bitpack_uint(values->ReadLength[3], 16, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->Buffer[0], 0, 5, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->Buffer[1], 0, 5, 63);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;

   const uint64_t v6_address =
      __gen_address(data, &dw[6], values->Buffer[2], 0, 5, 63);
   dw[6] = v6_address;
   dw[7] = v6_address >> 32;

   const uint64_t v8_address =
      __gen_address(data, &dw[8], values->Buffer[3], 0, 5, 63);
   dw[8] = v8_address;
   dw[9] = v8_address >> 32;
}

#define GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_length      7
struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY {
   bool                                 StreamOutputBufferOffsetAddressEnable;
   bool                                 StreamOffsetWriteEnable;
   uint32_t                             MOCS;
   bool                                 SOBufferEnable;
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             SurfaceSize;
   __gen_address_type                   StreamOutputBufferOffsetAddress;
   uint32_t                             StreamOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->StreamOutputBufferOffsetAddressEnable, 20, 20) |
      util_bitpack_uint(values->StreamOffsetWriteEnable, 21, 21) |
      util_bitpack_uint_nonzero(values->MOCS, 22, 28) |
      util_bitpack_uint(values->SOBufferEnable, 31, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->SurfaceBaseAddress, 0, 2, 47);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;

   dw[3] =
      util_bitpack_uint(values->SurfaceSize, 0, 29);

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->StreamOutputBufferOffsetAddress, 0, 2, 47);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;

   dw[6] =
      util_bitpack_uint(values->StreamOffset, 0, 31);
}

#define GFX30_BINDING_TABLE_EDIT_ENTRY_length      1
struct GFX30_BINDING_TABLE_EDIT_ENTRY {
   uint64_t                             SurfaceStatePointer;
   uint32_t                             BindingTableIndex;
};

static inline __attribute__((always_inline)) void
GFX30_BINDING_TABLE_EDIT_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                                    __attribute__((unused)) void * restrict dst,
                                    __attribute__((unused)) const struct GFX30_BINDING_TABLE_EDIT_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->SurfaceStatePointer, 0, 15) |
      util_bitpack_uint(values->BindingTableIndex, 16, 23);
}

#define GFX30_BINDING_TABLE_STATE_length       1
struct GFX30_BINDING_TABLE_STATE {
   uint64_t                             SurfaceStatePointer;
};

static inline __attribute__((always_inline)) void
GFX30_BINDING_TABLE_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_BINDING_TABLE_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->SurfaceStatePointer, 6, 31);
}

#define GFX30_BLEND_STATE_ENTRY_length         2
struct GFX30_BLEND_STATE_ENTRY {
   bool                                 WriteDisableBlue;
   bool                                 WriteDisableGreen;
   bool                                 WriteDisableRed;
   bool                                 WriteDisableAlpha;
   enum GFX30_3D_Color_Buffer_Blend_Function AlphaBlendFunction;
   enum GFX30_3D_Color_Buffer_Blend_Factor DestinationAlphaBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Factor SourceAlphaBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Function ColorBlendFunction;
   enum GFX30_3D_Color_Buffer_Blend_Factor DestinationBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Factor SourceBlendFactor;
   bool                                 ColorBufferBlendEnable;
   bool                                 PostBlendColorClampEnable;
   bool                                 PreBlendColorClampEnable;
   uint32_t                             ColorClampRange;
#define COLORCLAMP_UNORM                         0
#define COLORCLAMP_SNORM                         1
#define COLORCLAMP_RTFORMAT                      2
   bool                                 PreBlendSourceOnlyClampEnable;
   bool                                 SimpleFloatBlendEnable;
   enum GFX30_3D_Logic_Op_Function      LogicOpFunction;
   bool                                 LogicOpEnable;
};

static inline __attribute__((always_inline)) void
GFX30_BLEND_STATE_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_BLEND_STATE_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->WriteDisableBlue, 0, 0) |
      util_bitpack_uint(values->WriteDisableGreen, 1, 1) |
      util_bitpack_uint(values->WriteDisableRed, 2, 2) |
      util_bitpack_uint(values->WriteDisableAlpha, 3, 3) |
      util_bitpack_uint(values->AlphaBlendFunction, 5, 7) |
      util_bitpack_uint(values->DestinationAlphaBlendFactor, 8, 12) |
      util_bitpack_uint(values->SourceAlphaBlendFactor, 13, 17) |
      util_bitpack_uint(values->ColorBlendFunction, 18, 20) |
      util_bitpack_uint(values->DestinationBlendFactor, 21, 25) |
      util_bitpack_uint(values->SourceBlendFactor, 26, 30) |
      util_bitpack_uint(values->ColorBufferBlendEnable, 31, 31);

   dw[1] =
      util_bitpack_uint(values->PostBlendColorClampEnable, 0, 0) |
      util_bitpack_uint(values->PreBlendColorClampEnable, 1, 1) |
      util_bitpack_uint(values->ColorClampRange, 2, 3) |
      util_bitpack_uint(values->PreBlendSourceOnlyClampEnable, 4, 4) |
      util_bitpack_uint(values->SimpleFloatBlendEnable, 5, 5) |
      util_bitpack_uint(values->LogicOpFunction, 27, 30) |
      util_bitpack_uint(values->LogicOpEnable, 31, 31);
}

#define GFX30_BLEND_STATE_length               1
struct GFX30_BLEND_STATE {
   uint32_t                             YDitherOffset;
   uint32_t                             XDitherOffset;
   bool                                 ColorDitherEnable;
   enum GFX30_3D_Compare_Function       AlphaTestFunction;
   bool                                 AlphaTestEnable;
   bool                                 AlphaToCoverageDitherEnable;
   bool                                 AlphaToOneEnable;
   bool                                 IndependentAlphaBlendEnable;
   bool                                 AlphaToCoverageEnable;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_BLEND_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_BLEND_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->YDitherOffset, 19, 20) |
      util_bitpack_uint(values->XDitherOffset, 21, 22) |
      util_bitpack_uint(values->ColorDitherEnable, 23, 23) |
      util_bitpack_uint(values->AlphaTestFunction, 24, 26) |
      util_bitpack_uint(values->AlphaTestEnable, 27, 27) |
      util_bitpack_uint(values->AlphaToCoverageDitherEnable, 28, 28) |
      util_bitpack_uint(values->AlphaToOneEnable, 29, 29) |
      util_bitpack_uint(values->IndependentAlphaBlendEnable, 30, 30) |
      util_bitpack_uint(values->AlphaToCoverageEnable, 31, 31);
}

#define GFX30_CC_VIEWPORT_length               2
struct GFX30_CC_VIEWPORT {
   float                                MinimumDepth;
   float                                MaximumDepth;
};

static inline __attribute__((always_inline)) void
GFX30_CC_VIEWPORT_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_CC_VIEWPORT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_float(values->MinimumDepth);

   dw[1] =
      util_bitpack_float(values->MaximumDepth);
}

#define GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_length      2
struct GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY {
   float                                Table1XFilterCoefficientn2;
   float                                Table1YFilterCoefficientn2;
   float                                Table1XFilterCoefficientn3;
   float                                Table1YFilterCoefficientn3;
   float                                Table1XFilterCoefficientn4;
   float                                Table1YFilterCoefficientn4;
   float                                Table1XFilterCoefficientn5;
   float                                Table1YFilterCoefficientn5;
};

static inline __attribute__((always_inline)) void
GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(__attribute__((unused)) __gen_user_data *data,
                                            __attribute__((unused)) void * restrict dst,
                                            __attribute__((unused)) const struct GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->Table1XFilterCoefficientn2, 0, 7, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn2, 8, 15, 6) |
      util_bitpack_sfixed(values->Table1XFilterCoefficientn3, 16, 23, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn3, 24, 31, 6);

   dw[1] =
      util_bitpack_sfixed(values->Table1XFilterCoefficientn4, 0, 7, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn4, 8, 15, 6) |
      util_bitpack_sfixed(values->Table1XFilterCoefficientn5, 16, 23, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn5, 24, 31, 6);
}

#define GFX30_COLOR_CALC_STATE_length          6
struct GFX30_COLOR_CALC_STATE {
   uint32_t                             AlphaTestFormat;
#define ALPHATEST_UNORM8                         0
#define ALPHATEST_FLOAT32                        1
   bool                                 RoundDisableFunctionDisable;
   uint32_t                             AlphaReferenceValueAsUNORM8;
   float                                AlphaReferenceValueAsFLOAT32;
   float                                BlendConstantColorRed;
   float                                BlendConstantColorGreen;
   float                                BlendConstantColorBlue;
   float                                BlendConstantColorAlpha;
};

static inline __attribute__((always_inline)) void
GFX30_COLOR_CALC_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_COLOR_CALC_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AlphaTestFormat, 0, 0) |
      util_bitpack_uint(values->RoundDisableFunctionDisable, 15, 15);

   dw[1] =
      util_bitpack_uint(values->AlphaReferenceValueAsUNORM8, 0, 31) |
      util_bitpack_float(values->AlphaReferenceValueAsFLOAT32);

   dw[2] =
      util_bitpack_float(values->BlendConstantColorRed);

   dw[3] =
      util_bitpack_float(values->BlendConstantColorGreen);

   dw[4] =
      util_bitpack_float(values->BlendConstantColorBlue);

   dw[5] =
      util_bitpack_float(values->BlendConstantColorAlpha);
}

#define GFX30_INTERFACE_DESCRIPTOR_DATA_length      8
struct GFX30_INTERFACE_DESCRIPTOR_DATA {
   uint64_t                             KernelStartPointer;
   bool                                 SoftwareExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   bool                                 SingleProgramFlow;
   uint32_t                             DenormMode;
#define Ftz                                      0
#define SetByKernel                              1
   bool                                 ThreadPreemption;
   uint32_t                             RegistersPerThread;
   uint32_t                             SamplerCount;
#define Nosamplersused                           0
#define Between1and4samplersused                 1
#define Between5and8samplersused                 2
#define Between9and12samplersused                3
#define Between13and16samplersused               4
   uint64_t                             SamplerStatePointer;
   uint32_t                             BindingTableEntryCount;
   uint64_t                             BindingTablePointer;
   uint32_t                             NumberofThreadsinGPGPUThreadGroup;
   bool                                 Threadgroupforwardprogressguarantee;
   uint32_t                             SharedLocalMemorySize;
#define Encodes0K                                0
#define Encodes1K                                1
#define Encodes2K                                2
#define Encodes4K                                3
#define Encodes8K                                4
#define Encodes16K                               5
#define Encodes32K                               6
#define Encodes64K                               7
#define Encodes96K                               10
#define Encodes128K                              11
#define Encodes192K                              12
#define Encodes256K                              13
#define Encodes384K                              14
   uint32_t                             RoundingMode;
#define RTNE                                     0
#define RU                                       1
#define RD                                       2
#define RTZ                                      3
   uint32_t                             ThreadGroupDispatchSize;
#define TGsize8                                  0
#define TGsize4                                  1
#define TGsize2                                  2
#define TGsize1                                  3
   uint32_t                             NumberOfBarriers;
#define BARRIER_SIZE_NONE                        0
#define BARRIER_SIZE_B1                          1
#define BARRIER_SIZE_B2                          2
#define BARRIER_SIZE_B4                          3
#define BARRIER_SIZE_B8                          4
#define BARRIER_SIZE_B16                         5
#define BARRIER_SIZE_B24                         6
#define BARRIER_SIZE_B32                         7
   uint32_t                             BTDMode;
   enum GFX30_PREF_SLM_ALLOCATION_SIZE  PreferredSLMAllocationSize;
};

static inline __attribute__((always_inline)) void
GFX30_INTERFACE_DESCRIPTOR_DATA_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_INTERFACE_DESCRIPTOR_DATA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->KernelStartPointer, 6, 31);

   dw[1] = 0;

   dw[2] =
      util_bitpack_uint(values->SoftwareExceptionEnable, 7, 7) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->SingleProgramFlow, 18, 18) |
      util_bitpack_uint(values->DenormMode, 19, 19) |
      util_bitpack_uint(values->ThreadPreemption, 20, 20) |
      util_bitpack_uint(values->RegistersPerThread, 26, 30);

   dw[3] =
      util_bitpack_uint(values->SamplerCount, 2, 4) |
      __gen_offset(values->SamplerStatePointer, 5, 31);

   dw[4] =
      util_bitpack_uint(values->BindingTableEntryCount, 0, 4) |
      __gen_offset(values->BindingTablePointer, 5, 20);

   dw[5] =
      util_bitpack_uint(values->NumberofThreadsinGPGPUThreadGroup, 0, 9) |
      util_bitpack_uint(values->Threadgroupforwardprogressguarantee, 13, 13) |
      util_bitpack_uint(values->SharedLocalMemorySize, 16, 20) |
      util_bitpack_uint(values->RoundingMode, 22, 23) |
      util_bitpack_uint(values->ThreadGroupDispatchSize, 26, 27) |
      util_bitpack_uint(values->NumberOfBarriers, 28, 30) |
      util_bitpack_uint(values->BTDMode, 31, 31);

   dw[6] = 0;

   dw[7] =
      util_bitpack_uint(values->PreferredSLMAllocationSize, 0, 3);
}

#define GFX30_POSTSYNC_DATA_length             5
struct GFX30_POSTSYNC_DATA {
   uint32_t                             Operation;
#define NoWrite                                  0
#define WriteImmediateData                       1
#define WriteTimestamp                           3
   bool                                 HDCPipelineFlush;
   uint32_t                             MOCS;
   bool                                 SystemMemoryFenceRequest;
   bool                                 DataportSubsliceCacheFlush;
   __gen_address_type                   DestinationAddress;
   uint64_t                             ImmediateData;
};

static inline __attribute__((always_inline)) void
GFX30_POSTSYNC_DATA_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_POSTSYNC_DATA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Operation, 0, 1) |
      util_bitpack_uint(values->HDCPipelineFlush, 2, 2) |
      util_bitpack_uint_nonzero(values->MOCS, 4, 10) |
      util_bitpack_uint(values->SystemMemoryFenceRequest, 11, 11) |
      util_bitpack_uint(values->DataportSubsliceCacheFlush, 12, 12);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->DestinationAddress, 0, 0, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;

   const uint64_t v3 =
      util_bitpack_uint(values->ImmediateData, 0, 63);
   dw[3] = v3;
   dw[4] = v3 >> 32;
}

#define GFX30_COMPUTE_WALKER_BODY_length      39
struct GFX30_COMPUTE_WALKER_BODY {
   uint32_t                             IndirectDataLength;
   bool                                 L3prefetchdisable;
   uint32_t                             PartitionDispatchParameter;
   uint32_t                             PartitionType;
#define PartitionX                               1
#define PartitionY                               2
#define PartitionZ                               3
   uint64_t                             IndirectDataStartAddress;
   uint32_t                             ComputeDispatchAllwalkerEnable;
#define Disabled                                 0
#define Enabled                                  1
   uint32_t                             ThreadGroupBatchSize;
#define TG_BATCH_1                               0
#define TG_BATCH_2                               1
#define TG_BATCH_4                               2
#define TG_BATCH_8                               3
   uint32_t                             DispatchWalkOrder;
#define LinearWalk                               0
#define Yorderwalk                               1
#define MortonWalk                               2
   uint32_t                             MessageSIMD;
#define SIMT16                                   1
#define SIMT32                                   2
   uint32_t                             TileLayout;
#define Linear                                   0
#define TileY32bpe                               1
#define TileY64bpe                               2
#define TileY128bpe                              3
   uint32_t                             WalkOrder;
#define Walk012                                  0
#define Walk021                                  1
#define Walk102                                  2
#define Walk120                                  3
#define Walk201                                  4
#define Walk210                                  5
   bool                                 EmitInlineParameter;
   uint32_t                             EmitLocal;
#define EmitNone                                 0
#define EmitX                                    1
#define EmitXY                                   3
#define EmitXYZ                                  7
   bool                                 GenerateLocalID;
   uint32_t                             SIMDSize;
#define SIMT16                                   1
#define SIMT32                                   2
   uint32_t                             ExecutionMask;
   uint32_t                             LocalXMaximum;
   uint32_t                             LocalYMaximum;
   uint32_t                             LocalZMaximum;
   uint32_t                             ThreadGroupIDXDimension;
   uint32_t                             ThreadGroupIDYDimension;
   uint32_t                             ThreadGroupIDZDimension;
   uint32_t                             ThreadGroupIDStartingX;
   uint32_t                             ThreadGroupIDStartingY;
   uint32_t                             ThreadGroupIDStartingZ;
   uint32_t                             PartitionID;
   uint32_t                             PartitionSize;
   uint32_t                             PreemptX;
   uint32_t                             PreemptY;
   uint32_t                             PreemptZ;
   uint32_t                             WalkerID;
   uint32_t                             OverdispatchTGcount;
   struct GFX30_INTERFACE_DESCRIPTOR_DATA InterfaceDescriptor;
   struct GFX30_POSTSYNC_DATA           PostSync;
   uint32_t                             InlineData[8];
};

static inline __attribute__((always_inline)) void
GFX30_COMPUTE_WALKER_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_COMPUTE_WALKER_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] = 0;

   dw[1] =
      util_bitpack_uint(values->IndirectDataLength, 0, 16) |
      util_bitpack_uint(values->L3prefetchdisable, 17, 17) |
      util_bitpack_uint(values->PartitionDispatchParameter, 18, 29) |
      util_bitpack_uint(values->PartitionType, 30, 31);

   dw[2] =
      __gen_offset(values->IndirectDataStartAddress, 6, 31);

   dw[3] =
      util_bitpack_uint(values->ComputeDispatchAllwalkerEnable, 0, 0) |
      util_bitpack_uint(values->ThreadGroupBatchSize, 3, 4) |
      util_bitpack_uint(values->DispatchWalkOrder, 5, 6) |
      util_bitpack_uint(values->MessageSIMD, 17, 18) |
      util_bitpack_uint(values->TileLayout, 19, 21) |
      util_bitpack_uint(values->WalkOrder, 22, 24) |
      util_bitpack_uint(values->EmitInlineParameter, 25, 25) |
      util_bitpack_uint(values->EmitLocal, 26, 28) |
      util_bitpack_uint(values->GenerateLocalID, 29, 29) |
      util_bitpack_uint(values->SIMDSize, 30, 31);

   dw[4] =
      util_bitpack_uint(values->ExecutionMask, 0, 31);

   dw[5] =
      util_bitpack_uint(values->LocalXMaximum, 0, 9) |
      util_bitpack_uint(values->LocalYMaximum, 10, 19) |
      util_bitpack_uint(values->LocalZMaximum, 20, 29);

   dw[6] =
      util_bitpack_uint(values->ThreadGroupIDXDimension, 0, 31);

   dw[7] =
      util_bitpack_uint(values->ThreadGroupIDYDimension, 0, 31);

   dw[8] =
      util_bitpack_uint(values->ThreadGroupIDZDimension, 0, 31);

   dw[9] =
      util_bitpack_uint(values->ThreadGroupIDStartingX, 0, 31);

   dw[10] =
      util_bitpack_uint(values->ThreadGroupIDStartingY, 0, 31);

   dw[11] =
      util_bitpack_uint(values->ThreadGroupIDStartingZ, 0, 31);

   dw[12] =
      util_bitpack_uint(values->PartitionID, 0, 31);

   dw[13] =
      util_bitpack_uint(values->PartitionSize, 0, 31);

   dw[14] =
      util_bitpack_uint(values->PreemptX, 0, 31);

   dw[15] =
      util_bitpack_uint(values->PreemptY, 0, 31);

   dw[16] =
      util_bitpack_uint(values->PreemptZ, 0, 31);

   dw[17] =
      util_bitpack_uint(values->WalkerID, 0, 3) |
      util_bitpack_uint(values->OverdispatchTGcount, 8, 23);

   GFX30_INTERFACE_DESCRIPTOR_DATA_pack(data, &dw[18], &values->InterfaceDescriptor);

   GFX30_POSTSYNC_DATA_pack(data, &dw[26], &values->PostSync);

   dw[31] =
      util_bitpack_uint(values->InlineData[0], 0, 31);

   dw[32] =
      util_bitpack_uint(values->InlineData[1], 0, 31);

   dw[33] =
      util_bitpack_uint(values->InlineData[2], 0, 31);

   dw[34] =
      util_bitpack_uint(values->InlineData[3], 0, 31);

   dw[35] =
      util_bitpack_uint(values->InlineData[4], 0, 31);

   dw[36] =
      util_bitpack_uint(values->InlineData[5], 0, 31);

   dw[37] =
      util_bitpack_uint(values->InlineData[6], 0, 31);

   dw[38] =
      util_bitpack_uint(values->InlineData[7], 0, 31);
}

#define GFX30_FILTER_COEFFICIENT_length        1
struct GFX30_FILTER_COEFFICIENT {
   float                                FilterCoefficient;
};

static inline __attribute__((always_inline)) void
GFX30_FILTER_COEFFICIENT_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_FILTER_COEFFICIENT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->FilterCoefficient, 0, 7, 6);
}

#define GFX30_FRAMEDELTAQP_length              2
struct GFX30_FRAMEDELTAQP {
   int32_t                              FrameDeltaQP[8];
};

static inline __attribute__((always_inline)) void
GFX30_FRAMEDELTAQP_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_FRAMEDELTAQP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sint(values->FrameDeltaQP[0], 0, 7) |
      util_bitpack_sint(values->FrameDeltaQP[1], 8, 15) |
      util_bitpack_sint(values->FrameDeltaQP[2], 16, 23) |
      util_bitpack_sint(values->FrameDeltaQP[3], 24, 31);

   dw[1] =
      util_bitpack_sint(values->FrameDeltaQP[4], 0, 7) |
      util_bitpack_sint(values->FrameDeltaQP[5], 8, 15) |
      util_bitpack_sint(values->FrameDeltaQP[6], 16, 23) |
      util_bitpack_sint(values->FrameDeltaQP[7], 24, 31);
}

#define GFX30_FRAMEDELTAQPRANGE_length         2
struct GFX30_FRAMEDELTAQPRANGE {
   uint32_t                             FrameDeltaQPRange[8];
};

static inline __attribute__((always_inline)) void
GFX30_FRAMEDELTAQPRANGE_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_FRAMEDELTAQPRANGE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->FrameDeltaQPRange[0], 0, 7) |
      util_bitpack_uint(values->FrameDeltaQPRange[1], 8, 15) |
      util_bitpack_uint(values->FrameDeltaQPRange[2], 16, 23) |
      util_bitpack_uint(values->FrameDeltaQPRange[3], 24, 31);

   dw[1] =
      util_bitpack_uint(values->FrameDeltaQPRange[4], 0, 7) |
      util_bitpack_uint(values->FrameDeltaQPRange[5], 8, 15) |
      util_bitpack_uint(values->FrameDeltaQPRange[6], 16, 23) |
      util_bitpack_uint(values->FrameDeltaQPRange[7], 24, 31);
}

#define GFX30_GATHER_CONSTANT_ENTRY_length      1
struct GFX30_GATHER_CONSTANT_ENTRY {
   uint32_t                             BindingTableIndexOffset;
   uint32_t                             ChannelMask;
   uint64_t                             ConstantBufferOffset;
};

static inline __attribute__((always_inline)) void
GFX30_GATHER_CONSTANT_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_GATHER_CONSTANT_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->BindingTableIndexOffset, 0, 3) |
      util_bitpack_uint(values->ChannelMask, 4, 7) |
      __gen_offset(values->ConstantBufferOffset, 8, 15);
}

#define GFX30_HEVC_ARBITRATION_PRIORITY_length      1
struct GFX30_HEVC_ARBITRATION_PRIORITY {
   uint32_t                             Priority;
#define Highestpriority                          0
#define Secondhighestpriority                    1
#define Thirdhighestpriority                     2
#define Lowestpriority                           3
};

static inline __attribute__((always_inline)) void
GFX30_HEVC_ARBITRATION_PRIORITY_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_HEVC_ARBITRATION_PRIORITY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Priority, 0, 1);
}

#define GFX30_MEMORYADDRESSATTRIBUTES_length      1
struct GFX30_MEMORYADDRESSATTRIBUTES {
   uint32_t                             MOCS;
   struct GFX30_HEVC_ARBITRATION_PRIORITY ArbitrationPriorityControl;
   bool                                 MemoryCompressionEnable;
   uint32_t                             MemoryCompressionMode;
   uint32_t                             RowStoreScratchBufferCacheSelect;
#define LLC                                      0
#define InternalMediaStorage                     1
   uint32_t                             TiledResourceMode;
#define TRMODE_NONE                              0
#define TRMODE_TILES                             1
#define TRMODE_TILEX                             2
#define TRMODE_TILEF                             3
};

static inline __attribute__((always_inline)) void
GFX30_MEMORYADDRESSATTRIBUTES_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_MEMORYADDRESSATTRIBUTES * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   uint32_t v0_0;
   GFX30_HEVC_ARBITRATION_PRIORITY_pack(data, &v0_0, &values->ArbitrationPriorityControl);

   dw[0] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(v0_0, 7, 8) |
      util_bitpack_uint(values->MemoryCompressionEnable, 9, 9) |
      util_bitpack_uint(values->MemoryCompressionMode, 10, 10) |
      util_bitpack_uint(values->RowStoreScratchBufferCacheSelect, 12, 12) |
      util_bitpack_uint(values->TiledResourceMode, 13, 14);
}

#define GFX30_HCP_PAK_INSERT_OBJECT_INDIRECT_PAYLOAD_length      4
struct GFX30_HCP_PAK_INSERT_OBJECT_INDIRECT_PAYLOAD {
   uint32_t                             IndirectPayloadDataSizeinbits;
   __gen_address_type                   IndirectPayloadBaseAddress;
   struct GFX30_MEMORYADDRESSATTRIBUTES IndirectPayloadBaseAddress2;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_PAK_INSERT_OBJECT_INDIRECT_PAYLOAD_pack(__attribute__((unused)) __gen_user_data *data,
                                                  __attribute__((unused)) void * restrict dst,
                                                  __attribute__((unused)) const struct GFX30_HCP_PAK_INSERT_OBJECT_INDIRECT_PAYLOAD * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->IndirectPayloadDataSizeinbits, 0, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->IndirectPayloadBaseAddress, 0, 0, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;

   GFX30_MEMORYADDRESSATTRIBUTES_pack(data, &dw[3], &values->IndirectPayloadBaseAddress2);
}

#define GFX30_HCP_REF_LIST_ENTRY_length        1
struct GFX30_HCP_REF_LIST_ENTRY {
   uint32_t                             ReferencePicturetbValue;
   uint32_t                             ListEntry;
   uint32_t                             ChromaWeightedPrediction;
#define Default                                  0
#define Explicit                                 1
   uint32_t                             LumaWeightedPrediction;
#define Default                                  0
#define Explicit                                 1
   bool                                 LongTermReference;
   bool                                 FieldPic;
   bool                                 TopField;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_REF_LIST_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_HCP_REF_LIST_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ReferencePicturetbValue, 0, 7) |
      util_bitpack_uint(values->ListEntry, 8, 10) |
      util_bitpack_uint(values->ChromaWeightedPrediction, 11, 11) |
      util_bitpack_uint(values->LumaWeightedPrediction, 12, 12) |
      util_bitpack_uint(values->LongTermReference, 13, 13) |
      util_bitpack_uint(values->FieldPic, 14, 14) |
      util_bitpack_uint(values->TopField, 15, 15);
}

#define GFX30_HCP_TILE_POSITION_IN_CTB_length      1
struct GFX30_HCP_TILE_POSITION_IN_CTB {
   uint32_t                             CtbPos0i;
   uint32_t                             CtbPos1i;
   uint32_t                             CtbPos2i;
   uint32_t                             CtbPos3i;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_TILE_POSITION_IN_CTB_pack(__attribute__((unused)) __gen_user_data *data,
                                    __attribute__((unused)) void * restrict dst,
                                    __attribute__((unused)) const struct GFX30_HCP_TILE_POSITION_IN_CTB * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->CtbPos0i, 0, 7) |
      util_bitpack_uint(values->CtbPos1i, 8, 15) |
      util_bitpack_uint(values->CtbPos2i, 16, 23) |
      util_bitpack_uint(values->CtbPos3i, 24, 31);
}

#define GFX30_HCP_TILE_POSITION_IN_CTB_MSB_length      2
struct GFX30_HCP_TILE_POSITION_IN_CTB_MSB {
   uint32_t                             CtbRowPositionofTileColumn[22];
};

static inline __attribute__((always_inline)) void
GFX30_HCP_TILE_POSITION_IN_CTB_MSB_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_HCP_TILE_POSITION_IN_CTB_MSB * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->CtbRowPositionofTileColumn[0], 0, 1) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[1], 2, 3) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[2], 4, 5) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[3], 6, 7) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[4], 8, 9) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[5], 10, 11) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[6], 12, 13) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[7], 14, 15) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[8], 16, 17) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[9], 18, 19) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[10], 20, 21) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[11], 22, 23) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[12], 24, 25) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[13], 26, 27) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[14], 28, 29) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[15], 30, 31);

   dw[1] =
      util_bitpack_uint(values->CtbRowPositionofTileColumn[16], 0, 1) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[17], 2, 3) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[18], 4, 5) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[19], 6, 7) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[20], 8, 9) |
      util_bitpack_uint(values->CtbRowPositionofTileColumn[21], 10, 11);
}

#define GFX30_HCP_WEIGHTOFFSET_CHROMA_ENTRY_length      1
struct GFX30_HCP_WEIGHTOFFSET_CHROMA_ENTRY {
   uint32_t                             DeltaChromaWeightLX0;
   uint32_t                             ChromaOffsetLX0;
   uint32_t                             DeltaChromaWeightLX1;
   uint32_t                             ChromaOffsetLX1;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_WEIGHTOFFSET_CHROMA_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_HCP_WEIGHTOFFSET_CHROMA_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DeltaChromaWeightLX0, 0, 7) |
      util_bitpack_uint(values->ChromaOffsetLX0, 8, 15) |
      util_bitpack_uint(values->DeltaChromaWeightLX1, 16, 23) |
      util_bitpack_uint(values->ChromaOffsetLX1, 24, 31);
}

#define GFX30_HCP_WEIGHTOFFSET_CHROMA_EXT_ENTRY_length      1
struct GFX30_HCP_WEIGHTOFFSET_CHROMA_EXT_ENTRY {
   uint32_t                             ChromaOffsetLX0MSBytei;
   uint32_t                             ChromaOffsetLX0MSBytei1;
   uint32_t                             ChromaOffsetLX1MSBytei;
   uint32_t                             ChromaOffsetLX1MSBytei1;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_WEIGHTOFFSET_CHROMA_EXT_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_HCP_WEIGHTOFFSET_CHROMA_EXT_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ChromaOffsetLX0MSBytei, 0, 7) |
      util_bitpack_uint(values->ChromaOffsetLX0MSBytei1, 8, 15) |
      util_bitpack_uint(values->ChromaOffsetLX1MSBytei, 16, 23) |
      util_bitpack_uint(values->ChromaOffsetLX1MSBytei1, 24, 31);
}

#define GFX30_HCP_WEIGHTOFFSET_LUMA_ENTRY_length      1
struct GFX30_HCP_WEIGHTOFFSET_LUMA_ENTRY {
   uint32_t                             DeltaLumaWeightLX;
   uint32_t                             LumaOffsetLX;
   uint32_t                             LumaOffsetLXMSByte;
};

static inline __attribute__((always_inline)) void
GFX30_HCP_WEIGHTOFFSET_LUMA_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_HCP_WEIGHTOFFSET_LUMA_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DeltaLumaWeightLX, 0, 7) |
      util_bitpack_uint(values->LumaOffsetLX, 8, 15) |
      util_bitpack_uint(values->LumaOffsetLXMSByte, 24, 31);
}

#define GFX30_HEVC_VP9_RDOQ_LAMBDA_FIELDS_length      1
struct GFX30_HEVC_VP9_RDOQ_LAMBDA_FIELDS {
   uint32_t                             LambdaValue0;
   uint32_t                             LambdaValue1;
};

static inline __attribute__((always_inline)) void
GFX30_HEVC_VP9_RDOQ_LAMBDA_FIELDS_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_HEVC_VP9_RDOQ_LAMBDA_FIELDS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->LambdaValue0, 0, 15) |
      util_bitpack_uint(values->LambdaValue1, 16, 31);
}

#define GFX30_HUC_VIRTUAL_ADDR_REGION_length      3
struct GFX30_HUC_VIRTUAL_ADDR_REGION {
   __gen_address_type                   Address;
   struct GFX30_MEMORYADDRESSATTRIBUTES MemoryAddressAttributes;
};

static inline __attribute__((always_inline)) void
GFX30_HUC_VIRTUAL_ADDR_REGION_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_HUC_VIRTUAL_ADDR_REGION * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0_address =
      __gen_address(data, &dw[0], values->Address, 0, 0, 63);
   dw[0] = v0_address;
   dw[1] = v0_address >> 32;

   GFX30_MEMORYADDRESSATTRIBUTES_pack(data, &dw[2], &values->MemoryAddressAttributes);
}

#define GFX30_IMAGE_STATE_COST_length          2
struct GFX30_IMAGE_STATE_COST {
   uint32_t                             MV0Cost;
   uint32_t                             MV1Cost;
   uint32_t                             MV2Cost;
   uint32_t                             MV3Cost;
   uint32_t                             MV4Cost;
   uint32_t                             MV5Cost;
   uint32_t                             MV6Cost;
   uint32_t                             MV7Cost;
};

static inline __attribute__((always_inline)) void
GFX30_IMAGE_STATE_COST_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_IMAGE_STATE_COST * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->MV0Cost, 0, 7) |
      util_bitpack_uint(values->MV1Cost, 8, 15) |
      util_bitpack_uint(values->MV2Cost, 16, 23) |
      util_bitpack_uint(values->MV3Cost, 24, 31);

   dw[1] =
      util_bitpack_uint(values->MV4Cost, 0, 7) |
      util_bitpack_uint(values->MV5Cost, 8, 15) |
      util_bitpack_uint(values->MV6Cost, 16, 23) |
      util_bitpack_uint(values->MV7Cost, 24, 31);
}

#define GFX30_INLINE_DATA_DESCRIPTION_FOR_MFD_AVC_BSD_OBJECT_length      3
struct GFX30_INLINE_DATA_DESCRIPTION_FOR_MFD_AVC_BSD_OBJECT {
   bool                                 MBErrorConcealmentPSliceWeightPredictionDisable;
   bool                                 MBErrorConcealmentPSliceMotionVectorsOverrideDisable;
   bool                                 MBErrorConcealmentBSpatialWeightPredictionDisable;
   bool                                 MBErrorConcealmentBSpatialMotionVectorsOverrideDisable;
   uint32_t                             MBErrorConcealmentBSpatialPredictionMode;
   bool                                 MBHeaderErrorHandling;
   bool                                 EntropyErrorHandling;
   bool                                 MPRErrorHandling;
   bool                                 BSDPrematureCompleteErrorHandling;
   uint32_t                             ConcealmentPictureID;
   bool                                 MBErrorConcealmentBTemporalWeightPredictionDisable;
   bool                                 MBErrorConcealmentBTemporalMotionVectorsOverrideEnable;
   uint32_t                             MBErrorConcealmentBTemporalPredictionMode;
   bool                                 IntraPredMode4x48x8LumaErrorControl;
   bool                                 InitCurrentMBNumber;
   uint32_t                             ConcealmentMethod;
   uint32_t                             FirstMBBitOffset;
   bool                                 LastSlice;
   bool                                 EmulationPreventionBytePresent;
   bool                                 FixPrevMBSkipped;
   uint32_t                             FirstMBByteOffsetofSliceDataorSliceHeader;
   bool                                 IntraPredictionErrorControl;
   bool                                 Intra8x84x4PredictionErrorConcealmentControl;
   uint32_t                             BSliceTemporalInterConcealmentMode;
   uint32_t                             BSliceSpatialInterConcealmentMode;
   uint32_t                             BSliceInterDirectTypeConcealmentMode;
   uint32_t                             BSliceConcealmentMode;
#define IntraConcealment                         1
#define InterConcealment                         0
   uint32_t                             PSliceInterConcealmentMode;
   uint32_t                             PSliceConcealmentMode;
#define IntraConcealment                         1
#define InterConcealment                         0
   uint32_t                             ConcealmentReferencePictureFieldBit;
   uint32_t                             ISliceConcealmentMode;
#define IntraConcealment                         1
#define InterConcealment                         0
};

static inline __attribute__((always_inline)) void
GFX30_INLINE_DATA_DESCRIPTION_FOR_MFD_AVC_BSD_OBJECT_pack(__attribute__((unused)) __gen_user_data *data,
                                                          __attribute__((unused)) void * restrict dst,
                                                          __attribute__((unused)) const struct GFX30_INLINE_DATA_DESCRIPTION_FOR_MFD_AVC_BSD_OBJECT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->MBErrorConcealmentPSliceWeightPredictionDisable, 0, 0) |
      util_bitpack_uint(values->MBErrorConcealmentPSliceMotionVectorsOverrideDisable, 1, 1) |
      util_bitpack_uint(values->MBErrorConcealmentBSpatialWeightPredictionDisable, 3, 3) |
      util_bitpack_uint(values->MBErrorConcealmentBSpatialMotionVectorsOverrideDisable, 4, 4) |
      util_bitpack_uint(values->MBErrorConcealmentBSpatialPredictionMode, 6, 7) |
      util_bitpack_uint(values->MBHeaderErrorHandling, 8, 8) |
      util_bitpack_uint(values->EntropyErrorHandling, 10, 10) |
      util_bitpack_uint(values->MPRErrorHandling, 12, 12) |
      util_bitpack_uint(values->BSDPrematureCompleteErrorHandling, 14, 14) |
      util_bitpack_uint(values->ConcealmentPictureID, 16, 21) |
      util_bitpack_uint(values->MBErrorConcealmentBTemporalWeightPredictionDisable, 24, 24) |
      util_bitpack_uint(values->MBErrorConcealmentBTemporalMotionVectorsOverrideEnable, 25, 25) |
      util_bitpack_uint(values->MBErrorConcealmentBTemporalPredictionMode, 27, 28) |
      util_bitpack_uint(values->IntraPredMode4x48x8LumaErrorControl, 29, 29) |
      util_bitpack_uint(values->InitCurrentMBNumber, 30, 30) |
      util_bitpack_uint(values->ConcealmentMethod, 31, 31);

   dw[1] =
      util_bitpack_uint(values->FirstMBBitOffset, 0, 2) |
      util_bitpack_uint(values->LastSlice, 3, 3) |
      util_bitpack_uint(values->EmulationPreventionBytePresent, 4, 4) |
      util_bitpack_uint(values->FixPrevMBSkipped, 7, 7) |
      util_bitpack_uint(values->FirstMBByteOffsetofSliceDataorSliceHeader, 16, 31);

   dw[2] =
      util_bitpack_uint(values->IntraPredictionErrorControl, 0, 0) |
      util_bitpack_uint(values->Intra8x84x4PredictionErrorConcealmentControl, 1, 1) |
      util_bitpack_uint(values->BSliceTemporalInterConcealmentMode, 4, 6) |
      util_bitpack_uint(values->BSliceSpatialInterConcealmentMode, 8, 10) |
      util_bitpack_uint(values->BSliceInterDirectTypeConcealmentMode, 12, 13) |
      util_bitpack_uint(values->BSliceConcealmentMode, 15, 15) |
      util_bitpack_uint(values->PSliceInterConcealmentMode, 16, 18) |
      util_bitpack_uint(values->PSliceConcealmentMode, 23, 23) |
      util_bitpack_uint(values->ConcealmentReferencePictureFieldBit, 24, 29) |
      util_bitpack_uint(values->ISliceConcealmentMode, 31, 31);
}

#define GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_length      4
struct GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY {
   float                                Table0XFilterCoefficientn0;
   float                                Table0YFilterCoefficientn0;
   float                                Table0XFilterCoefficientn1;
   float                                Table0YFilterCoefficientn1;
   float                                Table0XFilterCoefficientn2;
   float                                Table0YFilterCoefficientn2;
   float                                Table0XFilterCoefficientn3;
   float                                Table0YFilterCoefficientn3;
   float                                Table0XFilterCoefficientn4;
   float                                Table0YFilterCoefficientn4;
   float                                Table0XFilterCoefficientn5;
   float                                Table0YFilterCoefficientn5;
   float                                Table0XFilterCoefficientn6;
   float                                Table0YFilterCoefficientn6;
   float                                Table0XFilterCoefficientn7;
   float                                Table0YFilterCoefficientn7;
};

static inline __attribute__((always_inline)) void
GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn0, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn0, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn1, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn1, 24, 31, 6);

   dw[1] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn2, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn2, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn3, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn3, 24, 31, 6);

   dw[2] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn4, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn4, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn5, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn5, 24, 31, 6);

   dw[3] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn6, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn6, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn7, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn7, 24, 31, 6);
}

#define GFX30_MFD_MPEG2_BSD_OBJECT_INLINE_DATA_DESCRIPTION_length      2
struct GFX30_MFD_MPEG2_BSD_OBJECT_INLINE_DATA_DESCRIPTION {
   uint32_t                             FirstMBBitOffset;
   bool                                 LastMB;
   bool                                 LastPicSlice;
   uint32_t                             SliceConcealmentType;
   uint32_t                             SliceConcealmentOverride;
   uint32_t                             MBCount;
   uint32_t                             SliceVerticalPosition;
   uint32_t                             SliceHorizontalPosition;
   uint32_t                             NextSliceHorizontalPosition;
   uint32_t                             NextSliceVerticalPosition;
   uint32_t                             QuantizerScaleCode;
};

static inline __attribute__((always_inline)) void
GFX30_MFD_MPEG2_BSD_OBJECT_INLINE_DATA_DESCRIPTION_pack(__attribute__((unused)) __gen_user_data *data,
                                                        __attribute__((unused)) void * restrict dst,
                                                        __attribute__((unused)) const struct GFX30_MFD_MPEG2_BSD_OBJECT_INLINE_DATA_DESCRIPTION * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->FirstMBBitOffset, 0, 2) |
      util_bitpack_uint(values->LastMB, 3, 3) |
      util_bitpack_uint(values->LastPicSlice, 5, 5) |
      util_bitpack_uint(values->SliceConcealmentType, 6, 6) |
      util_bitpack_uint(values->SliceConcealmentOverride, 7, 7) |
      util_bitpack_uint(values->MBCount, 8, 15) |
      util_bitpack_uint(values->SliceVerticalPosition, 16, 23) |
      util_bitpack_uint(values->SliceHorizontalPosition, 24, 31);

   dw[1] =
      util_bitpack_uint(values->NextSliceHorizontalPosition, 0, 7) |
      util_bitpack_uint(values->NextSliceVerticalPosition, 8, 16) |
      util_bitpack_uint(values->QuantizerScaleCode, 24, 28);
}

#define GFX30_MI_MATH_ALU_INSTRUCTION_length      1
struct GFX30_MI_MATH_ALU_INSTRUCTION {
   uint32_t                             Operand2;
#define MI_ALU_REG0                              0
#define MI_ALU_REG1                              1
#define MI_ALU_REG2                              2
#define MI_ALU_REG3                              3
#define MI_ALU_REG4                              4
#define MI_ALU_REG5                              5
#define MI_ALU_REG6                              6
#define MI_ALU_REG7                              7
#define MI_ALU_REG8                              8
#define MI_ALU_REG9                              9
#define MI_ALU_REG10                             10
#define MI_ALU_REG11                             11
#define MI_ALU_REG12                             12
#define MI_ALU_REG13                             13
#define MI_ALU_REG14                             14
#define MI_ALU_REG15                             15
#define MI_ALU_SRCA                              32
#define MI_ALU_SRCB                              33
#define MI_ALU_ACCU                              49
#define MI_ALU_ZF                                50
#define MI_ALU_CF                                51
   uint32_t                             Operand1;
#define MI_ALU_REG0                              0
#define MI_ALU_REG1                              1
#define MI_ALU_REG2                              2
#define MI_ALU_REG3                              3
#define MI_ALU_REG4                              4
#define MI_ALU_REG5                              5
#define MI_ALU_REG6                              6
#define MI_ALU_REG7                              7
#define MI_ALU_REG8                              8
#define MI_ALU_REG9                              9
#define MI_ALU_REG10                             10
#define MI_ALU_REG11                             11
#define MI_ALU_REG12                             12
#define MI_ALU_REG13                             13
#define MI_ALU_REG14                             14
#define MI_ALU_REG15                             15
#define MI_ALU_SRCA                              32
#define MI_ALU_SRCB                              33
#define MI_ALU_ACCU                              49
#define MI_ALU_ZF                                50
#define MI_ALU_CF                                51
   uint32_t                             ALUOpcode;
#define MI_ALU_NOOP                              0
#define MI_ALU_FENCE_RD                          1
#define MI_ALU_FENCE_WR                          2
#define MI_ALU_LOAD                              128
#define MI_ALU_LOADINV                           1152
#define MI_ALU_LOAD0                             129
#define MI_ALU_LOAD1                             1153
#define MI_ALU_LOADIND                           130
#define MI_ALU_ADD                               256
#define MI_ALU_SUB                               257
#define MI_ALU_AND                               258
#define MI_ALU_OR                                259
#define MI_ALU_XOR                               260
#define MI_ALU_SHL                               261
#define MI_ALU_SHR                               262
#define MI_ALU_SAR                               263
#define MI_ALU_STORE                             384
#define MI_ALU_STOREINV                          1408
#define MI_ALU_STOREIND                          385
};

static inline __attribute__((always_inline)) void
GFX30_MI_MATH_ALU_INSTRUCTION_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_MI_MATH_ALU_INSTRUCTION * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Operand2, 0, 9) |
      util_bitpack_uint(values->Operand1, 10, 19) |
      util_bitpack_uint(values->ALUOpcode, 20, 31);
}

#define GFX30_PALETTE_ENTRY_length             1
struct GFX30_PALETTE_ENTRY {
   uint32_t                             Blue;
   uint32_t                             Green;
   uint32_t                             Red;
   uint32_t                             Alpha;
};

static inline __attribute__((always_inline)) void
GFX30_PALETTE_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_PALETTE_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Blue, 0, 7) |
      util_bitpack_uint(values->Green, 8, 15) |
      util_bitpack_uint(values->Red, 16, 23) |
      util_bitpack_uint(values->Alpha, 24, 31);
}

#define GFX30_RENDER_SURFACE_STATE_length     16
struct GFX30_RENDER_SURFACE_STATE {
   bool                                 CubeFaceEnablePositiveZ;
   bool                                 CubeFaceEnableNegativeZ;
   bool                                 CubeFaceEnablePositiveY;
   bool                                 CubeFaceEnableNegativeY;
   bool                                 CubeFaceEnablePositiveX;
   bool                                 CubeFaceEnableNegativeX;
   uint32_t                             MediaBoundaryPixelMode;
#define NORMAL_MODE                              0
#define PROGRESSIVE_FRAME                        2
#define INTERLACED_FRAME                         3
   uint32_t                             RenderCacheReadWriteMode;
#define WriteOnlyCache                           0
#define ReadWriteCache                           1
   bool                                 EnableSamplerRoutetoLSC;
   uint32_t                             VerticalLineStrideOffset;
   uint32_t                             VerticalLineStride;
   uint32_t                             TileMode;
#define LINEAR                                   0
#define TILE64                                   1
#define XMAJOR                                   2
#define TILE4                                    3
   uint32_t                             SurfaceHorizontalAlignment;
#define HALIGN_16                                0
#define HALIGN_32                                1
#define HALIGN_64                                2
#define HALIGN_128                               3
   uint32_t                             SurfaceVerticalAlignment;
#define VALIGN_4                                 1
#define VALIGN_8                                 2
#define VALIGN_16                                3
   uint32_t                             SurfaceFormat;
   bool                                 SurfaceArray;
   enum GFX30_STATE_SURFACE_TYPE        SurfaceType;
   uint32_t                             SurfaceQPitch;
   bool                                 SampleTapDiscardDisable;
   bool                                 CornerTexelMode;
   uint32_t                             BaseMipLevel;
   uint32_t                             MOCS;
   uint32_t                             Width;
   uint32_t                             Height;
   bool                                 DepthStencilResource;
   uint32_t                             SurfacePitch;
   uint32_t                             Depth;
   uint32_t                             MultisamplePositionPaletteIndex;
   uint32_t                             NumberofMultisamples;
#define MULTISAMPLECOUNT_1                       0
#define MULTISAMPLECOUNT_2                       1
#define MULTISAMPLECOUNT_4                       2
#define MULTISAMPLECOUNT_8                       3
#define MULTISAMPLECOUNT_16                      4
   uint32_t                             MultisampledSurfaceStorageFormat;
#define MSFMT_MSS                                0
#define MSFMT_DEPTH_STENCIL                      1
   uint32_t                             RenderTargetViewExtent;
   uint32_t                             MinimumArrayElement;
   uint32_t                             RenderTargetAndSampleUnormRotation;
#define _0DEG                                    0
#define _90DEG                                   1
#define _180DEG                                  2
#define _270DEG                                  3
   uint32_t                             MIPCountLOD;
   uint32_t                             SurfaceMinLOD;
   uint32_t                             MipTailStartLOD;
   enum GFX30_L1_CACHE_CONTROL          L1CacheControl;
   bool                                 EWADisableForCube;
   uint32_t                             YOffset;
   uint32_t                             XOffset;
   uint32_t                             AuxiliarySurfaceMode;
#define AUX_NONE                                 0
#define AUX_APPEND                               1
#define AUX_MCS                                  2
   uint32_t                             YOffsetforUorUVPlane;
   uint32_t                             AuxiliarySurfacePitch;
   bool                                 YUVInterpolationEnable;
   uint32_t                             XOffsetforUorUVPlane;
   uint32_t                             AuxiliarySurfaceQPitch;
   float                                ResourceMinLOD;
   enum GFX30_ShaderChannelSelect       ShaderChannelSelectAlpha;
   enum GFX30_ShaderChannelSelect       ShaderChannelSelectBlue;
   enum GFX30_ShaderChannelSelect       ShaderChannelSelectGreen;
   enum GFX30_ShaderChannelSelect       ShaderChannelSelectRed;
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             MipRegionWidthInLog2;
   __gen_address_type                   AppendCounterAddress;
   uint32_t                             MipRegionHeightInLog2;
   bool                                 ProceduralTexture;
   __gen_address_type                   AuxiliarySurfaceBaseAddress;
   uint32_t                             YOffsetforVPlane;
   uint32_t                             XOffsetforVPlane;
   enum GFX30_UNIFIED_COMPRESSION_FORMAT CompressionFormat;
   uint32_t                             MipRegionDepthInLog2;
   bool                                 Disallowlowqualityfiltering;
};

static inline __attribute__((always_inline)) void
GFX30_RENDER_SURFACE_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_RENDER_SURFACE_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->CubeFaceEnablePositiveZ, 0, 0) |
      util_bitpack_uint(values->CubeFaceEnableNegativeZ, 1, 1) |
      util_bitpack_uint(values->CubeFaceEnablePositiveY, 2, 2) |
      util_bitpack_uint(values->CubeFaceEnableNegativeY, 3, 3) |
      util_bitpack_uint(values->CubeFaceEnablePositiveX, 4, 4) |
      util_bitpack_uint(values->CubeFaceEnableNegativeX, 5, 5) |
      util_bitpack_uint(values->MediaBoundaryPixelMode, 6, 7) |
      util_bitpack_uint(values->RenderCacheReadWriteMode, 8, 8) |
      util_bitpack_uint(values->EnableSamplerRoutetoLSC, 9, 9) |
      util_bitpack_uint(values->VerticalLineStrideOffset, 10, 10) |
      util_bitpack_uint(values->VerticalLineStride, 11, 11) |
      util_bitpack_uint(values->TileMode, 12, 13) |
      util_bitpack_uint(values->SurfaceHorizontalAlignment, 14, 15) |
      util_bitpack_uint(values->SurfaceVerticalAlignment, 16, 17) |
      util_bitpack_uint(values->SurfaceFormat, 18, 26) |
      util_bitpack_uint(values->SurfaceArray, 28, 28) |
      util_bitpack_uint(values->SurfaceType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SurfaceQPitch, 0, 14) |
      util_bitpack_uint(values->SampleTapDiscardDisable, 15, 15) |
      util_bitpack_uint(values->CornerTexelMode, 18, 18) |
      util_bitpack_uint(values->BaseMipLevel, 19, 23) |
      util_bitpack_uint_nonzero(values->MOCS, 24, 30);

   dw[2] =
      util_bitpack_uint(values->Width, 0, 13) |
      util_bitpack_uint(values->Height, 16, 29) |
      util_bitpack_uint(values->DepthStencilResource, 31, 31);

   dw[3] =
      util_bitpack_uint(values->SurfacePitch, 0, 17) |
      util_bitpack_uint(values->Depth, 21, 31);

   dw[4] =
      util_bitpack_uint(values->MultisamplePositionPaletteIndex, 0, 2) |
      util_bitpack_uint(values->NumberofMultisamples, 3, 5) |
      util_bitpack_uint(values->MultisampledSurfaceStorageFormat, 6, 6) |
      util_bitpack_uint(values->RenderTargetViewExtent, 7, 17) |
      util_bitpack_uint(values->MinimumArrayElement, 18, 28) |
      util_bitpack_uint(values->RenderTargetAndSampleUnormRotation, 29, 30);

   dw[5] =
      util_bitpack_uint(values->MIPCountLOD, 0, 3) |
      util_bitpack_uint(values->SurfaceMinLOD, 4, 7) |
      util_bitpack_uint(values->MipTailStartLOD, 8, 11) |
      util_bitpack_uint(values->L1CacheControl, 16, 18) |
      util_bitpack_uint(values->EWADisableForCube, 20, 20) |
      util_bitpack_uint(values->YOffset, 21, 23) |
      util_bitpack_uint(values->XOffset, 25, 31);

   dw[6] =
      util_bitpack_uint(values->AuxiliarySurfaceMode, 0, 2) |
      util_bitpack_uint(values->YOffsetforUorUVPlane, 0, 13) |
      util_bitpack_uint(values->AuxiliarySurfacePitch, 3, 12) |
      util_bitpack_uint(values->YUVInterpolationEnable, 15, 15) |
      util_bitpack_uint(values->XOffsetforUorUVPlane, 16, 29) |
      util_bitpack_uint(values->AuxiliarySurfaceQPitch, 16, 30);

   dw[7] =
      util_bitpack_ufixed(values->ResourceMinLOD, 0, 11, 8) |
      util_bitpack_uint(values->ShaderChannelSelectAlpha, 16, 18) |
      util_bitpack_uint(values->ShaderChannelSelectBlue, 19, 21) |
      util_bitpack_uint(values->ShaderChannelSelectGreen, 22, 24) |
      util_bitpack_uint(values->ShaderChannelSelectRed, 25, 27);

   const uint64_t v8_address =
      __gen_address(data, &dw[8], values->SurfaceBaseAddress, 0, 0, 63);
   dw[8] = v8_address;
   dw[9] = v8_address >> 32;

   const uint64_t v10 =
      util_bitpack_uint(values->MipRegionWidthInLog2, 0, 3) |
      util_bitpack_uint(values->MipRegionHeightInLog2, 4, 7) |
      util_bitpack_uint(values->ProceduralTexture, 11, 11) |
      util_bitpack_uint(values->YOffsetforVPlane, 32, 45) |
      util_bitpack_uint(values->XOffsetforVPlane, 48, 61);
   const uint64_t v10_address =
      __gen_address(data, &dw[10], values->AuxiliarySurfaceBaseAddress, v10, 12, 63);
   dw[10] = v10_address;
   dw[11] = (v10_address >> 32) | (v10 >> 32);

   dw[12] =
      util_bitpack_uint(values->CompressionFormat, 0, 3) |
      util_bitpack_uint(values->MipRegionDepthInLog2, 4, 7);

   dw[13] =
      util_bitpack_uint(values->Disallowlowqualityfiltering, 31, 31);

   dw[14] = 0;

   dw[15] = 0;
}

#define GFX30_RESOURCE_BARRIER_BODY_length      4
struct GFX30_RESOURCE_BARRIER_BODY {
   enum GFX30_RESOURCE_BARRIER_STAGE    WaitStage;
   enum GFX30_RESOURCE_BARRIER_STAGE    SignalStage;
   enum GFX30_RESOURCE_BARRIER_TYPE     BarrierType;
   bool                                 L1DataportCacheInvalidate;
   bool                                 DepthCache;
   bool                                 ColorCache;
   bool                                 L1DataportUAVFlush;
   bool                                 TextureRO;
   bool                                 StateRO;
   bool                                 VFRO;
   bool                                 AMFS;
   bool                                 ConstantCache;
   __gen_address_type                   BarrierIDAddress;
};

static inline __attribute__((always_inline)) void
GFX30_RESOURCE_BARRIER_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_RESOURCE_BARRIER_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->WaitStage, 0, 11) |
      util_bitpack_uint(values->SignalStage, 12, 23) |
      util_bitpack_uint(values->BarrierType, 30, 31);

   dw[1] =
      util_bitpack_uint(values->L1DataportCacheInvalidate, 21, 21) |
      util_bitpack_uint(values->DepthCache, 22, 22) |
      util_bitpack_uint(values->ColorCache, 23, 23) |
      util_bitpack_uint(values->L1DataportUAVFlush, 24, 24) |
      util_bitpack_uint(values->TextureRO, 25, 25) |
      util_bitpack_uint(values->StateRO, 26, 26) |
      util_bitpack_uint(values->VFRO, 27, 27) |
      util_bitpack_uint(values->AMFS, 28, 28) |
      util_bitpack_uint(values->ConstantCache, 29, 29);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->BarrierIDAddress, 0, 3, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;
}

#define GFX30_ROUNDINGPRECISIONTABLE_3_BITS_length      1
struct GFX30_ROUNDINGPRECISIONTABLE_3_BITS {
   uint32_t                             RoundingPrecision;
#define _116                                     0
#define _216                                     1
#define _316                                     2
#define _416                                     3
#define _516                                     4
#define _616                                     5
#define _716                                     6
#define _816                                     7
};

static inline __attribute__((always_inline)) void
GFX30_ROUNDINGPRECISIONTABLE_3_BITS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_ROUNDINGPRECISIONTABLE_3_BITS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RoundingPrecision, 0, 2);
}

#define GFX30_SAMPLER_BORDER_COLOR_STATE_length      4
struct GFX30_SAMPLER_BORDER_COLOR_STATE {
   float                                BorderColorFloatRed;
   uint32_t                             BorderColor32bitRed;
   float                                BorderColorFloatGreen;
   uint32_t                             BorderColor32bitGreen;
   float                                BorderColorFloatBlue;
   uint32_t                             BorderColor32bitBlue;
   float                                BorderColorFloatAlpha;
   uint32_t                             BorderColor32bitAlpha;
};

static inline __attribute__((always_inline)) void
GFX30_SAMPLER_BORDER_COLOR_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_SAMPLER_BORDER_COLOR_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_float(values->BorderColorFloatRed) |
      util_bitpack_uint(values->BorderColor32bitRed, 0, 31);

   dw[1] =
      util_bitpack_float(values->BorderColorFloatGreen) |
      util_bitpack_uint(values->BorderColor32bitGreen, 0, 31);

   dw[2] =
      util_bitpack_float(values->BorderColorFloatBlue) |
      util_bitpack_uint(values->BorderColor32bitBlue, 0, 31);

   dw[3] =
      util_bitpack_float(values->BorderColorFloatAlpha) |
      util_bitpack_uint(values->BorderColor32bitAlpha, 0, 31);
}

#define GFX30_SAMPLER_INDIRECT_STATE_BORDER_COLOR_length      4
struct GFX30_SAMPLER_INDIRECT_STATE_BORDER_COLOR {
   int32_t                              BorderColorRedAsS31;
   uint32_t                             BorderColorRedAsU32;
   float                                BorderColorRedAsFloat;
   uint32_t                             BorderColorRedAsU8;
   uint32_t                             BorderColorGreenAsU8;
   uint32_t                             BorderColorBlueAsU8;
   uint32_t                             BorderColorAlphaAsU8;
   int32_t                              BorderColorGreenAsS31;
   uint32_t                             BorderColorGreenAsU32;
   float                                BorderColorGreenAsFloat;
   int32_t                              BorderColorBlueAsS31;
   uint32_t                             BorderColorBlueAsU32;
   float                                BorderColorBlueAsFloat;
   int32_t                              BorderColorAlphaAsS31;
   uint32_t                             BorderColorAlphaAsU32;
   float                                BorderColorAlphaAsFloat;
};

static inline __attribute__((always_inline)) void
GFX30_SAMPLER_INDIRECT_STATE_BORDER_COLOR_pack(__attribute__((unused)) __gen_user_data *data,
                                               __attribute__((unused)) void * restrict dst,
                                               __attribute__((unused)) const struct GFX30_SAMPLER_INDIRECT_STATE_BORDER_COLOR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sint(values->BorderColorRedAsS31, 0, 31) |
      util_bitpack_uint(values->BorderColorRedAsU32, 0, 31) |
      util_bitpack_float(values->BorderColorRedAsFloat) |
      util_bitpack_uint(values->BorderColorRedAsU8, 0, 7) |
      util_bitpack_uint(values->BorderColorGreenAsU8, 8, 15) |
      util_bitpack_uint(values->BorderColorBlueAsU8, 16, 23) |
      util_bitpack_uint(values->BorderColorAlphaAsU8, 24, 31);

   dw[1] =
      util_bitpack_sint(values->BorderColorGreenAsS31, 0, 31) |
      util_bitpack_uint(values->BorderColorGreenAsU32, 0, 31) |
      util_bitpack_float(values->BorderColorGreenAsFloat);

   dw[2] =
      util_bitpack_sint(values->BorderColorBlueAsS31, 0, 31) |
      util_bitpack_uint(values->BorderColorBlueAsU32, 0, 31) |
      util_bitpack_float(values->BorderColorBlueAsFloat);

   dw[3] =
      util_bitpack_sint(values->BorderColorAlphaAsS31, 0, 31) |
      util_bitpack_uint(values->BorderColorAlphaAsU32, 0, 31) |
      util_bitpack_float(values->BorderColorAlphaAsFloat);
}

#define GFX30_SAMPLER_STATE_length             4
struct GFX30_SAMPLER_STATE {
   uint32_t                             AnisotropicAlgorithm;
#define LEGACY                                   0
#define EWAApproximation                         1
   float                                TextureLODBias;
   uint32_t                             MinModeFilter;
#define MAPFILTER_NEAREST                        0
#define MAPFILTER_LINEAR                         1
#define MAPFILTER_ANISOTROPIC                    2
#define MAPFILTER_ANISOTROPIC_FAST               3
#define MAPFILTER_MONO                           6
   uint32_t                             MagModeFilter;
#define MAPFILTER_NEAREST                        0
#define MAPFILTER_LINEAR                         1
#define MAPFILTER_ANISOTROPIC                    2
#define MAPFILTER_ANISOTROPIC_FAST               3
#define MAPFILTER_MONO                           6
   uint32_t                             MipModeFilter;
#define MIPFILTER_NONE                           0
#define MIPFILTER_NEAREST                        1
#define MIPFILTER_LINEAR                         3
   uint32_t                             CoarseLODQualityMode;
   uint32_t                             LODPreClampMode;
#define CLAMP_MODE_NONE                          0
#define CLAMP_MODE_OGL                           2
   uint32_t                             TextureBorderColorMode;
#define DX10OGL                                  0
#define DX9                                      1
   bool                                 CPSLODCompensationEnable;
   bool                                 SamplerDisable;
   uint32_t                             CubeSurfaceControlMode;
#define PROGRAMMED                               0
#define OVERRIDE                                 1
   uint32_t                             ShadowFunction;
#define PREFILTEROP_ALWAYS                       0
#define PREFILTEROP_NEVER                        1
#define PREFILTEROP_LESS                         2
#define PREFILTEROP_EQUAL                        3
#define PREFILTEROP_LEQUAL                       4
#define PREFILTEROP_GREATER                      5
#define PREFILTEROP_NOTEQUAL                     6
#define PREFILTEROP_GEQUAL                       7
   uint32_t                             ChromaKeyMode;
#define KEYFILTER_KILL_ON_ANY_MATCH              0
#define KEYFILTER_REPLACE_BLACK                  1
   uint32_t                             ChromaKeyIndex;
   bool                                 ChromaKeyEnable;
   float                                MaxLOD;
   float                                MinLOD;
   uint32_t                             LODClampMagnificationMode;
#define MIPNONE                                  0
#define MIPFILTER                                1
   uint32_t                             SRGBDECODE;
#define DECODE_EXT                               0
#define SKIP_DECODE_EXT                          1
   uint32_t                             ReturnFilterWeightforNullTexels;
   uint32_t                             ReturnFilterWeightforBorderTexels;
   bool                                 Forcegather4Behavior;
   uint64_t                             BorderColorPointer;
   enum GFX30_TextureCoordinateMode     TCZAddressControlMode;
   enum GFX30_TextureCoordinateMode     TCYAddressControlMode;
   enum GFX30_TextureCoordinateMode     TCXAddressControlMode;
   bool                                 ReductionTypeEnable;
   bool                                 NonnormalizedCoordinateEnable;
   uint32_t                             TrilinearFilterQuality;
#define FULL                                     0
#define HIGH                                     1
#define MED                                      2
#define LOW                                      3
   bool                                 RAddressMinFilterRoundingEnable;
   bool                                 RAddressMagFilterRoundingEnable;
   bool                                 VAddressMinFilterRoundingEnable;
   bool                                 VAddressMagFilterRoundingEnable;
   bool                                 UAddressMinFilterRoundingEnable;
   bool                                 UAddressMagFilterRoundingEnable;
   uint32_t                             MaximumAnisotropy;
#define RATIO21                                  0
#define RATIO41                                  1
#define RATIO61                                  2
#define RATIO81                                  3
#define RATIO101                                 4
#define RATIO121                                 5
#define RATIO141                                 6
#define RATIO161                                 7
   uint32_t                             ReductionType;
#define STD_FILTER                               0
#define COMPARISON                               1
#define MINIMUM                                  2
#define MAXIMUM                                  3
   bool                                 AllowLowQualityLODCalculation;
   bool                                 LowQualityFilter;
};

static inline __attribute__((always_inline)) void
GFX30_SAMPLER_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_SAMPLER_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AnisotropicAlgorithm, 0, 0) |
      util_bitpack_sfixed(values->TextureLODBias, 1, 13, 8) |
      util_bitpack_uint(values->MinModeFilter, 14, 16) |
      util_bitpack_uint(values->MagModeFilter, 17, 19) |
      util_bitpack_uint(values->MipModeFilter, 20, 21) |
      util_bitpack_uint(values->CoarseLODQualityMode, 22, 26) |
      util_bitpack_uint(values->LODPreClampMode, 27, 28) |
      util_bitpack_uint(values->TextureBorderColorMode, 29, 29) |
      util_bitpack_uint(values->CPSLODCompensationEnable, 30, 30) |
      util_bitpack_uint(values->SamplerDisable, 31, 31);

   dw[1] =
      util_bitpack_uint(values->CubeSurfaceControlMode, 0, 0) |
      util_bitpack_uint(values->ShadowFunction, 1, 3) |
      util_bitpack_uint(values->ChromaKeyMode, 4, 4) |
      util_bitpack_uint(values->ChromaKeyIndex, 5, 6) |
      util_bitpack_uint(values->ChromaKeyEnable, 7, 7) |
      util_bitpack_ufixed(values->MaxLOD, 8, 19, 8) |
      util_bitpack_ufixed(values->MinLOD, 20, 31, 8);

   dw[2] =
      util_bitpack_uint(values->LODClampMagnificationMode, 0, 0) |
      util_bitpack_uint(values->SRGBDECODE, 1, 1) |
      util_bitpack_uint(values->ReturnFilterWeightforNullTexels, 2, 2) |
      util_bitpack_uint(values->ReturnFilterWeightforBorderTexels, 3, 3) |
      util_bitpack_uint(values->Forcegather4Behavior, 5, 5) |
      __gen_offset(values->BorderColorPointer, 6, 31);

   dw[3] =
      util_bitpack_uint(values->TCZAddressControlMode, 0, 2) |
      util_bitpack_uint(values->TCYAddressControlMode, 3, 5) |
      util_bitpack_uint(values->TCXAddressControlMode, 6, 8) |
      util_bitpack_uint(values->ReductionTypeEnable, 9, 9) |
      util_bitpack_uint(values->NonnormalizedCoordinateEnable, 10, 10) |
      util_bitpack_uint(values->TrilinearFilterQuality, 11, 12) |
      util_bitpack_uint(values->RAddressMinFilterRoundingEnable, 13, 13) |
      util_bitpack_uint(values->RAddressMagFilterRoundingEnable, 14, 14) |
      util_bitpack_uint(values->VAddressMinFilterRoundingEnable, 15, 15) |
      util_bitpack_uint(values->VAddressMagFilterRoundingEnable, 16, 16) |
      util_bitpack_uint(values->UAddressMinFilterRoundingEnable, 17, 17) |
      util_bitpack_uint(values->UAddressMagFilterRoundingEnable, 18, 18) |
      util_bitpack_uint(values->MaximumAnisotropy, 19, 21) |
      util_bitpack_uint(values->ReductionType, 22, 23) |
      util_bitpack_uint(values->AllowLowQualityLODCalculation, 24, 24) |
      util_bitpack_uint(values->LowQualityFilter, 26, 26);
}

#define GFX30_SAMPLER_STATE_8X8_AVS_COEFFICIENTS_length      8
struct GFX30_SAMPLER_STATE_8X8_AVS_COEFFICIENTS {
   float                                Table0FilterCoefficient[4];
   float                                Table1FilterCoefficient0[4];
   float                                Table1FilterCoefficient1[4];
};

static inline __attribute__((always_inline)) void
GFX30_SAMPLER_STATE_8X8_AVS_COEFFICIENTS_pack(__attribute__((unused)) __gen_user_data *data,
                                              __attribute__((unused)) void * restrict dst,
                                              __attribute__((unused)) const struct GFX30_SAMPLER_STATE_8X8_AVS_COEFFICIENTS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->Table0FilterCoefficient[0], 0, 7, 6) |
      util_bitpack_sfixed(values->Table0FilterCoefficient[1], 8, 15, 6) |
      util_bitpack_sfixed(values->Table0FilterCoefficient[2], 16, 23, 6) |
      util_bitpack_sfixed(values->Table0FilterCoefficient[3], 24, 31, 6) |
      util_bitpack_sfixed(values->Table1FilterCoefficient0[0], 0, 7, 6) |
      util_bitpack_sfixed(values->Table1FilterCoefficient1[0], 8, 15, 6);

   dw[1] =
      util_bitpack_sfixed(values->Table1FilterCoefficient0[1], 0, 7, 6) |
      util_bitpack_sfixed(values->Table1FilterCoefficient1[1], 8, 15, 6);

   dw[2] =
      util_bitpack_sfixed(values->Table1FilterCoefficient0[2], 0, 7, 6) |
      util_bitpack_sfixed(values->Table1FilterCoefficient1[2], 8, 15, 6);

   dw[3] =
      util_bitpack_sfixed(values->Table1FilterCoefficient0[3], 0, 7, 6) |
      util_bitpack_sfixed(values->Table1FilterCoefficient1[3], 8, 15, 6);

   dw[4] = 0;

   dw[5] = 0;

   dw[6] = 0;

   dw[7] = 0;
}

#define GFX30_SCISSOR_RECT_length              2
struct GFX30_SCISSOR_RECT {
   uint32_t                             ScissorRectangleXMin;
   uint32_t                             ScissorRectangleYMin;
   uint32_t                             ScissorRectangleXMax;
   uint32_t                             ScissorRectangleYMax;
};

static inline __attribute__((always_inline)) void
GFX30_SCISSOR_RECT_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_SCISSOR_RECT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ScissorRectangleXMin, 0, 15) |
      util_bitpack_uint(values->ScissorRectangleYMin, 16, 31);

   dw[1] =
      util_bitpack_uint(values->ScissorRectangleXMax, 0, 15) |
      util_bitpack_uint(values->ScissorRectangleYMax, 16, 31);
}

#define GFX30_SFC_AVS_CHROMA_COEFF_TABLE_BODY_length     64
struct GFX30_SFC_AVS_CHROMA_COEFF_TABLE_BODY {
   float                                Table1XFilterCoefficientn2;
   float                                Table1YFilterCoefficientn2;
   float                                Table1XFilterCoefficientn3;
   float                                Table1YFilterCoefficientn3;
   float                                Table1XFilterCoefficientn4;
   float                                Table1YFilterCoefficientn4;
   float                                Table1XFilterCoefficientn5;
   float                                Table1YFilterCoefficientn5;
   struct GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY FilterCoefficients[31];
};

static inline __attribute__((always_inline)) void
GFX30_SFC_AVS_CHROMA_COEFF_TABLE_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                           __attribute__((unused)) void * restrict dst,
                                           __attribute__((unused)) const struct GFX30_SFC_AVS_CHROMA_COEFF_TABLE_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->Table1XFilterCoefficientn2, 0, 7, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn2, 8, 15, 6) |
      util_bitpack_sfixed(values->Table1XFilterCoefficientn3, 16, 23, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn3, 24, 31, 6);

   dw[1] =
      util_bitpack_sfixed(values->Table1XFilterCoefficientn4, 0, 7, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn4, 8, 15, 6) |
      util_bitpack_sfixed(values->Table1XFilterCoefficientn5, 16, 23, 6) |
      util_bitpack_sfixed(values->Table1YFilterCoefficientn5, 24, 31, 6);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[2], &values->FilterCoefficients[0]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[4], &values->FilterCoefficients[1]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[6], &values->FilterCoefficients[2]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[8], &values->FilterCoefficients[3]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[10], &values->FilterCoefficients[4]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[12], &values->FilterCoefficients[5]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[14], &values->FilterCoefficients[6]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[16], &values->FilterCoefficients[7]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[18], &values->FilterCoefficients[8]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[20], &values->FilterCoefficients[9]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[22], &values->FilterCoefficients[10]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[24], &values->FilterCoefficients[11]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[26], &values->FilterCoefficients[12]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[28], &values->FilterCoefficients[13]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[30], &values->FilterCoefficients[14]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[32], &values->FilterCoefficients[15]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[34], &values->FilterCoefficients[16]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[36], &values->FilterCoefficients[17]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[38], &values->FilterCoefficients[18]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[40], &values->FilterCoefficients[19]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[42], &values->FilterCoefficients[20]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[44], &values->FilterCoefficients[21]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[46], &values->FilterCoefficients[22]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[48], &values->FilterCoefficients[23]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[50], &values->FilterCoefficients[24]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[52], &values->FilterCoefficients[25]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[54], &values->FilterCoefficients[26]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[56], &values->FilterCoefficients[27]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[58], &values->FilterCoefficients[28]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[60], &values->FilterCoefficients[29]);

   GFX30_CHROMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[62], &values->FilterCoefficients[30]);
}

#define GFX30_SFC_AVS_LUMA_COEFF_TABLE_BODY_length    128
struct GFX30_SFC_AVS_LUMA_COEFF_TABLE_BODY {
   float                                Table0XFilterCoefficientn0;
   float                                Table0YFilterCoefficientn0;
   float                                Table0XFilterCoefficientn1;
   float                                Table0YFilterCoefficientn1;
   float                                Table0XFilterCoefficientn2;
   float                                Table0YFilterCoefficientn2;
   float                                Table0XFilterCoefficientn3;
   float                                Table0YFilterCoefficientn3;
   float                                Table0XFilterCoefficientn4;
   float                                Table0YFilterCoefficientn4;
   float                                Table0XFilterCoefficientn5;
   float                                Table0YFilterCoefficientn5;
   float                                Table0XFilterCoefficientn6;
   float                                Table0YFilterCoefficientn6;
   float                                Table0XFilterCoefficientn7;
   float                                Table0YFilterCoefficientn7;
   struct GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY FilterCoefficients[31];
};

static inline __attribute__((always_inline)) void
GFX30_SFC_AVS_LUMA_COEFF_TABLE_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_SFC_AVS_LUMA_COEFF_TABLE_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn0, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn0, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn1, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn1, 24, 31, 6);

   dw[1] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn2, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn2, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn3, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn3, 24, 31, 6);

   dw[2] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn4, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn4, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn5, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn5, 24, 31, 6);

   dw[3] =
      util_bitpack_sfixed(values->Table0XFilterCoefficientn6, 0, 7, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn6, 8, 15, 6) |
      util_bitpack_sfixed(values->Table0XFilterCoefficientn7, 16, 23, 6) |
      util_bitpack_sfixed(values->Table0YFilterCoefficientn7, 24, 31, 6);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[4], &values->FilterCoefficients[0]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[8], &values->FilterCoefficients[1]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[12], &values->FilterCoefficients[2]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[16], &values->FilterCoefficients[3]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[20], &values->FilterCoefficients[4]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[24], &values->FilterCoefficients[5]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[28], &values->FilterCoefficients[6]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[32], &values->FilterCoefficients[7]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[36], &values->FilterCoefficients[8]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[40], &values->FilterCoefficients[9]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[44], &values->FilterCoefficients[10]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[48], &values->FilterCoefficients[11]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[52], &values->FilterCoefficients[12]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[56], &values->FilterCoefficients[13]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[60], &values->FilterCoefficients[14]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[64], &values->FilterCoefficients[15]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[68], &values->FilterCoefficients[16]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[72], &values->FilterCoefficients[17]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[76], &values->FilterCoefficients[18]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[80], &values->FilterCoefficients[19]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[84], &values->FilterCoefficients[20]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[88], &values->FilterCoefficients[21]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[92], &values->FilterCoefficients[22]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[96], &values->FilterCoefficients[23]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[100], &values->FilterCoefficients[24]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[104], &values->FilterCoefficients[25]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[108], &values->FilterCoefficients[26]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[112], &values->FilterCoefficients[27]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[116], &values->FilterCoefficients[28]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[120], &values->FilterCoefficients[29]);

   GFX30_LUMA_FILTER_COEFFICIENTS_ARRAY_pack(data, &dw[124], &values->FilterCoefficients[30]);
}

#define GFX30_SFC_AVS_STATE_BODY_length        3
struct GFX30_SFC_AVS_STATE_BODY {
   uint32_t                             TransitionAreawith8Pixels;
   uint32_t                             TransitionAreawith4Pixels;
   uint32_t                             SharpnessLevel;
   uint32_t                             MaxDerivative8Pixels;
   uint32_t                             MaxDerivative4Pixels;
   uint32_t                             InputVerticalSiting;
#define _18                                      1
#define _28                                      2
#define _38                                      3
#define _48                                      4
#define _58                                      5
#define _68                                      6
#define _78                                      7
#define _88                                      8
};

static inline __attribute__((always_inline)) void
GFX30_SFC_AVS_STATE_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_SFC_AVS_STATE_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TransitionAreawith8Pixels, 0, 2) |
      util_bitpack_uint(values->TransitionAreawith4Pixels, 4, 6) |
      util_bitpack_uint(values->SharpnessLevel, 24, 31);

   dw[1] =
      util_bitpack_uint(values->MaxDerivative8Pixels, 0, 7) |
      util_bitpack_uint(values->MaxDerivative4Pixels, 16, 23);

   dw[2] =
      util_bitpack_uint(values->InputVerticalSiting, 0, 3);
}

#define GFX30_SFC_IEF_STATE_BODY_length       23
struct GFX30_SFC_IEF_STATE_BODY {
   uint32_t                             GainFactor;
   uint32_t                             WeakEdgeThreshold;
   uint32_t                             StrongEdgeThreshold;
   float                                R3xCoefficient;
   float                                R3cCoefficient;
   uint32_t                             GlobalNoiseEstimation;
   uint32_t                             NonEdgeWeight;
   uint32_t                             RegularWeight;
   uint32_t                             StrongEdgeWeight;
   float                                R5xCoefficient;
   float                                R5cxCoefficient;
   float                                R5cCoefficient;
   float                                STDSinalpha;
   float                                STDCosalpha;
   uint32_t                             Sat_Max;
   uint32_t                             Hue_Max;
   float                                S3U;
   uint32_t                             DiamondMargin;
   bool                                 VY_STD_Enable;
   uint32_t                             U_Mid;
   uint32_t                             V_Mid;
   int32_t                              Diamond_dv;
   uint32_t                             Diamond_Th;
   float                                Diamond_alpha;
   uint32_t                             HS_margin;
   int32_t                              Diamond_du;
   uint32_t                             SkinDetailFactor;
#define DetailRevealed                           0
#define NotDetailRevealed                        1
   uint32_t                             Y_point_1;
   uint32_t                             Y_point_2;
   uint32_t                             Y_point_3;
   uint32_t                             Y_point_4;
   float                                INV_Margin_VYL;
   float                                INV_Margin_VYU;
   uint32_t                             P0L;
   uint32_t                             P1L;
   uint32_t                             P2L;
   uint32_t                             P3L;
   uint32_t                             B0L;
   uint32_t                             B1L;
   uint32_t                             B2L;
   uint32_t                             B3L;
   float                                S0L;
   float                                Y_Slope_2;
   float                                S1L;
   float                                S2L;
   float                                S3L;
   uint32_t                             P0U;
   uint32_t                             P1U;
   float                                Y_Slope1;
   uint32_t                             P2U;
   uint32_t                             P3U;
   uint32_t                             B0U;
   uint32_t                             B1U;
   uint32_t                             B2U;
   uint32_t                             B3U;
   float                                S0U;
   float                                S1U;
   float                                S2U;
   bool                                 TransformEnable;
   bool                                 YUVChannelSwap;
   float                                C0;
   float                                C1;
   float                                C2;
   float                                C3;
   float                                C4;
   float                                C5;
   float                                C6;
   float                                C7;
   float                                C8;
   float                                OffsetIn1;
   float                                OffsetOut1;
   float                                OffsetIn2;
   float                                OffsetOut2;
   float                                OffsetIn3;
   float                                OffsetOut3;
};

static inline __attribute__((always_inline)) void
GFX30_SFC_IEF_STATE_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_SFC_IEF_STATE_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->GainFactor, 0, 5) |
      util_bitpack_uint(values->WeakEdgeThreshold, 6, 11) |
      util_bitpack_uint(values->StrongEdgeThreshold, 12, 17) |
      util_bitpack_ufixed(values->R3xCoefficient, 18, 22, 5) |
      util_bitpack_ufixed(values->R3cCoefficient, 23, 27, 5);

   dw[1] =
      util_bitpack_uint(values->GlobalNoiseEstimation, 0, 7) |
      util_bitpack_uint(values->NonEdgeWeight, 8, 10) |
      util_bitpack_uint(values->RegularWeight, 11, 13) |
      util_bitpack_uint(values->StrongEdgeWeight, 14, 16) |
      util_bitpack_ufixed(values->R5xCoefficient, 17, 21, 5) |
      util_bitpack_ufixed(values->R5cxCoefficient, 22, 26, 5) |
      util_bitpack_ufixed(values->R5cCoefficient, 27, 31, 5);

   dw[2] =
      util_bitpack_sfixed(values->STDSinalpha, 0, 7, 7) |
      util_bitpack_sfixed(values->STDCosalpha, 8, 15, 7) |
      util_bitpack_uint(values->Sat_Max, 16, 21) |
      util_bitpack_uint(values->Hue_Max, 22, 27);

   dw[3] =
      util_bitpack_sfixed(values->S3U, 0, 10, 8) |
      util_bitpack_uint(values->DiamondMargin, 12, 14) |
      util_bitpack_uint(values->VY_STD_Enable, 15, 15) |
      util_bitpack_uint(values->U_Mid, 16, 23) |
      util_bitpack_uint(values->V_Mid, 24, 31);

   dw[4] =
      util_bitpack_sint(values->Diamond_dv, 0, 6) |
      util_bitpack_uint(values->Diamond_Th, 7, 12) |
      util_bitpack_ufixed(values->Diamond_alpha, 13, 20, 6) |
      util_bitpack_uint(values->HS_margin, 21, 23) |
      util_bitpack_sint(values->Diamond_du, 24, 30) |
      util_bitpack_uint(values->SkinDetailFactor, 31, 31);

   dw[5] =
      util_bitpack_uint(values->Y_point_1, 0, 7) |
      util_bitpack_uint(values->Y_point_2, 8, 15) |
      util_bitpack_uint(values->Y_point_3, 16, 23) |
      util_bitpack_uint(values->Y_point_4, 24, 31);

   dw[6] =
      util_bitpack_ufixed(values->INV_Margin_VYL, 0, 15, 16);

   dw[7] =
      util_bitpack_ufixed(values->INV_Margin_VYU, 0, 15, 16) |
      util_bitpack_uint(values->P0L, 16, 23) |
      util_bitpack_uint(values->P1L, 24, 31);

   dw[8] =
      util_bitpack_uint(values->P2L, 0, 7) |
      util_bitpack_uint(values->P3L, 8, 15) |
      util_bitpack_uint(values->B0L, 16, 23) |
      util_bitpack_uint(values->B1L, 24, 31);

   dw[9] =
      util_bitpack_uint(values->B2L, 0, 7) |
      util_bitpack_uint(values->B3L, 8, 15) |
      util_bitpack_sfixed(values->S0L, 16, 26, 8) |
      util_bitpack_ufixed(values->Y_Slope_2, 27, 31, 3);

   dw[10] =
      util_bitpack_sfixed(values->S1L, 0, 10, 8) |
      util_bitpack_sfixed(values->S2L, 11, 21, 8);

   dw[11] =
      util_bitpack_sfixed(values->S3L, 0, 10, 8) |
      util_bitpack_uint(values->P0U, 11, 18) |
      util_bitpack_uint(values->P1U, 19, 26) |
      util_bitpack_ufixed(values->Y_Slope1, 27, 31, 3);

   dw[12] =
      util_bitpack_uint(values->P2U, 0, 7) |
      util_bitpack_uint(values->P3U, 8, 15) |
      util_bitpack_uint(values->B0U, 16, 23) |
      util_bitpack_uint(values->B1U, 24, 31);

   dw[13] =
      util_bitpack_uint(values->B2U, 0, 7) |
      util_bitpack_uint(values->B3U, 8, 15) |
      util_bitpack_sfixed(values->S0U, 16, 26, 8);

   dw[14] =
      util_bitpack_sfixed(values->S1U, 0, 10, 8) |
      util_bitpack_sfixed(values->S2U, 11, 21, 8);

   dw[15] =
      util_bitpack_uint(values->TransformEnable, 0, 0) |
      util_bitpack_uint(values->YUVChannelSwap, 1, 1) |
      util_bitpack_sfixed(values->C0, 3, 15, 10) |
      util_bitpack_sfixed(values->C1, 16, 28, 10);

   dw[16] =
      util_bitpack_sfixed(values->C2, 0, 12, 10) |
      util_bitpack_sfixed(values->C3, 13, 25, 10);

   dw[17] =
      util_bitpack_sfixed(values->C4, 0, 12, 10) |
      util_bitpack_sfixed(values->C5, 13, 25, 10);

   dw[18] =
      util_bitpack_sfixed(values->C6, 0, 12, 10) |
      util_bitpack_sfixed(values->C7, 13, 25, 10);

   dw[19] =
      util_bitpack_sfixed(values->C8, 0, 12, 10);

   dw[20] =
      util_bitpack_sfixed(values->OffsetIn1, 0, 10, 8) |
      util_bitpack_sfixed(values->OffsetOut1, 11, 21, 8);

   dw[21] =
      util_bitpack_sfixed(values->OffsetIn2, 0, 10, 8) |
      util_bitpack_sfixed(values->OffsetOut2, 11, 21, 8);

   dw[22] =
      util_bitpack_sfixed(values->OffsetIn3, 0, 10, 8) |
      util_bitpack_sfixed(values->OffsetOut3, 11, 21, 8);
}

#define GFX30_SFC_LOCK_BODY_length             1
struct GFX30_SFC_LOCK_BODY {
   uint32_t                             VESFCPipeSelect;
   bool                                 PreScaledOutputSurfaceOutputEnable;
};

static inline __attribute__((always_inline)) void
GFX30_SFC_LOCK_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_SFC_LOCK_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->VESFCPipeSelect, 0, 0) |
      util_bitpack_uint(values->PreScaledOutputSurfaceOutputEnable, 1, 1);
}

#define GFX30_SF_CLIP_VIEWPORT_length         16
struct GFX30_SF_CLIP_VIEWPORT {
   float                                ViewportMatrixElementm00;
   float                                ViewportMatrixElementm11;
   float                                ViewportMatrixElementm22;
   float                                ViewportMatrixElementm30;
   float                                ViewportMatrixElementm31;
   float                                ViewportMatrixElementm32;
   float                                XMinClipGuardband;
   float                                XMaxClipGuardband;
   float                                YMinClipGuardband;
   float                                YMaxClipGuardband;
   float                                XMinViewPort;
   float                                XMaxViewPort;
   float                                YMinViewPort;
   float                                YMaxViewPort;
};

static inline __attribute__((always_inline)) void
GFX30_SF_CLIP_VIEWPORT_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SF_CLIP_VIEWPORT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_float(values->ViewportMatrixElementm00);

   dw[1] =
      util_bitpack_float(values->ViewportMatrixElementm11);

   dw[2] =
      util_bitpack_float(values->ViewportMatrixElementm22);

   dw[3] =
      util_bitpack_float(values->ViewportMatrixElementm30);

   dw[4] =
      util_bitpack_float(values->ViewportMatrixElementm31);

   dw[5] =
      util_bitpack_float(values->ViewportMatrixElementm32);

   dw[6] = 0;

   dw[7] = 0;

   dw[8] =
      util_bitpack_float(values->XMinClipGuardband);

   dw[9] =
      util_bitpack_float(values->XMaxClipGuardband);

   dw[10] =
      util_bitpack_float(values->YMinClipGuardband);

   dw[11] =
      util_bitpack_float(values->YMaxClipGuardband);

   dw[12] =
      util_bitpack_float(values->XMinViewPort);

   dw[13] =
      util_bitpack_float(values->XMaxViewPort);

   dw[14] =
      util_bitpack_float(values->YMinViewPort);

   dw[15] =
      util_bitpack_float(values->YMaxViewPort);
}

#define GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_length      1
struct GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL {
   uint32_t                             SourceAttribute;
   uint32_t                             SwizzleSelect;
#define INPUTATTR                                0
#define INPUTATTR_FACING                         1
#define INPUTATTR_W                              2
#define INPUTATTR_FACING_W                       3
   uint32_t                             ConstantSource;
#define CONST_0000                               0
#define CONST_0001_FLOAT                         1
#define CONST_1111_FLOAT                         2
#define PRIM_ID                                  3
   uint32_t                             SwizzleControlMode;
   bool                                 ComponentOverrideX;
   bool                                 ComponentOverrideY;
   bool                                 ComponentOverrideZ;
   bool                                 ComponentOverrideW;
};

static inline __attribute__((always_inline)) void
GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->SourceAttribute, 0, 4) |
      util_bitpack_uint(values->SwizzleSelect, 6, 7) |
      util_bitpack_uint(values->ConstantSource, 9, 10) |
      util_bitpack_uint(values->SwizzleControlMode, 11, 11) |
      util_bitpack_uint(values->ComponentOverrideX, 12, 12) |
      util_bitpack_uint(values->ComponentOverrideY, 13, 13) |
      util_bitpack_uint(values->ComponentOverrideZ, 14, 14) |
      util_bitpack_uint(values->ComponentOverrideW, 15, 15);
}

#define GFX30_SLICE_HASH_TABLE_length        224
struct GFX30_SLICE_HASH_TABLE {
   uint32_t                             Entry[7][16][16];
};

static inline __attribute__((always_inline)) void
GFX30_SLICE_HASH_TABLE_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SLICE_HASH_TABLE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Entry[0][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][0][7], 28, 31);

   dw[1] =
      util_bitpack_uint(values->Entry[0][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][0][15], 28, 31);

   dw[2] =
      util_bitpack_uint(values->Entry[0][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][1][7], 28, 31);

   dw[3] =
      util_bitpack_uint(values->Entry[0][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][1][15], 28, 31);

   dw[4] =
      util_bitpack_uint(values->Entry[0][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][2][7], 28, 31);

   dw[5] =
      util_bitpack_uint(values->Entry[0][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][2][15], 28, 31);

   dw[6] =
      util_bitpack_uint(values->Entry[0][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][3][7], 28, 31);

   dw[7] =
      util_bitpack_uint(values->Entry[0][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][3][15], 28, 31);

   dw[8] =
      util_bitpack_uint(values->Entry[0][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][4][7], 28, 31);

   dw[9] =
      util_bitpack_uint(values->Entry[0][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][4][15], 28, 31);

   dw[10] =
      util_bitpack_uint(values->Entry[0][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][5][7], 28, 31);

   dw[11] =
      util_bitpack_uint(values->Entry[0][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][5][15], 28, 31);

   dw[12] =
      util_bitpack_uint(values->Entry[0][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][6][7], 28, 31);

   dw[13] =
      util_bitpack_uint(values->Entry[0][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][6][15], 28, 31);

   dw[14] =
      util_bitpack_uint(values->Entry[0][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][7][7], 28, 31);

   dw[15] =
      util_bitpack_uint(values->Entry[0][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][7][15], 28, 31);

   dw[16] =
      util_bitpack_uint(values->Entry[0][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][8][7], 28, 31);

   dw[17] =
      util_bitpack_uint(values->Entry[0][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][8][15], 28, 31);

   dw[18] =
      util_bitpack_uint(values->Entry[0][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][9][7], 28, 31);

   dw[19] =
      util_bitpack_uint(values->Entry[0][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][9][15], 28, 31);

   dw[20] =
      util_bitpack_uint(values->Entry[0][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][10][7], 28, 31);

   dw[21] =
      util_bitpack_uint(values->Entry[0][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][10][15], 28, 31);

   dw[22] =
      util_bitpack_uint(values->Entry[0][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][11][7], 28, 31);

   dw[23] =
      util_bitpack_uint(values->Entry[0][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][11][15], 28, 31);

   dw[24] =
      util_bitpack_uint(values->Entry[0][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][12][7], 28, 31);

   dw[25] =
      util_bitpack_uint(values->Entry[0][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][12][15], 28, 31);

   dw[26] =
      util_bitpack_uint(values->Entry[0][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][13][7], 28, 31);

   dw[27] =
      util_bitpack_uint(values->Entry[0][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][13][15], 28, 31);

   dw[28] =
      util_bitpack_uint(values->Entry[0][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][14][7], 28, 31);

   dw[29] =
      util_bitpack_uint(values->Entry[0][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][14][15], 28, 31);

   dw[30] =
      util_bitpack_uint(values->Entry[0][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[0][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[0][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[0][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[0][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[0][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[0][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[0][15][7], 28, 31);

   dw[31] =
      util_bitpack_uint(values->Entry[0][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[0][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[0][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[0][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[0][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[0][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[0][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[0][15][15], 28, 31);

   dw[32] =
      util_bitpack_uint(values->Entry[1][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][0][7], 28, 31);

   dw[33] =
      util_bitpack_uint(values->Entry[1][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][0][15], 28, 31);

   dw[34] =
      util_bitpack_uint(values->Entry[1][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][1][7], 28, 31);

   dw[35] =
      util_bitpack_uint(values->Entry[1][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][1][15], 28, 31);

   dw[36] =
      util_bitpack_uint(values->Entry[1][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][2][7], 28, 31);

   dw[37] =
      util_bitpack_uint(values->Entry[1][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][2][15], 28, 31);

   dw[38] =
      util_bitpack_uint(values->Entry[1][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][3][7], 28, 31);

   dw[39] =
      util_bitpack_uint(values->Entry[1][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][3][15], 28, 31);

   dw[40] =
      util_bitpack_uint(values->Entry[1][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][4][7], 28, 31);

   dw[41] =
      util_bitpack_uint(values->Entry[1][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][4][15], 28, 31);

   dw[42] =
      util_bitpack_uint(values->Entry[1][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][5][7], 28, 31);

   dw[43] =
      util_bitpack_uint(values->Entry[1][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][5][15], 28, 31);

   dw[44] =
      util_bitpack_uint(values->Entry[1][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][6][7], 28, 31);

   dw[45] =
      util_bitpack_uint(values->Entry[1][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][6][15], 28, 31);

   dw[46] =
      util_bitpack_uint(values->Entry[1][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][7][7], 28, 31);

   dw[47] =
      util_bitpack_uint(values->Entry[1][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][7][15], 28, 31);

   dw[48] =
      util_bitpack_uint(values->Entry[1][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][8][7], 28, 31);

   dw[49] =
      util_bitpack_uint(values->Entry[1][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][8][15], 28, 31);

   dw[50] =
      util_bitpack_uint(values->Entry[1][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][9][7], 28, 31);

   dw[51] =
      util_bitpack_uint(values->Entry[1][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][9][15], 28, 31);

   dw[52] =
      util_bitpack_uint(values->Entry[1][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][10][7], 28, 31);

   dw[53] =
      util_bitpack_uint(values->Entry[1][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][10][15], 28, 31);

   dw[54] =
      util_bitpack_uint(values->Entry[1][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][11][7], 28, 31);

   dw[55] =
      util_bitpack_uint(values->Entry[1][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][11][15], 28, 31);

   dw[56] =
      util_bitpack_uint(values->Entry[1][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][12][7], 28, 31);

   dw[57] =
      util_bitpack_uint(values->Entry[1][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][12][15], 28, 31);

   dw[58] =
      util_bitpack_uint(values->Entry[1][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][13][7], 28, 31);

   dw[59] =
      util_bitpack_uint(values->Entry[1][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][13][15], 28, 31);

   dw[60] =
      util_bitpack_uint(values->Entry[1][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][14][7], 28, 31);

   dw[61] =
      util_bitpack_uint(values->Entry[1][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][14][15], 28, 31);

   dw[62] =
      util_bitpack_uint(values->Entry[1][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[1][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[1][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[1][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[1][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[1][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[1][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[1][15][7], 28, 31);

   dw[63] =
      util_bitpack_uint(values->Entry[1][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[1][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[1][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[1][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[1][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[1][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[1][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[1][15][15], 28, 31);

   dw[64] =
      util_bitpack_uint(values->Entry[2][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][0][7], 28, 31);

   dw[65] =
      util_bitpack_uint(values->Entry[2][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][0][15], 28, 31);

   dw[66] =
      util_bitpack_uint(values->Entry[2][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][1][7], 28, 31);

   dw[67] =
      util_bitpack_uint(values->Entry[2][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][1][15], 28, 31);

   dw[68] =
      util_bitpack_uint(values->Entry[2][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][2][7], 28, 31);

   dw[69] =
      util_bitpack_uint(values->Entry[2][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][2][15], 28, 31);

   dw[70] =
      util_bitpack_uint(values->Entry[2][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][3][7], 28, 31);

   dw[71] =
      util_bitpack_uint(values->Entry[2][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][3][15], 28, 31);

   dw[72] =
      util_bitpack_uint(values->Entry[2][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][4][7], 28, 31);

   dw[73] =
      util_bitpack_uint(values->Entry[2][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][4][15], 28, 31);

   dw[74] =
      util_bitpack_uint(values->Entry[2][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][5][7], 28, 31);

   dw[75] =
      util_bitpack_uint(values->Entry[2][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][5][15], 28, 31);

   dw[76] =
      util_bitpack_uint(values->Entry[2][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][6][7], 28, 31);

   dw[77] =
      util_bitpack_uint(values->Entry[2][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][6][15], 28, 31);

   dw[78] =
      util_bitpack_uint(values->Entry[2][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][7][7], 28, 31);

   dw[79] =
      util_bitpack_uint(values->Entry[2][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][7][15], 28, 31);

   dw[80] =
      util_bitpack_uint(values->Entry[2][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][8][7], 28, 31);

   dw[81] =
      util_bitpack_uint(values->Entry[2][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][8][15], 28, 31);

   dw[82] =
      util_bitpack_uint(values->Entry[2][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][9][7], 28, 31);

   dw[83] =
      util_bitpack_uint(values->Entry[2][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][9][15], 28, 31);

   dw[84] =
      util_bitpack_uint(values->Entry[2][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][10][7], 28, 31);

   dw[85] =
      util_bitpack_uint(values->Entry[2][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][10][15], 28, 31);

   dw[86] =
      util_bitpack_uint(values->Entry[2][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][11][7], 28, 31);

   dw[87] =
      util_bitpack_uint(values->Entry[2][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][11][15], 28, 31);

   dw[88] =
      util_bitpack_uint(values->Entry[2][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][12][7], 28, 31);

   dw[89] =
      util_bitpack_uint(values->Entry[2][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][12][15], 28, 31);

   dw[90] =
      util_bitpack_uint(values->Entry[2][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][13][7], 28, 31);

   dw[91] =
      util_bitpack_uint(values->Entry[2][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][13][15], 28, 31);

   dw[92] =
      util_bitpack_uint(values->Entry[2][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][14][7], 28, 31);

   dw[93] =
      util_bitpack_uint(values->Entry[2][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][14][15], 28, 31);

   dw[94] =
      util_bitpack_uint(values->Entry[2][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[2][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[2][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[2][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[2][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[2][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[2][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[2][15][7], 28, 31);

   dw[95] =
      util_bitpack_uint(values->Entry[2][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[2][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[2][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[2][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[2][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[2][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[2][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[2][15][15], 28, 31);

   dw[96] =
      util_bitpack_uint(values->Entry[3][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][0][7], 28, 31);

   dw[97] =
      util_bitpack_uint(values->Entry[3][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][0][15], 28, 31);

   dw[98] =
      util_bitpack_uint(values->Entry[3][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][1][7], 28, 31);

   dw[99] =
      util_bitpack_uint(values->Entry[3][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][1][15], 28, 31);

   dw[100] =
      util_bitpack_uint(values->Entry[3][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][2][7], 28, 31);

   dw[101] =
      util_bitpack_uint(values->Entry[3][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][2][15], 28, 31);

   dw[102] =
      util_bitpack_uint(values->Entry[3][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][3][7], 28, 31);

   dw[103] =
      util_bitpack_uint(values->Entry[3][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][3][15], 28, 31);

   dw[104] =
      util_bitpack_uint(values->Entry[3][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][4][7], 28, 31);

   dw[105] =
      util_bitpack_uint(values->Entry[3][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][4][15], 28, 31);

   dw[106] =
      util_bitpack_uint(values->Entry[3][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][5][7], 28, 31);

   dw[107] =
      util_bitpack_uint(values->Entry[3][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][5][15], 28, 31);

   dw[108] =
      util_bitpack_uint(values->Entry[3][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][6][7], 28, 31);

   dw[109] =
      util_bitpack_uint(values->Entry[3][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][6][15], 28, 31);

   dw[110] =
      util_bitpack_uint(values->Entry[3][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][7][7], 28, 31);

   dw[111] =
      util_bitpack_uint(values->Entry[3][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][7][15], 28, 31);

   dw[112] =
      util_bitpack_uint(values->Entry[3][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][8][7], 28, 31);

   dw[113] =
      util_bitpack_uint(values->Entry[3][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][8][15], 28, 31);

   dw[114] =
      util_bitpack_uint(values->Entry[3][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][9][7], 28, 31);

   dw[115] =
      util_bitpack_uint(values->Entry[3][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][9][15], 28, 31);

   dw[116] =
      util_bitpack_uint(values->Entry[3][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][10][7], 28, 31);

   dw[117] =
      util_bitpack_uint(values->Entry[3][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][10][15], 28, 31);

   dw[118] =
      util_bitpack_uint(values->Entry[3][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][11][7], 28, 31);

   dw[119] =
      util_bitpack_uint(values->Entry[3][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][11][15], 28, 31);

   dw[120] =
      util_bitpack_uint(values->Entry[3][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][12][7], 28, 31);

   dw[121] =
      util_bitpack_uint(values->Entry[3][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][12][15], 28, 31);

   dw[122] =
      util_bitpack_uint(values->Entry[3][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][13][7], 28, 31);

   dw[123] =
      util_bitpack_uint(values->Entry[3][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][13][15], 28, 31);

   dw[124] =
      util_bitpack_uint(values->Entry[3][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][14][7], 28, 31);

   dw[125] =
      util_bitpack_uint(values->Entry[3][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][14][15], 28, 31);

   dw[126] =
      util_bitpack_uint(values->Entry[3][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[3][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[3][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[3][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[3][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[3][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[3][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[3][15][7], 28, 31);

   dw[127] =
      util_bitpack_uint(values->Entry[3][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[3][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[3][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[3][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[3][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[3][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[3][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[3][15][15], 28, 31);

   dw[128] =
      util_bitpack_uint(values->Entry[4][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][0][7], 28, 31);

   dw[129] =
      util_bitpack_uint(values->Entry[4][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][0][15], 28, 31);

   dw[130] =
      util_bitpack_uint(values->Entry[4][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][1][7], 28, 31);

   dw[131] =
      util_bitpack_uint(values->Entry[4][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][1][15], 28, 31);

   dw[132] =
      util_bitpack_uint(values->Entry[4][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][2][7], 28, 31);

   dw[133] =
      util_bitpack_uint(values->Entry[4][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][2][15], 28, 31);

   dw[134] =
      util_bitpack_uint(values->Entry[4][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][3][7], 28, 31);

   dw[135] =
      util_bitpack_uint(values->Entry[4][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][3][15], 28, 31);

   dw[136] =
      util_bitpack_uint(values->Entry[4][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][4][7], 28, 31);

   dw[137] =
      util_bitpack_uint(values->Entry[4][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][4][15], 28, 31);

   dw[138] =
      util_bitpack_uint(values->Entry[4][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][5][7], 28, 31);

   dw[139] =
      util_bitpack_uint(values->Entry[4][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][5][15], 28, 31);

   dw[140] =
      util_bitpack_uint(values->Entry[4][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][6][7], 28, 31);

   dw[141] =
      util_bitpack_uint(values->Entry[4][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][6][15], 28, 31);

   dw[142] =
      util_bitpack_uint(values->Entry[4][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][7][7], 28, 31);

   dw[143] =
      util_bitpack_uint(values->Entry[4][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][7][15], 28, 31);

   dw[144] =
      util_bitpack_uint(values->Entry[4][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][8][7], 28, 31);

   dw[145] =
      util_bitpack_uint(values->Entry[4][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][8][15], 28, 31);

   dw[146] =
      util_bitpack_uint(values->Entry[4][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][9][7], 28, 31);

   dw[147] =
      util_bitpack_uint(values->Entry[4][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][9][15], 28, 31);

   dw[148] =
      util_bitpack_uint(values->Entry[4][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][10][7], 28, 31);

   dw[149] =
      util_bitpack_uint(values->Entry[4][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][10][15], 28, 31);

   dw[150] =
      util_bitpack_uint(values->Entry[4][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][11][7], 28, 31);

   dw[151] =
      util_bitpack_uint(values->Entry[4][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][11][15], 28, 31);

   dw[152] =
      util_bitpack_uint(values->Entry[4][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][12][7], 28, 31);

   dw[153] =
      util_bitpack_uint(values->Entry[4][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][12][15], 28, 31);

   dw[154] =
      util_bitpack_uint(values->Entry[4][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][13][7], 28, 31);

   dw[155] =
      util_bitpack_uint(values->Entry[4][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][13][15], 28, 31);

   dw[156] =
      util_bitpack_uint(values->Entry[4][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][14][7], 28, 31);

   dw[157] =
      util_bitpack_uint(values->Entry[4][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][14][15], 28, 31);

   dw[158] =
      util_bitpack_uint(values->Entry[4][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[4][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[4][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[4][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[4][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[4][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[4][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[4][15][7], 28, 31);

   dw[159] =
      util_bitpack_uint(values->Entry[4][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[4][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[4][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[4][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[4][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[4][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[4][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[4][15][15], 28, 31);

   dw[160] =
      util_bitpack_uint(values->Entry[5][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][0][7], 28, 31);

   dw[161] =
      util_bitpack_uint(values->Entry[5][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][0][15], 28, 31);

   dw[162] =
      util_bitpack_uint(values->Entry[5][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][1][7], 28, 31);

   dw[163] =
      util_bitpack_uint(values->Entry[5][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][1][15], 28, 31);

   dw[164] =
      util_bitpack_uint(values->Entry[5][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][2][7], 28, 31);

   dw[165] =
      util_bitpack_uint(values->Entry[5][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][2][15], 28, 31);

   dw[166] =
      util_bitpack_uint(values->Entry[5][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][3][7], 28, 31);

   dw[167] =
      util_bitpack_uint(values->Entry[5][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][3][15], 28, 31);

   dw[168] =
      util_bitpack_uint(values->Entry[5][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][4][7], 28, 31);

   dw[169] =
      util_bitpack_uint(values->Entry[5][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][4][15], 28, 31);

   dw[170] =
      util_bitpack_uint(values->Entry[5][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][5][7], 28, 31);

   dw[171] =
      util_bitpack_uint(values->Entry[5][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][5][15], 28, 31);

   dw[172] =
      util_bitpack_uint(values->Entry[5][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][6][7], 28, 31);

   dw[173] =
      util_bitpack_uint(values->Entry[5][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][6][15], 28, 31);

   dw[174] =
      util_bitpack_uint(values->Entry[5][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][7][7], 28, 31);

   dw[175] =
      util_bitpack_uint(values->Entry[5][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][7][15], 28, 31);

   dw[176] =
      util_bitpack_uint(values->Entry[5][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][8][7], 28, 31);

   dw[177] =
      util_bitpack_uint(values->Entry[5][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][8][15], 28, 31);

   dw[178] =
      util_bitpack_uint(values->Entry[5][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][9][7], 28, 31);

   dw[179] =
      util_bitpack_uint(values->Entry[5][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][9][15], 28, 31);

   dw[180] =
      util_bitpack_uint(values->Entry[5][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][10][7], 28, 31);

   dw[181] =
      util_bitpack_uint(values->Entry[5][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][10][15], 28, 31);

   dw[182] =
      util_bitpack_uint(values->Entry[5][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][11][7], 28, 31);

   dw[183] =
      util_bitpack_uint(values->Entry[5][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][11][15], 28, 31);

   dw[184] =
      util_bitpack_uint(values->Entry[5][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][12][7], 28, 31);

   dw[185] =
      util_bitpack_uint(values->Entry[5][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][12][15], 28, 31);

   dw[186] =
      util_bitpack_uint(values->Entry[5][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][13][7], 28, 31);

   dw[187] =
      util_bitpack_uint(values->Entry[5][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][13][15], 28, 31);

   dw[188] =
      util_bitpack_uint(values->Entry[5][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][14][7], 28, 31);

   dw[189] =
      util_bitpack_uint(values->Entry[5][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][14][15], 28, 31);

   dw[190] =
      util_bitpack_uint(values->Entry[5][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[5][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[5][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[5][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[5][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[5][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[5][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[5][15][7], 28, 31);

   dw[191] =
      util_bitpack_uint(values->Entry[5][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[5][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[5][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[5][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[5][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[5][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[5][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[5][15][15], 28, 31);

   dw[192] =
      util_bitpack_uint(values->Entry[6][0][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][0][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][0][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][0][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][0][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][0][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][0][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][0][7], 28, 31);

   dw[193] =
      util_bitpack_uint(values->Entry[6][0][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][0][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][0][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][0][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][0][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][0][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][0][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][0][15], 28, 31);

   dw[194] =
      util_bitpack_uint(values->Entry[6][1][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][1][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][1][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][1][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][1][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][1][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][1][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][1][7], 28, 31);

   dw[195] =
      util_bitpack_uint(values->Entry[6][1][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][1][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][1][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][1][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][1][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][1][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][1][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][1][15], 28, 31);

   dw[196] =
      util_bitpack_uint(values->Entry[6][2][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][2][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][2][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][2][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][2][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][2][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][2][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][2][7], 28, 31);

   dw[197] =
      util_bitpack_uint(values->Entry[6][2][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][2][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][2][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][2][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][2][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][2][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][2][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][2][15], 28, 31);

   dw[198] =
      util_bitpack_uint(values->Entry[6][3][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][3][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][3][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][3][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][3][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][3][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][3][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][3][7], 28, 31);

   dw[199] =
      util_bitpack_uint(values->Entry[6][3][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][3][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][3][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][3][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][3][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][3][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][3][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][3][15], 28, 31);

   dw[200] =
      util_bitpack_uint(values->Entry[6][4][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][4][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][4][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][4][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][4][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][4][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][4][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][4][7], 28, 31);

   dw[201] =
      util_bitpack_uint(values->Entry[6][4][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][4][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][4][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][4][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][4][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][4][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][4][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][4][15], 28, 31);

   dw[202] =
      util_bitpack_uint(values->Entry[6][5][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][5][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][5][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][5][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][5][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][5][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][5][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][5][7], 28, 31);

   dw[203] =
      util_bitpack_uint(values->Entry[6][5][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][5][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][5][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][5][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][5][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][5][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][5][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][5][15], 28, 31);

   dw[204] =
      util_bitpack_uint(values->Entry[6][6][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][6][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][6][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][6][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][6][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][6][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][6][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][6][7], 28, 31);

   dw[205] =
      util_bitpack_uint(values->Entry[6][6][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][6][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][6][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][6][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][6][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][6][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][6][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][6][15], 28, 31);

   dw[206] =
      util_bitpack_uint(values->Entry[6][7][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][7][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][7][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][7][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][7][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][7][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][7][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][7][7], 28, 31);

   dw[207] =
      util_bitpack_uint(values->Entry[6][7][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][7][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][7][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][7][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][7][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][7][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][7][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][7][15], 28, 31);

   dw[208] =
      util_bitpack_uint(values->Entry[6][8][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][8][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][8][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][8][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][8][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][8][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][8][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][8][7], 28, 31);

   dw[209] =
      util_bitpack_uint(values->Entry[6][8][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][8][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][8][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][8][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][8][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][8][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][8][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][8][15], 28, 31);

   dw[210] =
      util_bitpack_uint(values->Entry[6][9][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][9][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][9][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][9][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][9][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][9][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][9][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][9][7], 28, 31);

   dw[211] =
      util_bitpack_uint(values->Entry[6][9][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][9][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][9][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][9][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][9][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][9][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][9][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][9][15], 28, 31);

   dw[212] =
      util_bitpack_uint(values->Entry[6][10][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][10][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][10][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][10][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][10][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][10][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][10][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][10][7], 28, 31);

   dw[213] =
      util_bitpack_uint(values->Entry[6][10][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][10][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][10][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][10][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][10][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][10][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][10][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][10][15], 28, 31);

   dw[214] =
      util_bitpack_uint(values->Entry[6][11][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][11][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][11][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][11][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][11][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][11][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][11][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][11][7], 28, 31);

   dw[215] =
      util_bitpack_uint(values->Entry[6][11][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][11][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][11][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][11][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][11][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][11][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][11][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][11][15], 28, 31);

   dw[216] =
      util_bitpack_uint(values->Entry[6][12][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][12][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][12][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][12][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][12][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][12][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][12][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][12][7], 28, 31);

   dw[217] =
      util_bitpack_uint(values->Entry[6][12][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][12][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][12][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][12][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][12][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][12][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][12][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][12][15], 28, 31);

   dw[218] =
      util_bitpack_uint(values->Entry[6][13][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][13][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][13][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][13][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][13][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][13][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][13][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][13][7], 28, 31);

   dw[219] =
      util_bitpack_uint(values->Entry[6][13][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][13][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][13][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][13][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][13][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][13][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][13][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][13][15], 28, 31);

   dw[220] =
      util_bitpack_uint(values->Entry[6][14][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][14][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][14][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][14][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][14][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][14][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][14][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][14][7], 28, 31);

   dw[221] =
      util_bitpack_uint(values->Entry[6][14][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][14][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][14][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][14][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][14][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][14][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][14][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][14][15], 28, 31);

   dw[222] =
      util_bitpack_uint(values->Entry[6][15][0], 0, 3) |
      util_bitpack_uint(values->Entry[6][15][1], 4, 7) |
      util_bitpack_uint(values->Entry[6][15][2], 8, 11) |
      util_bitpack_uint(values->Entry[6][15][3], 12, 15) |
      util_bitpack_uint(values->Entry[6][15][4], 16, 19) |
      util_bitpack_uint(values->Entry[6][15][5], 20, 23) |
      util_bitpack_uint(values->Entry[6][15][6], 24, 27) |
      util_bitpack_uint(values->Entry[6][15][7], 28, 31);

   dw[223] =
      util_bitpack_uint(values->Entry[6][15][8], 0, 3) |
      util_bitpack_uint(values->Entry[6][15][9], 4, 7) |
      util_bitpack_uint(values->Entry[6][15][10], 8, 11) |
      util_bitpack_uint(values->Entry[6][15][11], 12, 15) |
      util_bitpack_uint(values->Entry[6][15][12], 16, 19) |
      util_bitpack_uint(values->Entry[6][15][13], 20, 23) |
      util_bitpack_uint(values->Entry[6][15][14], 24, 27) |
      util_bitpack_uint(values->Entry[6][15][15], 28, 31);
}

#define GFX30_SO_DECL_length                   1
struct GFX30_SO_DECL {
   uint32_t                             ComponentMask;
   uint32_t                             RegisterIndex;
   bool                                 HoleFlag;
   uint32_t                             OutputBufferSlot;
};

static inline __attribute__((always_inline)) void
GFX30_SO_DECL_pack(__attribute__((unused)) __gen_user_data *data,
                   __attribute__((unused)) void * restrict dst,
                   __attribute__((unused)) const struct GFX30_SO_DECL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ComponentMask, 0, 3) |
      util_bitpack_uint(values->RegisterIndex, 4, 9) |
      util_bitpack_uint(values->HoleFlag, 11, 11) |
      util_bitpack_uint(values->OutputBufferSlot, 12, 13);
}

#define GFX30_SO_DECL_ENTRY_length             2
struct GFX30_SO_DECL_ENTRY {
   struct GFX30_SO_DECL                 Stream0Decl;
   struct GFX30_SO_DECL                 Stream1Decl;
   struct GFX30_SO_DECL                 Stream2Decl;
   struct GFX30_SO_DECL                 Stream3Decl;
};

static inline __attribute__((always_inline)) void
GFX30_SO_DECL_ENTRY_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_SO_DECL_ENTRY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   uint32_t v0_0;
   GFX30_SO_DECL_pack(data, &v0_0, &values->Stream0Decl);

   uint32_t v0_1;
   GFX30_SO_DECL_pack(data, &v0_1, &values->Stream1Decl);

   dw[0] =
      util_bitpack_uint(v0_0, 0, 15) |
      util_bitpack_uint(v0_1, 16, 31);

   uint32_t v1_0;
   GFX30_SO_DECL_pack(data, &v1_0, &values->Stream2Decl);

   uint32_t v1_1;
   GFX30_SO_DECL_pack(data, &v1_1, &values->Stream3Decl);

   dw[1] =
      util_bitpack_uint(v1_0, 0, 15) |
      util_bitpack_uint(v1_1, 16, 31);
}

#define GFX30_VDENC_SURFACE_CONTROL_BITS_length      1
struct GFX30_VDENC_SURFACE_CONTROL_BITS {
   uint32_t                             MOCS;
   uint32_t                             ArbitrationPriorityControl;
#define Highestpriority                          0
#define Secondhighestpriority                    1
#define Thirdhighestpriority                     2
#define Lowestpriority                           3
   bool                                 MemoryCompressionEnable;
   uint32_t                             MemoryCompressionMode;
   uint32_t                             CacheSelect;
};

static inline __attribute__((always_inline)) void
GFX30_VDENC_SURFACE_CONTROL_BITS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_VDENC_SURFACE_CONTROL_BITS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->ArbitrationPriorityControl, 7, 8) |
      util_bitpack_uint(values->MemoryCompressionEnable, 9, 9) |
      util_bitpack_uint(values->MemoryCompressionMode, 10, 10) |
      util_bitpack_uint(values->CacheSelect, 12, 12);
}

#define GFX30_VDENC_PICTURE_length             3
struct GFX30_VDENC_PICTURE {
   __gen_address_type                   Address;
   struct GFX30_VDENC_SURFACE_CONTROL_BITS PictureFields;
};

static inline __attribute__((always_inline)) void
GFX30_VDENC_PICTURE_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_VDENC_PICTURE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0_address =
      __gen_address(data, &dw[0], values->Address, 0, 0, 63);
   dw[0] = v0_address;
   dw[1] = v0_address >> 32;

   GFX30_VDENC_SURFACE_CONTROL_BITS_pack(data, &dw[2], &values->PictureFields);
}

#define GFX30_VDENC_SURFACE_STATE_FIELDS_length      4
struct GFX30_VDENC_SURFACE_STATE_FIELDS {
   float                                CrVCbUPixelOffsetVDirection;
   bool                                 SurfaceFormatByteSwizzle;
   uint32_t                             Colorspaceselection;
   uint32_t                             Width;
   uint32_t                             Height;
   uint32_t                             TileWalk;
#define TW_XMAJOR                                0
#define TW_YMAJOR                                1
   uint32_t                             TiledSurface;
   bool                                 HalfPitchforChroma;
   uint32_t                             SurfacePitch;
   uint32_t                             ChromaDownsampleFilterControl;
   uint32_t                             SurfaceFormat;
#define VDENC_YUV422                             0
#define VDENC_RGBA4444                           1
#define VDENC_YUV444                             2
#define VDENC_Y8_UNORM                           3
#define VDENC_PLANAR_420_8                       4
#define VDENC_P010                               13
   uint32_t                             YOffsetforUCb;
   uint32_t                             XOffsetforUCb;
   uint32_t                             YOffsetforVCr;
   uint32_t                             XOffsetforVCr;
};

static inline __attribute__((always_inline)) void
GFX30_VDENC_SURFACE_STATE_FIELDS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_VDENC_SURFACE_STATE_FIELDS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_ufixed(values->CrVCbUPixelOffsetVDirection, 0, 1, 2) |
      util_bitpack_uint(values->SurfaceFormatByteSwizzle, 2, 2) |
      util_bitpack_uint(values->Colorspaceselection, 3, 3) |
      util_bitpack_uint(values->Width, 4, 17) |
      util_bitpack_uint(values->Height, 18, 31);

   dw[1] =
      util_bitpack_uint(values->TileWalk, 0, 0) |
      util_bitpack_uint(values->TiledSurface, 1, 1) |
      util_bitpack_uint(values->HalfPitchforChroma, 2, 2) |
      util_bitpack_uint(values->SurfacePitch, 3, 19) |
      util_bitpack_uint(values->ChromaDownsampleFilterControl, 20, 22) |
      util_bitpack_uint(values->SurfaceFormat, 27, 31);

   dw[2] =
      util_bitpack_uint(values->YOffsetforUCb, 0, 14) |
      util_bitpack_uint(values->XOffsetforUCb, 16, 30);

   dw[3] =
      util_bitpack_uint(values->YOffsetforVCr, 0, 15) |
      util_bitpack_uint(values->XOffsetforVCr, 16, 28);
}

#define GFX30_VD_CONTROL_STATE_BODY_length      2
struct GFX30_VD_CONTROL_STATE_BODY {
   bool                                 PipelineInitialization;
   bool                                 ScalableModePipeLock;
   bool                                 ScalableModePipeUnlock;
   bool                                 MemoryImplicitFlush;
};

static inline __attribute__((always_inline)) void
GFX30_VD_CONTROL_STATE_BODY_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_VD_CONTROL_STATE_BODY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PipelineInitialization, 0, 0);

   dw[1] =
      util_bitpack_uint(values->ScalableModePipeLock, 0, 0) |
      util_bitpack_uint(values->ScalableModePipeUnlock, 1, 1) |
      util_bitpack_uint(values->MemoryImplicitFlush, 2, 2);
}

#define GFX30_VERTEX_BUFFER_STATE_length       4
struct GFX30_VERTEX_BUFFER_STATE {
   uint32_t                             BufferPitch;
   bool                                 NullVertexBuffer;
   bool                                 AddressModifyEnable;
   uint32_t                             MOCS;
   bool                                 L3BypassDisable;
   uint32_t                             VertexBufferIndex;
   __gen_address_type                   BufferStartingAddress;
   uint32_t                             BufferSize;
};

static inline __attribute__((always_inline)) void
GFX30_VERTEX_BUFFER_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_VERTEX_BUFFER_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->BufferPitch, 0, 11) |
      util_bitpack_uint(values->NullVertexBuffer, 13, 13) |
      util_bitpack_uint(values->AddressModifyEnable, 14, 14) |
      util_bitpack_uint_nonzero(values->MOCS, 16, 22) |
      util_bitpack_uint(values->L3BypassDisable, 25, 25) |
      util_bitpack_uint(values->VertexBufferIndex, 26, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->BufferStartingAddress, 0, 0, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;

   dw[3] =
      util_bitpack_uint(values->BufferSize, 0, 31);
}

#define GFX30_VERTEX_ELEMENT_STATE_length      2
struct GFX30_VERTEX_ELEMENT_STATE {
   uint32_t                             SourceElementOffset;
   bool                                 EdgeFlagEnable;
   uint32_t                             SourceElementFormat;
   bool                                 Valid;
   uint32_t                             VertexBufferIndex;
   enum GFX30_3D_Vertex_Component_Control Component3Control;
   enum GFX30_3D_Vertex_Component_Control Component2Control;
   enum GFX30_3D_Vertex_Component_Control Component1Control;
   enum GFX30_3D_Vertex_Component_Control Component0Control;
};

static inline __attribute__((always_inline)) void
GFX30_VERTEX_ELEMENT_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_VERTEX_ELEMENT_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->SourceElementOffset, 0, 11) |
      util_bitpack_uint(values->EdgeFlagEnable, 15, 15) |
      util_bitpack_uint(values->SourceElementFormat, 16, 24) |
      util_bitpack_uint(values->Valid, 25, 25) |
      util_bitpack_uint(values->VertexBufferIndex, 26, 31);

   dw[1] =
      util_bitpack_uint(values->Component3Control, 16, 18) |
      util_bitpack_uint(values->Component2Control, 20, 22) |
      util_bitpack_uint(values->Component1Control, 24, 26) |
      util_bitpack_uint(values->Component0Control, 28, 30);
}

#define GFX30_3DMESH_1D_length                 3
#define GFX30_3DMESH_1D_length_bias            2
#define GFX30_3DMESH_1D_header                  \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =      1,  \
   ._3DCommandOpcode                    =      3,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DMESH_1D {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   bool                                 UAVCoherencyRequired;
   bool                                 IndirectParameterEnable;
   uint32_t                             TBIMREnabled;
   uint32_t                             ExtendedParameter0Present;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ThreadGroupCountX;
   uint32_t                             StartingThreadGroupIDX;
   uint32_t                             ExtendedParameter0XP0;
};

static inline __attribute__((always_inline)) void
GFX30_3DMESH_1D_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_3DMESH_1D * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->UAVCoherencyRequired, 9, 9) |
      util_bitpack_uint(values->IndirectParameterEnable, 10, 10) |
      util_bitpack_uint(values->TBIMREnabled, 13, 13) |
      util_bitpack_uint(values->ExtendedParameter0Present, 14, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ThreadGroupCountX, 0, 31);

   dw[2] =
      util_bitpack_uint(values->StartingThreadGroupIDX, 0, 31);
}

#define GFX30_3DMESH_3D_length                 4
#define GFX30_3DMESH_3D_length_bias            2
#define GFX30_3DMESH_3D_header                  \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =      2,  \
   ._3DCommandOpcode                    =      3,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DMESH_3D {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   bool                                 UAVCoherencyRequired;
   bool                                 IndirectParameterEnable;
   uint32_t                             TBIMREnabled;
   uint32_t                             ExtendedParameter0Present;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ThreadGroupCountX;
   uint32_t                             ThreadGroupCountY;
   uint32_t                             ThreadGroupCountZ;
   uint32_t                             ExtendedParameter0XP0;
};

static inline __attribute__((always_inline)) void
GFX30_3DMESH_3D_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_3DMESH_3D * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->UAVCoherencyRequired, 9, 9) |
      util_bitpack_uint(values->IndirectParameterEnable, 10, 10) |
      util_bitpack_uint(values->TBIMREnabled, 13, 13) |
      util_bitpack_uint(values->ExtendedParameter0Present, 14, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ThreadGroupCountX, 0, 31);

   dw[2] =
      util_bitpack_uint(values->ThreadGroupCountY, 0, 31);

   dw[3] =
      util_bitpack_uint(values->ThreadGroupCountZ, 0, 31);
}

#define GFX30_3DPRIMITIVE_length               7
#define GFX30_3DPRIMITIVE_length_bias          2
#define GFX30_3DPRIMITIVE_header                \
   .DWordLength                         =      5,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      3,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DPRIMITIVE {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   bool                                 UAVCoherencyRequired;
   bool                                 IndirectParameterEnable;
   uint32_t                             ExtendedParametersPresent;
   uint32_t                             TBIMREnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   enum GFX30_3D_Prim_Topo_Type         PrimitiveTopologyType;
   uint32_t                             VertexAccessType;
#define SEQUENTIAL                               0
#define RANDOM                                   1
   bool                                 EndOffsetEnable;
   uint32_t                             VertexCountPerInstance;
   uint32_t                             StartVertexLocation;
   uint32_t                             InstanceCount;
   uint32_t                             StartInstanceLocation;
   int32_t                              BaseVertexLocation;
   uint32_t                             ExtendedParameter0;
   uint32_t                             ExtendedParameter1;
   uint32_t                             ExtendedParameter2;
};

static inline __attribute__((always_inline)) void
GFX30_3DPRIMITIVE_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_3DPRIMITIVE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->UAVCoherencyRequired, 9, 9) |
      util_bitpack_uint(values->IndirectParameterEnable, 10, 10) |
      util_bitpack_uint(values->ExtendedParametersPresent, 11, 11) |
      util_bitpack_uint(values->TBIMREnable, 13, 13) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PrimitiveTopologyType, 0, 5) |
      util_bitpack_uint(values->VertexAccessType, 8, 8) |
      util_bitpack_uint(values->EndOffsetEnable, 9, 9);

   dw[2] =
      util_bitpack_uint(values->VertexCountPerInstance, 0, 31);

   dw[3] =
      util_bitpack_uint(values->StartVertexLocation, 0, 31);

   dw[4] =
      util_bitpack_uint(values->InstanceCount, 0, 31);

   dw[5] =
      util_bitpack_uint(values->StartInstanceLocation, 0, 31);

   dw[6] =
      util_bitpack_sint(values->BaseVertexLocation, 0, 31);
}

#define GFX30_3DPRIMITIVE_EXTENDED_length     10
#define GFX30_3DPRIMITIVE_EXTENDED_length_bias      2
#define GFX30_3DPRIMITIVE_EXTENDED_header       \
   .DWordLength                         =      8,  \
   .ExtendedParametersPresent           =      1,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      3,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DPRIMITIVE_EXTENDED {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   bool                                 UAVCoherencyRequired;
   bool                                 IndirectParameterEnable;
   bool                                 ExtendedParametersPresent;
   uint32_t                             TBIMREnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   enum GFX30_3D_Prim_Topo_Type         PrimitiveTopologyType;
   uint32_t                             VertexAccessType;
#define SEQUENTIAL                               0
#define RANDOM                                   1
   bool                                 EndOffsetEnable;
   uint32_t                             VertexCountPerInstance;
   uint32_t                             StartVertexLocation;
   uint32_t                             InstanceCount;
   uint32_t                             StartInstanceLocation;
   int32_t                              BaseVertexLocation;
   uint32_t                             ExtendedParameter0;
   uint32_t                             ExtendedParameter1;
   uint32_t                             ExtendedParameter2;
};

static inline __attribute__((always_inline)) void
GFX30_3DPRIMITIVE_EXTENDED_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DPRIMITIVE_EXTENDED * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->UAVCoherencyRequired, 9, 9) |
      util_bitpack_uint(values->IndirectParameterEnable, 10, 10) |
      util_bitpack_uint(values->ExtendedParametersPresent, 11, 11) |
      util_bitpack_uint(values->TBIMREnable, 13, 13) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PrimitiveTopologyType, 0, 5) |
      util_bitpack_uint(values->VertexAccessType, 8, 8) |
      util_bitpack_uint(values->EndOffsetEnable, 9, 9);

   dw[2] =
      util_bitpack_uint(values->VertexCountPerInstance, 0, 31);

   dw[3] =
      util_bitpack_uint(values->StartVertexLocation, 0, 31);

   dw[4] =
      util_bitpack_uint(values->InstanceCount, 0, 31);

   dw[5] =
      util_bitpack_uint(values->StartInstanceLocation, 0, 31);

   dw[6] =
      util_bitpack_sint(values->BaseVertexLocation, 0, 31);

   dw[7] =
      util_bitpack_uint(values->ExtendedParameter0, 0, 31);

   dw[8] =
      util_bitpack_uint(values->ExtendedParameter1, 0, 31);

   dw[9] =
      util_bitpack_uint(values->ExtendedParameter2, 0, 31);
}

#define GFX30_3DSTATE_3D_MODE_length           5
#define GFX30_3DSTATE_3D_MODE_length_bias      2
#define GFX30_3DSTATE_3D_MODE_header            \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     30,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_3D_MODE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             CrossSliceHashingMode;
#define NormalMode                               0
#define Disable                                  1
#define hashing32x32                             3
   bool                                 _3DScoreboardHashingMode;
   bool                                 SubsliceHashingTableEnable;
   bool                                 SliceHashingTableEnable;
   bool                                 DX10OGLBorderModeforYCRCB;
   bool                                 EnableOOOreadsinRCPB;
   int32_t                              CrossSliceHashingModeMask;
   bool                                 _3DScoreboardHashingModeMask;
   bool                                 SubsliceHashingTableEnableMask;
   bool                                 SliceHashingTableEnableMask;
   bool                                 DX10OGLBorderModeforYCRCBMask;
   bool                                 EnableOOOreadsinRCPBMask;
   uint32_t                             AMFSMOCS;
   bool                                 StateCacheredirecttoCSsectionenable;
   uint32_t                             AMFSMOCSMask;
   bool                                 StateCacheredirecttoCSsectionenableMask;
   bool                                 RCCRHWOOptimizationDisable;
   bool                                 RCCRHWOOptimizationDisableMask;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_3D_MODE_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_3DSTATE_3D_MODE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->CrossSliceHashingMode, 0, 1) |
      util_bitpack_uint(values->_3DScoreboardHashingMode, 4, 4) |
      util_bitpack_uint(values->SubsliceHashingTableEnable, 5, 5) |
      util_bitpack_uint(values->SliceHashingTableEnable, 6, 6) |
      util_bitpack_uint(values->DX10OGLBorderModeforYCRCB, 10, 10) |
      util_bitpack_uint(values->EnableOOOreadsinRCPB, 11, 11) |
      util_bitpack_sint(values->CrossSliceHashingModeMask, 16, 17) |
      util_bitpack_uint(values->_3DScoreboardHashingModeMask, 20, 20) |
      util_bitpack_uint(values->SubsliceHashingTableEnableMask, 21, 21) |
      util_bitpack_uint(values->SliceHashingTableEnableMask, 22, 22) |
      util_bitpack_uint(values->DX10OGLBorderModeforYCRCBMask, 26, 26) |
      util_bitpack_uint(values->EnableOOOreadsinRCPBMask, 27, 27);

   dw[2] =
      util_bitpack_uint(values->AMFSMOCS, 0, 6) |
      util_bitpack_uint(values->StateCacheredirecttoCSsectionenable, 9, 9) |
      util_bitpack_uint(values->AMFSMOCSMask, 16, 22) |
      util_bitpack_uint(values->StateCacheredirecttoCSsectionenableMask, 25, 25);

   dw[3] =
      util_bitpack_uint(values->RCCRHWOOptimizationDisable, 15, 15) |
      util_bitpack_uint(values->RCCRHWOOptimizationDisableMask, 31, 31);

   dw[4] = 0;
}

#define GFX30_3DSTATE_AA_LINE_PARAMETERS_length      3
#define GFX30_3DSTATE_AA_LINE_PARAMETERS_length_bias      2
#define GFX30_3DSTATE_AA_LINE_PARAMETERS_header \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     10,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_AA_LINE_PARAMETERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   float                                AACoverageSlope;
   float                                AAPointCoverageSlope;
   float                                AACoverageBias;
   float                                AAPointCoverageBias;
   float                                AACoverageEndCapSlope;
   float                                AAPointCoverageEndCapSlope;
   float                                AACoverageEndCapBias;
   float                                AAPointCoverageEndCapBias;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_AA_LINE_PARAMETERS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_AA_LINE_PARAMETERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_ufixed(values->AACoverageSlope, 0, 7, 8) |
      util_bitpack_ufixed(values->AAPointCoverageSlope, 8, 15, 8) |
      util_bitpack_ufixed(values->AACoverageBias, 16, 23, 8) |
      util_bitpack_ufixed(values->AAPointCoverageBias, 24, 31, 8);

   dw[2] =
      util_bitpack_ufixed(values->AACoverageEndCapSlope, 0, 7, 8) |
      util_bitpack_ufixed(values->AAPointCoverageEndCapSlope, 8, 15, 8) |
      util_bitpack_ufixed(values->AACoverageEndCapBias, 16, 23, 8) |
      util_bitpack_ufixed(values->AAPointCoverageEndCapBias, 24, 31, 8);
}

#define GFX30_3DSTATE_BINDING_TABLE_EDIT_DS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_EDIT_DS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     70,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_EDIT_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             BindingTableEditTarget;
#define AllCores                                 3
#define Core1                                    2
#define Core0                                    1
   uint32_t                             BindingTableBlockClear;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_EDIT_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_EDIT_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BindingTableEditTarget, 0, 1) |
      util_bitpack_uint(values->BindingTableBlockClear, 16, 31);
}

#define GFX30_3DSTATE_BINDING_TABLE_EDIT_GS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_EDIT_GS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     68,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_EDIT_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             BindingTableEditTarget;
#define AllCores                                 3
#define Core1                                    2
#define Core0                                    1
   uint32_t                             BindingTableBlockClear;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_EDIT_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_EDIT_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BindingTableEditTarget, 0, 1) |
      util_bitpack_uint(values->BindingTableBlockClear, 16, 31);
}

#define GFX30_3DSTATE_BINDING_TABLE_EDIT_HS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_EDIT_HS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     69,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_EDIT_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             BindingTableEditTarget;
#define AllCores                                 3
#define Core1                                    2
#define Core0                                    1
   uint32_t                             BindingTableBlockClear;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_EDIT_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_EDIT_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BindingTableEditTarget, 0, 1) |
      util_bitpack_uint(values->BindingTableBlockClear, 16, 31);
}

#define GFX30_3DSTATE_BINDING_TABLE_EDIT_PS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_EDIT_PS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     71,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_EDIT_PS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             BindingTableEditTarget;
#define AllCores                                 3
#define Core1                                    2
#define Core0                                    1
   uint32_t                             BindingTableBlockClear;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_EDIT_PS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_EDIT_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BindingTableEditTarget, 0, 1) |
      util_bitpack_uint(values->BindingTableBlockClear, 16, 31);
}

#define GFX30_3DSTATE_BINDING_TABLE_EDIT_VS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_EDIT_VS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     67,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_EDIT_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             BindingTableEditTarget;
#define AllCores                                 3
#define Core1                                    2
#define Core0                                    1
   uint32_t                             BindingTableBlockClear;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_EDIT_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_EDIT_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BindingTableEditTarget, 0, 1) |
      util_bitpack_uint(values->BindingTableBlockClear, 16, 31);
}

#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS_length      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     40,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoDSBindingTable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoDSBindingTable, 5, 20);
}

#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS_length      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     41,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoGSBindingTable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoGSBindingTable, 5, 20);
}

#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS_length      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     39,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoHSBindingTable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoHSBindingTable, 5, 20);
}

#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS_length      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     42,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoPSBindingTable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoPSBindingTable, 5, 20);
}

#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS_length      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     38,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoVSBindingTable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POINTERS_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoVSBindingTable, 5, 20);
}

#define GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC_length      4
#define GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC_length_bias      2
#define GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC_header\
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     25,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MOCS;
   __gen_address_type                   BindingTablePoolBaseAddress;
   uint32_t                             BindingTablePoolBufferSize;
#define NoValidData                              0
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC_pack(__attribute__((unused)) __gen_user_data *data,
                                            __attribute__((unused)) void * restrict dst,
                                            __attribute__((unused)) const struct GFX30_3DSTATE_BINDING_TABLE_POOL_ALLOC * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->BindingTablePoolBaseAddress, v1, 12, 63);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   dw[3] =
      util_bitpack_uint(values->BindingTablePoolBufferSize, 12, 31);
}

#define GFX30_3DSTATE_BLEND_STATE_POINTERS_length      2
#define GFX30_3DSTATE_BLEND_STATE_POINTERS_length_bias      2
#define GFX30_3DSTATE_BLEND_STATE_POINTERS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     36,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_BLEND_STATE_POINTERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 BlendStatePointerValid;
   uint64_t                             BlendStatePointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BLEND_STATE_POINTERS_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_3DSTATE_BLEND_STATE_POINTERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->BlendStatePointerValid, 0, 0) |
      __gen_offset(values->BlendStatePointer, 6, 31);
}

#define GFX30_3DSTATE_BTD_length               6
#define GFX30_3DSTATE_BTD_length_bias          2
#define GFX30_3DSTATE_BTD_header                \
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =      6,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3,  \
   .PerDSSMemoryBackedBufferSize        =      6

struct GFX30_3DSTATE_BTD {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             DispatchTimeoutCounter;
#define _64clocks                                0
#define _128clocks                               1
#define _192clocks                               2
#define _256clocks                               3
#define _512clocks                               4
#define _1024clocks                              5
#define _2048clocks                              6
#define _4096clocks                              7
   uint32_t                             ControlsthemaximumnumberofoutstandingRayQueriesperSS;
#define RAYS_QUERIES_OUTSTANDING_128             0
#define RAYS_QUERIES_OUTSTANDING_256             1
#define RAYS_QUERIES_OUTSTANDING_512             2
#define RAYS_QUERIES_OUTSTANDING_1024            3
   uint32_t                             DynamicstackmanagementmechanismMISSPENALTY;
#define MISS_PENALTY_128                         7
#define MISS_PENALTY_64                          6
#define MISS_PENALTY_32                          5
#define MISS_PENALTY_16                          4
#define MISS_PENALTY_8                           3
#define MISS_PENALTY_4                           2
#define MISS_PENALTY_2                           1
#define MISS_PENALTY_1                           0
   uint32_t                             DynamicstackmanagementmechanismHITREWARD;
#define HIT_REWARD_128                           7
#define HIT_REWARD_64                            6
#define HIT_REWARD_32                            5
#define HIT_REWARD_16                            4
#define HIT_REWARD_8                             3
#define HIT_REWARD_4                             2
#define HIT_REWARD_2                             1
#define HIT_REWARD_1                             0
   uint32_t                             DynamicstackmanagementmechanismSCALINGFACTOR;
#define SCALING_FACTOR_128                       7
#define SCALING_FACTOR_64                        6
#define SCALING_FACTOR_32                        5
#define SCALING_FACTOR_16                        4
#define SCALING_FACTOR_8                         3
#define SCALING_FACTOR_4                         2
#define SCALING_FACTOR_2                         1
#define SCALING_FACTOR_1                         0
   uint32_t                             DynamicstackmanagementmechanismREDUCTIONCAP;
#define REDUCTION_CAP_1024                       7
#define REDUCTION_CAP_896                        6
#define REDUCTION_CAP_768                        5
#define REDUCTION_CAP_640                        4
#define REDUCTION_CAP_512                        3
#define REDUCTION_CAP_384                        2
#define REDUCTION_CAP_256                        1
#define REDUCTION_CAP_128                        0
   bool                                 RTMemStructures64bModeEnable;
   bool                                 BTDMidthreadpreemption;
   uint32_t                             PerDSSMemoryBackedBufferSize;
#define _2KB                                     0
#define _4KB                                     1
#define _8KB                                     2
#define _16KB                                    3
#define _32KB                                    4
#define _64KB                                    5
#define _128KB                                   6
   __gen_address_type                   MemoryBackedBufferBasePointer;
   uint32_t                             ScratchSpaceBuffer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_BTD_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_3DSTATE_BTD * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DispatchTimeoutCounter, 0, 2) |
      util_bitpack_uint(values->ControlsthemaximumnumberofoutstandingRayQueriesperSS, 7, 8) |
      util_bitpack_uint(values->DynamicstackmanagementmechanismMISSPENALTY, 9, 11) |
      util_bitpack_uint(values->DynamicstackmanagementmechanismHITREWARD, 12, 14) |
      util_bitpack_uint(values->DynamicstackmanagementmechanismSCALINGFACTOR, 15, 17) |
      util_bitpack_uint(values->DynamicstackmanagementmechanismREDUCTIONCAP, 18, 20) |
      util_bitpack_uint(values->RTMemStructures64bModeEnable, 30, 30) |
      util_bitpack_uint(values->BTDMidthreadpreemption, 31, 31);

   const uint64_t v2 =
      util_bitpack_uint(values->PerDSSMemoryBackedBufferSize, 0, 2);
   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->MemoryBackedBufferBasePointer, v2, 10, 63);
   dw[2] = v2_address;
   dw[3] = (v2_address >> 32) | (v2 >> 32);

   dw[4] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[5] = 0;
}

#define GFX30_3DSTATE_CC_STATE_POINTERS_length      2
#define GFX30_3DSTATE_CC_STATE_POINTERS_length_bias      2
#define GFX30_3DSTATE_CC_STATE_POINTERS_header  \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     14,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CC_STATE_POINTERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 ColorCalcStatePointerValid;
   uint64_t                             ColorCalcStatePointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CC_STATE_POINTERS_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_CC_STATE_POINTERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ColorCalcStatePointerValid, 0, 0) |
      __gen_offset(values->ColorCalcStatePointer, 6, 31);
}

#define GFX30_3DSTATE_CHROMA_KEY_length        4
#define GFX30_3DSTATE_CHROMA_KEY_length_bias      2
#define GFX30_3DSTATE_CHROMA_KEY_header         \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =      4,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CHROMA_KEY {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ChromaKeyTableIndex;
   uint32_t                             ChromaKeyLowValue;
   uint32_t                             ChromaKeyHighValue;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CHROMA_KEY_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_3DSTATE_CHROMA_KEY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ChromaKeyTableIndex, 30, 31);

   dw[2] =
      util_bitpack_uint(values->ChromaKeyLowValue, 0, 31);

   dw[3] =
      util_bitpack_uint(values->ChromaKeyHighValue, 0, 31);
}

#define GFX30_3DSTATE_CLIP_length              4
#define GFX30_3DSTATE_CLIP_length_bias         2
#define GFX30_3DSTATE_CLIP_header               \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     18,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CLIP {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             UserClipDistanceCullTestEnableBitmask;
   bool                                 StatisticsEnable;
   bool                                 ForceClipMode;
   bool                                 ForceUserClipDistanceClipTestEnableBitmask;
   bool                                 EarlyCullEnable;
   uint32_t                             VertexSubPixelPrecisionSelect;
#define _8Bit                                    0
#define _4Bit                                    1
   bool                                 ForceUserClipDistanceCullTestEnableBitmask;
   uint32_t                             TriangleFanProvokingVertexSelect;
   uint32_t                             LineStripListProvokingVertexSelect;
   uint32_t                             TriangleStripListProvokingVertexSelect;
   uint32_t                             TriangleStripOddProvokingVertexSelect;
   bool                                 NonPerspectiveBarycentricEnable;
   bool                                 PerspectiveDivideDisable;
   uint32_t                             ClipMode;
#define CLIPMODE_NORMAL                          0
#define CLIPMODE_REJECT_ALL                      3
#define CLIPMODE_ACCEPT_ALL                      4
   uint32_t                             UserClipDistanceClipTestEnableBitmask;
   bool                                 GuardbandClipTestEnable;
   bool                                 ViewportXYClipTestEnable;
   uint32_t                             APIMode;
#define APIMODE_OGL                              0
#define APIMODE_D3D                              1
   bool                                 ClipEnable;
   uint32_t                             MaximumVPIndex;
   bool                                 ForceZeroRTAIndexEnable;
   float                                MaximumPointWidth;
   float                                MinimumPointWidth;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CLIP_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_3DSTATE_CLIP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->UserClipDistanceCullTestEnableBitmask, 0, 7) |
      util_bitpack_uint(values->StatisticsEnable, 10, 10) |
      util_bitpack_uint(values->ForceClipMode, 16, 16) |
      util_bitpack_uint(values->ForceUserClipDistanceClipTestEnableBitmask, 17, 17) |
      util_bitpack_uint(values->EarlyCullEnable, 18, 18) |
      util_bitpack_uint(values->VertexSubPixelPrecisionSelect, 19, 19) |
      util_bitpack_uint(values->ForceUserClipDistanceCullTestEnableBitmask, 20, 20);

   dw[2] =
      util_bitpack_uint(values->TriangleFanProvokingVertexSelect, 0, 1) |
      util_bitpack_uint(values->LineStripListProvokingVertexSelect, 2, 3) |
      util_bitpack_uint(values->TriangleStripListProvokingVertexSelect, 4, 5) |
      util_bitpack_uint(values->TriangleStripOddProvokingVertexSelect, 6, 7) |
      util_bitpack_uint(values->NonPerspectiveBarycentricEnable, 8, 8) |
      util_bitpack_uint(values->PerspectiveDivideDisable, 9, 9) |
      util_bitpack_uint(values->ClipMode, 13, 15) |
      util_bitpack_uint(values->UserClipDistanceClipTestEnableBitmask, 16, 23) |
      util_bitpack_uint(values->GuardbandClipTestEnable, 26, 26) |
      util_bitpack_uint(values->ViewportXYClipTestEnable, 28, 28) |
      util_bitpack_uint(values->APIMode, 30, 30) |
      util_bitpack_uint(values->ClipEnable, 31, 31);

   dw[3] =
      util_bitpack_uint(values->MaximumVPIndex, 0, 3) |
      util_bitpack_uint(values->ForceZeroRTAIndexEnable, 5, 5) |
      util_bitpack_ufixed(values->MaximumPointWidth, 6, 16, 3) |
      util_bitpack_ufixed(values->MinimumPointWidth, 17, 27, 3);
}

#define GFX30_3DSTATE_CLIP_MESH_length         2
#define GFX30_3DSTATE_CLIP_MESH_length_bias      2
#define GFX30_3DSTATE_CLIP_MESH_header          \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    129,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CLIP_MESH {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             UserClipDistanceCullTestEnableBitmask;
   uint32_t                             UserClipDistanceClipTestEnableBitmask;
   uint32_t                             PrimitiveHeaderEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CLIP_MESH_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_3DSTATE_CLIP_MESH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->UserClipDistanceCullTestEnableBitmask, 0, 7) |
      util_bitpack_uint(values->UserClipDistanceClipTestEnableBitmask, 8, 15) |
      util_bitpack_uint(values->PrimitiveHeaderEnable, 16, 16);
}

#define GFX30_3DSTATE_COARSE_PIXEL_length      2
#define GFX30_3DSTATE_COARSE_PIXEL_length_bias      2
#define GFX30_3DSTATE_COARSE_PIXEL_header       \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    137,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_COARSE_PIXEL {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   enum GFX30_STATE_CPS_SIZE            CPSizeX;
   enum GFX30_STATE_CPS_SIZE            CPSizeY;
   enum GFX30_STATE_CPS_COMBINER_OPCODE CPSizeCombiner1Opcode;
   enum GFX30_STATE_CPS_COMBINER_OPCODE CPSizeCombiner0Opcode;
   bool                                 DisableCPSPointers;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_COARSE_PIXEL_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_COARSE_PIXEL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->CPSizeX, 0, 1) |
      util_bitpack_uint(values->CPSizeY, 2, 3) |
      util_bitpack_uint(values->CPSizeCombiner1Opcode, 4, 6) |
      util_bitpack_uint(values->CPSizeCombiner0Opcode, 7, 9) |
      util_bitpack_uint(values->DisableCPSPointers, 31, 31);
}

#define GFX30_3DSTATE_CONSTANT_ALL_length      2
#define GFX30_3DSTATE_CONSTANT_ALL_length_bias      2
#define GFX30_3DSTATE_CONSTANT_ALL_header       \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    109,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_ALL {
   uint32_t                             DWordLength;
   uint32_t                             ShaderUpdateEnable;
   bool                                 VertexShaderUpdateEnable;
   bool                                 HullShaderUpdateEnable;
   bool                                 DomainShaderUpdateEnable;
   bool                                 GeometryShaderUpdateEnable;
   bool                                 PixelShaderUpdateEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MOCS;
   uint32_t                             PointerBufferMask;
   bool                                 UpdateMode;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_ALL_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_ALL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->ShaderUpdateEnable, 8, 12) |
      util_bitpack_uint(values->VertexShaderUpdateEnable, 8, 8) |
      util_bitpack_uint(values->HullShaderUpdateEnable, 9, 9) |
      util_bitpack_uint(values->DomainShaderUpdateEnable, 10, 10) |
      util_bitpack_uint(values->GeometryShaderUpdateEnable, 11, 11) |
      util_bitpack_uint(values->PixelShaderUpdateEnable, 12, 12) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->PointerBufferMask, 16, 19) |
      util_bitpack_uint(values->UpdateMode, 31, 31);
}

#define GFX30_3DSTATE_CONSTANT_DS_length      11
#define GFX30_3DSTATE_CONSTANT_DS_length_bias      2
#define GFX30_3DSTATE_CONSTANT_DS_header        \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     26,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_DS {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_CONSTANT_BODY   ConstantBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_DS_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_CONSTANT_BODY_pack(data, &dw[1], &values->ConstantBody);
}

#define GFX30_3DSTATE_CONSTANT_GS_length      11
#define GFX30_3DSTATE_CONSTANT_GS_length_bias      2
#define GFX30_3DSTATE_CONSTANT_GS_header        \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     22,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_GS {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_CONSTANT_BODY   ConstantBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_GS_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_CONSTANT_BODY_pack(data, &dw[1], &values->ConstantBody);
}

#define GFX30_3DSTATE_CONSTANT_HS_length      11
#define GFX30_3DSTATE_CONSTANT_HS_length_bias      2
#define GFX30_3DSTATE_CONSTANT_HS_header        \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     25,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_HS {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_CONSTANT_BODY   ConstantBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_HS_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_CONSTANT_BODY_pack(data, &dw[1], &values->ConstantBody);
}

#define GFX30_3DSTATE_CONSTANT_PS_length      11
#define GFX30_3DSTATE_CONSTANT_PS_length_bias      2
#define GFX30_3DSTATE_CONSTANT_PS_header        \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     23,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_PS {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   uint32_t                             DisableGatheratSetShaderHint;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_CONSTANT_BODY   ConstantBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_PS_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->DisableGatheratSetShaderHint, 15, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_CONSTANT_BODY_pack(data, &dw[1], &values->ConstantBody);
}

#define GFX30_3DSTATE_CONSTANT_VS_length      11
#define GFX30_3DSTATE_CONSTANT_VS_length_bias      2
#define GFX30_3DSTATE_CONSTANT_VS_header        \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     21,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CONSTANT_VS {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_CONSTANT_BODY   ConstantBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CONSTANT_VS_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_CONSTANT_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_CONSTANT_BODY_pack(data, &dw[1], &values->ConstantBody);
}

#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_length      8
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_length_bias      2
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_header\
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =    131,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SurfacePitch;
   uint32_t                             SurfaceType;
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             Width;
   uint32_t                             Height;
   uint32_t                             MOCS;
   uint32_t                             MinimumArrayElement;
   uint32_t                             Depth;
   enum GFX30_UNIFIED_COMPRESSION_FORMAT CompressionFormat;
   uint32_t                             MipTailStartLOD;
   uint32_t                             TiledMode;
#define TILE64                                   1
#define TILE4                                    3
   uint32_t                             SurfaceQPitch;
   uint32_t                             SurfLOD;
   uint32_t                             RenderTargetViewExtent;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SurfacePitch, 0, 16) |
      util_bitpack_uint(values->SurfaceType, 29, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SurfaceBaseAddress, 0, 0, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->Width, 1, 14) |
      util_bitpack_uint(values->Height, 17, 30);

   dw[5] =
      util_bitpack_uint(values->MOCS, 0, 6) |
      util_bitpack_uint(values->MinimumArrayElement, 8, 18) |
      util_bitpack_uint(values->Depth, 20, 30);

   dw[6] =
      util_bitpack_uint(values->CompressionFormat, 0, 3) |
      util_bitpack_uint(values->MipTailStartLOD, 26, 29) |
      util_bitpack_uint(values->TiledMode, 30, 31);

   dw[7] =
      util_bitpack_uint(values->SurfaceQPitch, 0, 14) |
      util_bitpack_uint(values->SurfLOD, 16, 19) |
      util_bitpack_uint(values->RenderTargetViewExtent, 21, 31);
}

#define GFX30_3DSTATE_DEPTH_BOUNDS_length      4
#define GFX30_3DSTATE_DEPTH_BOUNDS_length_bias      2
#define GFX30_3DSTATE_DEPTH_BOUNDS_header       \
   .Length                              =      2,  \
   ._3DSTATECommandSubOpcode            =    113,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_DEPTH_BOUNDS {
   uint32_t                             Length;
   bool                                 DepthBoundsTestValueModifyDisable;
   bool                                 DepthBoundsTestEnableModifyDisable;
   uint32_t                             _3DSTATECommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             DepthBoundsTestEnable;
   float                                DepthBoundsTestMinValue;
   float                                DepthBoundsTestMaxValue;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_DEPTH_BOUNDS_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_DEPTH_BOUNDS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->Length, 0, 7) |
      util_bitpack_uint(values->DepthBoundsTestValueModifyDisable, 14, 14) |
      util_bitpack_uint(values->DepthBoundsTestEnableModifyDisable, 15, 15) |
      util_bitpack_uint(values->_3DSTATECommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DepthBoundsTestEnable, 0, 0);

   dw[2] =
      util_bitpack_float(values->DepthBoundsTestMinValue);

   dw[3] =
      util_bitpack_float(values->DepthBoundsTestMaxValue);
}

#define GFX30_3DSTATE_DEPTH_BUFFER_length      8
#define GFX30_3DSTATE_DEPTH_BUFFER_length_bias      2
#define GFX30_3DSTATE_DEPTH_BUFFER_header       \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =      5,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_DEPTH_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SurfacePitch;
   bool                                 HierarchicalDepthBufferEnable;
   bool                                 CornerTexelMode;
   uint32_t                             SurfaceFormat;
#define D32_FLOAT                                1
#define D24_UNORM_X8_UINT                        3
#define D16_UNORM                                5
   bool                                 NullPageCoherencyEnable;
   bool                                 DepthWriteEnable;
   uint32_t                             SurfaceType;
#define SURFTYPE_2D                              1
#define SURFTYPE_CUBE                            3
#define SURFTYPE_NULL                            7
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             Width;
   uint32_t                             Height;
   uint32_t                             MOCS;
   uint32_t                             MinimumArrayElement;
   uint32_t                             Depth;
   enum GFX30_UNIFIED_COMPRESSION_FORMAT RenderCompressionFormat;
   uint32_t                             MipTailStartLOD;
   uint32_t                             TiledMode;
#define TILE64                                   1
#define TILE4                                    3
   uint32_t                             SurfaceQPitch;
   uint32_t                             LOD;
   uint32_t                             RenderTargetViewExtent;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_DEPTH_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_DEPTH_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SurfacePitch, 0, 17) |
      util_bitpack_uint(values->HierarchicalDepthBufferEnable, 22, 22) |
      util_bitpack_uint(values->CornerTexelMode, 23, 23) |
      util_bitpack_uint(values->SurfaceFormat, 24, 26) |
      util_bitpack_uint(values->NullPageCoherencyEnable, 27, 27) |
      util_bitpack_uint(values->DepthWriteEnable, 28, 28) |
      util_bitpack_uint(values->SurfaceType, 29, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SurfaceBaseAddress, 0, 0, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->Width, 1, 14) |
      util_bitpack_uint(values->Height, 17, 30);

   dw[5] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->MinimumArrayElement, 8, 18) |
      util_bitpack_uint(values->Depth, 20, 30);

   dw[6] =
      util_bitpack_uint(values->RenderCompressionFormat, 0, 3) |
      util_bitpack_uint(values->MipTailStartLOD, 26, 29) |
      util_bitpack_uint(values->TiledMode, 30, 31);

   dw[7] =
      util_bitpack_uint(values->SurfaceQPitch, 0, 14) |
      util_bitpack_uint(values->LOD, 16, 19) |
      util_bitpack_uint(values->RenderTargetViewExtent, 21, 31);
}

#define GFX30_3DSTATE_DRAWING_RECTANGLE_length      4
#define GFX30_3DSTATE_DRAWING_RECTANGLE_length_bias      2
#define GFX30_3DSTATE_DRAWING_RECTANGLE_header  \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_DRAWING_RECTANGLE {
   uint32_t                             DWordLength;
   uint32_t                             CoreModeSelect;
#define Legacy                                   0
#define Core0Enabled                             1
#define Core1Enabled                             2
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ClippedDrawingRectangleXMin;
   uint32_t                             ClippedDrawingRectangleYMin;
   uint32_t                             ClippedDrawingRectangleXMax;
   uint32_t                             ClippedDrawingRectangleYMax;
   int32_t                              DrawingRectangleOriginX;
   int32_t                              DrawingRectangleOriginY;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_DRAWING_RECTANGLE_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_DRAWING_RECTANGLE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->CoreModeSelect, 14, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ClippedDrawingRectangleXMin, 0, 15) |
      util_bitpack_uint(values->ClippedDrawingRectangleYMin, 16, 31);

   dw[2] =
      util_bitpack_uint(values->ClippedDrawingRectangleXMax, 0, 15) |
      util_bitpack_uint(values->ClippedDrawingRectangleYMax, 16, 31);

   dw[3] =
      util_bitpack_sint(values->DrawingRectangleOriginX, 0, 15) |
      util_bitpack_sint(values->DrawingRectangleOriginY, 16, 31);
}

#define GFX30_3DSTATE_DRAWING_RECTANGLE_FAST_length      4
#define GFX30_3DSTATE_DRAWING_RECTANGLE_FAST_length_bias      2
#define GFX30_3DSTATE_DRAWING_RECTANGLE_FAST_header\
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_DRAWING_RECTANGLE_FAST {
   uint32_t                             DWordLength;
   uint32_t                             CoreModeSelect;
#define Legacy                                   0
#define Core0Enabled                             1
#define Core1Enabled                             2
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ClippedDrawingRectangleXMin;
   uint32_t                             ClippedDrawingRectangleYMin;
   uint32_t                             ClippedDrawingRectangleXMax;
   uint32_t                             ClippedDrawingRectangleYMax;
   int32_t                              DrawingRectangleOriginX;
   int32_t                              DrawingRectangleOriginY;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_DRAWING_RECTANGLE_FAST_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_DRAWING_RECTANGLE_FAST * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->CoreModeSelect, 14, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ClippedDrawingRectangleXMin, 0, 15) |
      util_bitpack_uint(values->ClippedDrawingRectangleYMin, 16, 31);

   dw[2] =
      util_bitpack_uint(values->ClippedDrawingRectangleXMax, 0, 15) |
      util_bitpack_uint(values->ClippedDrawingRectangleYMax, 16, 31);

   dw[3] =
      util_bitpack_sint(values->DrawingRectangleOriginX, 0, 15) |
      util_bitpack_sint(values->DrawingRectangleOriginY, 16, 31);
}

#define GFX30_3DSTATE_DS_length               11
#define GFX30_3DSTATE_DS_length_bias           2
#define GFX30_3DSTATE_DS_header                 \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     29,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             KernelStartPointer;
   uint32_t                             RegistersPerThread;
   bool                                 SoftwareExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   bool                                 AccessesUAV;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             ThreadDispatchPriority;
#define High                                     1
   uint32_t                             BindingTableEntryCount;
   uint32_t                             SamplerCount;
#define NoSamplers                               0
#define _14Samplers                              1
#define _58Samplers                              2
#define _912Samplers                             3
#define _1316Samplers                            4
   bool                                 VectorMaskEnable;
   uint32_t                             ScratchSpaceBuffer;
   uint32_t                             PatchURBEntryReadOffset;
   uint32_t                             PatchURBEntryReadLength;
   uint32_t                             DispatchGRFStartRegisterForURBData;
   bool                                 Enable;
   bool                                 CacheDisable;
   bool                                 ComputeWCoordinateEnable;
   uint32_t                             DispatchMode;
#define DISPATCH_MODE_SIMD8_SINGLE_PATCH         1
#define DISPATCH_MODE_SIMD8_SINGLE_OR_DUAL_PATCH 2
   bool                                 PrimitiveIDNotRequired;
   bool                                 StatisticsEnable;
   uint32_t                             MaximumNumberofThreads;
   uint32_t                             UserClipDistanceCullTestEnableBitmask;
   uint32_t                             UserClipDistanceClipTestEnableBitmask;
   uint32_t                             VertexURBEntryOutputLength;
   uint32_t                             VertexURBEntryOutputReadOffset;
   uint64_t                             QUAD_PATCHKernelStartPointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_DS_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->RegistersPerThread, 0, 5) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 7, 7) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->AccessesUAV, 14, 14) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->ThreadDispatchPriority, 17, 17) |
      util_bitpack_uint(values->BindingTableEntryCount, 18, 25) |
      util_bitpack_uint(values->SamplerCount, 27, 29) |
      util_bitpack_uint(values->VectorMaskEnable, 30, 30);

   dw[4] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[5] = 0;

   dw[6] =
      util_bitpack_uint(values->PatchURBEntryReadOffset, 4, 9) |
      util_bitpack_uint(values->PatchURBEntryReadLength, 11, 17) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData, 20, 24);

   dw[7] =
      util_bitpack_uint(values->Enable, 0, 0) |
      util_bitpack_uint(values->CacheDisable, 1, 1) |
      util_bitpack_uint(values->ComputeWCoordinateEnable, 2, 2) |
      util_bitpack_uint(values->DispatchMode, 3, 4) |
      util_bitpack_uint(values->PrimitiveIDNotRequired, 9, 9) |
      util_bitpack_uint(values->StatisticsEnable, 10, 10) |
      util_bitpack_uint(values->MaximumNumberofThreads, 21, 30);

   dw[8] =
      util_bitpack_uint(values->UserClipDistanceCullTestEnableBitmask, 0, 7) |
      util_bitpack_uint(values->UserClipDistanceClipTestEnableBitmask, 8, 15) |
      util_bitpack_uint(values->VertexURBEntryOutputLength, 16, 20) |
      util_bitpack_uint(values->VertexURBEntryOutputReadOffset, 21, 26);

   const uint64_t v9 =
      __gen_offset(values->QUAD_PATCHKernelStartPointer, 6, 63);
   dw[9] = v9;
   dw[10] = v9 >> 32;
}

#define GFX30_3DSTATE_GATHER_CONSTANT_DS_length_bias      2
#define GFX30_3DSTATE_GATHER_CONSTANT_DS_header \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     55,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_CONSTANT_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             UpdateGatherTableOnly;
#define CommitGather                             0
#define NonCommitGather                          1
   uint32_t                             ConstantBufferBindingTableBlock;
   uint32_t                             ConstantBufferValid;
   uint32_t                             OnDieTable;
#define Load                                     0
#define Read                                     1
   bool                                 ConstantBufferDx9GenerateStall;
   uint64_t                             GatherBufferOffset;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_CONSTANT_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_CONSTANT_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->UpdateGatherTableOnly, 1, 1) |
      util_bitpack_uint(values->ConstantBufferBindingTableBlock, 12, 15) |
      util_bitpack_uint(values->ConstantBufferValid, 16, 31);

   dw[2] =
      util_bitpack_uint(values->OnDieTable, 3, 3) |
      util_bitpack_uint(values->ConstantBufferDx9GenerateStall, 5, 5) |
      __gen_offset(values->GatherBufferOffset, 6, 22);
}

#define GFX30_3DSTATE_GATHER_CONSTANT_GS_length_bias      2
#define GFX30_3DSTATE_GATHER_CONSTANT_GS_header \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     53,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_CONSTANT_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             UpdateGatherTableOnly;
#define CommitGather                             0
#define NonCommitGather                          1
   uint32_t                             ConstantBufferBindingTableBlock;
   uint32_t                             ConstantBufferValid;
   uint32_t                             OnDieTable;
#define Load                                     0
#define Read                                     1
   bool                                 ConstantBufferDx9GenerateStall;
   uint64_t                             GatherBufferOffset;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_CONSTANT_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_CONSTANT_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->UpdateGatherTableOnly, 1, 1) |
      util_bitpack_uint(values->ConstantBufferBindingTableBlock, 12, 15) |
      util_bitpack_uint(values->ConstantBufferValid, 16, 31);

   dw[2] =
      util_bitpack_uint(values->OnDieTable, 3, 3) |
      util_bitpack_uint(values->ConstantBufferDx9GenerateStall, 5, 5) |
      __gen_offset(values->GatherBufferOffset, 6, 22);
}

#define GFX30_3DSTATE_GATHER_CONSTANT_HS_length_bias      2
#define GFX30_3DSTATE_GATHER_CONSTANT_HS_header \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     54,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_CONSTANT_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             UpdateGatherTableOnly;
#define CommitGather                             0
#define NonCommitGather                          1
   uint32_t                             ConstantBufferBindingTableBlock;
   uint32_t                             ConstantBufferValid;
   uint32_t                             OnDieTable;
#define Load                                     0
#define Read                                     1
   bool                                 ConstantBufferDx9GenerateStall;
   uint64_t                             GatherBufferOffset;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_CONSTANT_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_CONSTANT_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->UpdateGatherTableOnly, 1, 1) |
      util_bitpack_uint(values->ConstantBufferBindingTableBlock, 12, 15) |
      util_bitpack_uint(values->ConstantBufferValid, 16, 31);

   dw[2] =
      util_bitpack_uint(values->OnDieTable, 3, 3) |
      util_bitpack_uint(values->ConstantBufferDx9GenerateStall, 5, 5) |
      __gen_offset(values->GatherBufferOffset, 6, 22);
}

#define GFX30_3DSTATE_GATHER_CONSTANT_PS_length_bias      2
#define GFX30_3DSTATE_GATHER_CONSTANT_PS_header \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     56,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_CONSTANT_PS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 DX9OnDieRegisterReadEnable;
   uint32_t                             UpdateGatherTableOnly;
#define CommitGather                             0
#define NonCommitGather                          1
   uint32_t                             ConstantBufferBindingTableBlock;
   uint32_t                             ConstantBufferValid;
   uint32_t                             OnDieTable;
#define Load                                     0
#define Read                                     1
   bool                                 ConstantBufferDx9Enable;
   bool                                 ConstantBufferDx9GenerateStall;
   uint64_t                             GatherBufferOffset;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_CONSTANT_PS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_CONSTANT_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DX9OnDieRegisterReadEnable, 0, 0) |
      util_bitpack_uint(values->UpdateGatherTableOnly, 1, 1) |
      util_bitpack_uint(values->ConstantBufferBindingTableBlock, 12, 15) |
      util_bitpack_uint(values->ConstantBufferValid, 16, 31);

   dw[2] =
      util_bitpack_uint(values->OnDieTable, 3, 3) |
      util_bitpack_uint(values->ConstantBufferDx9Enable, 4, 4) |
      util_bitpack_uint(values->ConstantBufferDx9GenerateStall, 5, 5) |
      __gen_offset(values->GatherBufferOffset, 6, 22);
}

#define GFX30_3DSTATE_GATHER_CONSTANT_VS_length_bias      2
#define GFX30_3DSTATE_GATHER_CONSTANT_VS_header \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     52,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_CONSTANT_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 DX9OnDieRegisterReadEnable;
   uint32_t                             UpdateGatherTableOnly;
#define CommitGather                             0
#define NonCommitGather                          1
   uint32_t                             ConstantBufferBindingTableBlock;
   uint32_t                             ConstantBufferValid;
   uint32_t                             OnDieTable;
#define Load                                     0
#define Read                                     1
   bool                                 ConstantBufferDx9Enable;
   bool                                 ConstantBufferDx9GenerateStall;
   uint64_t                             GatherBufferOffset;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_CONSTANT_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_CONSTANT_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DX9OnDieRegisterReadEnable, 0, 0) |
      util_bitpack_uint(values->UpdateGatherTableOnly, 1, 1) |
      util_bitpack_uint(values->ConstantBufferBindingTableBlock, 12, 15) |
      util_bitpack_uint(values->ConstantBufferValid, 16, 31);

   dw[2] =
      util_bitpack_uint(values->OnDieTable, 3, 3) |
      util_bitpack_uint(values->ConstantBufferDx9Enable, 4, 4) |
      util_bitpack_uint(values->ConstantBufferDx9GenerateStall, 5, 5) |
      __gen_offset(values->GatherBufferOffset, 6, 22);
}

#define GFX30_3DSTATE_GATHER_POOL_ALLOC_length      4
#define GFX30_3DSTATE_GATHER_POOL_ALLOC_length_bias      2
#define GFX30_3DSTATE_GATHER_POOL_ALLOC_header  \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     26,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GATHER_POOL_ALLOC {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MOCS;
   bool                                 GatherPoolEnable;
   __gen_address_type                   GatherPoolBaseAddress;
   uint32_t                             GatherPoolBufferSize;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GATHER_POOL_ALLOC_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_GATHER_POOL_ALLOC * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->GatherPoolEnable, 11, 11);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->GatherPoolBaseAddress, v1, 12, 63);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   dw[3] =
      util_bitpack_uint(values->GatherPoolBufferSize, 12, 31);
}

#define GFX30_3DSTATE_GS_length               10
#define GFX30_3DSTATE_GS_length_bias           2
#define GFX30_3DSTATE_GS_header                 \
   .DWordLength                         =      8,  \
   ._3DCommandSubOpcode                 =     17,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             KernelStartPointer;
   uint32_t                             ExpectedVertexCount;
   bool                                 SoftwareExceptionEnable;
   bool                                 MaskStackExceptionEnable;
   bool                                 AccessesUAV;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             ThreadDispatchPriority;
#define Normal                                   0
#define High                                     1
   uint32_t                             BindingTableEntryCount;
   uint32_t                             SamplerCount;
#define NoSamplers                               0
#define _14Samplers                              1
#define _58Samplers                              2
#define _912Samplers                             3
#define _1316Samplers                            4
   uint32_t                             VectorMaskEnable;
#define Dmask                                    0
   bool                                 SingleProgramFlow;
   uint32_t                             ScratchSpaceBuffer;
   uint32_t                             DispatchGRFStartRegisterForURBData;
   uint32_t                             VertexURBEntryReadOffset;
   bool                                 IncludeVertexHandles;
   uint32_t                             VertexURBEntryReadLength;
   enum GFX30_3D_Prim_Topo_Type         OutputTopology;
   uint32_t                             OutputVertexSize;
   uint32_t                             DispatchGRFStartRegisterForURBData54;
   bool                                 Enable;
   bool                                 DiscardAdjacency;
   uint32_t                             ReorderMode;
#define LEADING                                  0
#define TRAILING                                 1
   bool                                 Hint;
   bool                                 IncludePrimitiveID;
   uint32_t                             InvocationsIncrementValue;
   bool                                 StatisticsEnable;
   uint32_t                             DefaultStreamId;
   uint32_t                             InstanceControl;
   uint32_t                             ControlDataHeaderSize;
   uint32_t                             RegistersPerThread;
   uint32_t                             MaximumNumberofThreads;
   uint32_t                             StaticOutputVertexCount;
   bool                                 StaticOutput;
   uint32_t                             ControlDataFormat;
#define CUT                                      0
#define SID                                      1
   uint32_t                             UserClipDistanceCullTestEnableBitmask;
   uint32_t                             UserClipDistanceClipTestEnableBitmask;
   uint32_t                             VertexURBEntryOutputLength;
   uint32_t                             VertexURBEntryOutputReadOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_GS_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->ExpectedVertexCount, 0, 5) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 7, 7) |
      util_bitpack_uint(values->MaskStackExceptionEnable, 11, 11) |
      util_bitpack_uint(values->AccessesUAV, 12, 12) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->ThreadDispatchPriority, 17, 17) |
      util_bitpack_uint(values->BindingTableEntryCount, 18, 25) |
      util_bitpack_uint(values->SamplerCount, 27, 29) |
      util_bitpack_uint(values->VectorMaskEnable, 30, 30) |
      util_bitpack_uint(values->SingleProgramFlow, 31, 31);

   dw[4] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[5] = 0;

   dw[6] =
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData, 0, 3) |
      util_bitpack_uint(values->VertexURBEntryReadOffset, 4, 9) |
      util_bitpack_uint(values->IncludeVertexHandles, 10, 10) |
      util_bitpack_uint(values->VertexURBEntryReadLength, 11, 16) |
      util_bitpack_uint(values->OutputTopology, 17, 22) |
      util_bitpack_uint(values->OutputVertexSize, 23, 28) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData54, 29, 30);

   dw[7] =
      util_bitpack_uint(values->Enable, 0, 0) |
      util_bitpack_uint(values->DiscardAdjacency, 1, 1) |
      util_bitpack_uint(values->ReorderMode, 2, 2) |
      util_bitpack_uint(values->Hint, 3, 3) |
      util_bitpack_uint(values->IncludePrimitiveID, 4, 4) |
      util_bitpack_uint(values->InvocationsIncrementValue, 5, 9) |
      util_bitpack_uint(values->StatisticsEnable, 10, 10) |
      util_bitpack_uint(values->DefaultStreamId, 13, 14) |
      util_bitpack_uint(values->InstanceControl, 15, 19) |
      util_bitpack_uint(values->ControlDataHeaderSize, 20, 23) |
      util_bitpack_uint(values->RegistersPerThread, 26, 31);

   dw[8] =
      util_bitpack_uint(values->MaximumNumberofThreads, 0, 8) |
      util_bitpack_uint(values->StaticOutputVertexCount, 16, 26) |
      util_bitpack_uint(values->StaticOutput, 30, 30) |
      util_bitpack_uint(values->ControlDataFormat, 31, 31);

   dw[9] =
      util_bitpack_uint(values->UserClipDistanceCullTestEnableBitmask, 0, 7) |
      util_bitpack_uint(values->UserClipDistanceClipTestEnableBitmask, 8, 15) |
      util_bitpack_uint(values->VertexURBEntryOutputLength, 16, 20) |
      util_bitpack_uint(values->VertexURBEntryOutputReadOffset, 21, 26);
}

#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_length      5
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_length_bias      2
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_header  \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =      7,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_HIER_DEPTH_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SurfacePitch;
   bool                                 HierarchicalDepthBufferWriteThruEnable;
   uint32_t                             TiledMode;
#define TILE64                                   1
#define TILE4                                    3
   uint32_t                             MOCS;
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             SurfaceQPitch;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_HIER_DEPTH_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_HIER_DEPTH_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SurfacePitch, 0, 16) |
      util_bitpack_uint(values->HierarchicalDepthBufferWriteThruEnable, 20, 20) |
      util_bitpack_uint(values->TiledMode, 22, 23) |
      util_bitpack_uint_nonzero(values->MOCS, 25, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SurfaceBaseAddress, 0, 0, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->SurfaceQPitch, 0, 14);
}

#define GFX30_3DSTATE_HS_length                8
#define GFX30_3DSTATE_HS_length_bias           2
#define GFX30_3DSTATE_HS_header                 \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     27,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             PatchCountThreshold;
   uint32_t                             RegistersPerThread;
   bool                                 SoftwareExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define alternate                                1
   uint32_t                             ThreadDispatchPriority;
#define Normal                                   0
#define High                                     1
   uint32_t                             BindingTableEntryCount;
   uint32_t                             SamplerCount;
#define NoSamplers                               0
#define _14Samplers                              1
#define _58Samplers                              2
#define _912Samplers                             3
#define _1316Samplers                            4
   uint32_t                             InstanceCount;
   uint32_t                             MaximumNumberofThreads;
   bool                                 StatisticsEnable;
   bool                                 Enable;
   uint64_t                             KernelStartPointer;
   uint32_t                             ScratchSpaceBuffer;
   bool                                 IncludePrimitiveID;
   uint32_t                             VertexURBEntryReadOffset;
   uint32_t                             VertexURBEntryReadLength;
   uint32_t                             DispatchGRFStartRegisterForURBData;
   bool                                 IncludeVertexHandles;
   bool                                 AccessesUAV;
   uint32_t                             VectorMaskEnable;
#define Dmask                                    0
   bool                                 SingleProgramFlow;
   uint32_t                             DispatchGRFStartRegisterForURBData5;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_HS_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PatchCountThreshold, 0, 3) |
      util_bitpack_uint(values->RegistersPerThread, 4, 9) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 12, 12) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->ThreadDispatchPriority, 17, 17) |
      util_bitpack_uint(values->BindingTableEntryCount, 18, 25) |
      util_bitpack_uint(values->SamplerCount, 27, 29);

   dw[2] =
      util_bitpack_uint(values->InstanceCount, 0, 4) |
      util_bitpack_uint(values->MaximumNumberofThreads, 8, 16) |
      util_bitpack_uint(values->StatisticsEnable, 29, 29) |
      util_bitpack_uint(values->Enable, 31, 31);

   const uint64_t v3 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[3] = v3;
   dw[4] = v3 >> 32;

   dw[5] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[6] = 0;

   dw[7] =
      util_bitpack_uint(values->IncludePrimitiveID, 0, 0) |
      util_bitpack_uint(values->VertexURBEntryReadOffset, 4, 9) |
      util_bitpack_uint(values->VertexURBEntryReadLength, 11, 16) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData, 19, 23) |
      util_bitpack_uint(values->IncludeVertexHandles, 24, 24) |
      util_bitpack_uint(values->AccessesUAV, 25, 25) |
      util_bitpack_uint(values->VectorMaskEnable, 26, 26) |
      util_bitpack_uint(values->SingleProgramFlow, 27, 27) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData5, 28, 28);
}

#define GFX30_3DSTATE_INDEX_BUFFER_length      5
#define GFX30_3DSTATE_INDEX_BUFFER_length_bias      2
#define GFX30_3DSTATE_INDEX_BUFFER_header       \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     10,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_INDEX_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MOCS;
   uint32_t                             IndexFormat;
#define INDEX_BYTE                               0
#define INDEX_WORD                               1
#define INDEX_DWORD                              2
   bool                                 L3BypassDisable;
   __gen_address_type                   BufferStartingAddress;
   uint32_t                             BufferSize;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_INDEX_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_INDEX_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->IndexFormat, 8, 9) |
      util_bitpack_uint(values->L3BypassDisable, 11, 11);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->BufferStartingAddress, 0, 0, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->BufferSize, 0, 31);
}

#define GFX30_3DSTATE_LINE_STIPPLE_length      3
#define GFX30_3DSTATE_LINE_STIPPLE_length_bias      2
#define GFX30_3DSTATE_LINE_STIPPLE_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =      8,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_LINE_STIPPLE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             LineStipplePattern;
   uint32_t                             CurrentStippleIndex;
   uint32_t                             CurrentRepeatCounter;
   bool                                 ModifyEnableCurrentRepeatCounterCurrentStippleIndex;
   uint32_t                             LineStippleRepeatCount;
   float                                LineStippleInverseRepeatCount;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_LINE_STIPPLE_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_LINE_STIPPLE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->LineStipplePattern, 0, 15) |
      util_bitpack_uint(values->CurrentStippleIndex, 16, 19) |
      util_bitpack_uint(values->CurrentRepeatCounter, 21, 29) |
      util_bitpack_uint(values->ModifyEnableCurrentRepeatCounterCurrentStippleIndex, 31, 31);

   dw[2] =
      util_bitpack_uint(values->LineStippleRepeatCount, 0, 8) |
      util_bitpack_ufixed(values->LineStippleInverseRepeatCount, 15, 31, 16);
}

#define GFX30_3DSTATE_MESH_CONTROL_length      3
#define GFX30_3DSTATE_MESH_CONTROL_length_bias      2
#define GFX30_3DSTATE_MESH_CONTROL_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =    119,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_MESH_CONTROL {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MaximumNumberofThreadGroups;
   bool                                 VPandRTAIndexAutostripEnable;
   uint32_t                             ThreadDispatchPriority;
#define High                                     1
   bool                                 FusedEUDispatch;
   bool                                 StatisticsEnable;
   uint32_t                             MeshShaderEnable;
   uint32_t                             ScratchSpaceBuffer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MESH_CONTROL_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_MESH_CONTROL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MaximumNumberofThreadGroups, 0, 8) |
      util_bitpack_uint(values->VPandRTAIndexAutostripEnable, 27, 27) |
      util_bitpack_uint(values->ThreadDispatchPriority, 28, 28) |
      util_bitpack_uint(values->FusedEUDispatch, 29, 29) |
      util_bitpack_uint(values->StatisticsEnable, 30, 30) |
      util_bitpack_uint(values->MeshShaderEnable, 31, 31);

   dw[2] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);
}

#define GFX30_3DSTATE_MESH_DISTRIB_length      2
#define GFX30_3DSTATE_MESH_DISTRIB_length_bias      2
#define GFX30_3DSTATE_MESH_DISTRIB_header       \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    120,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_MESH_DISTRIB {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 MeshDistributionDisable;
   bool                                 TaskDistributionDisable;
   uint32_t                             DistributionMode;
#define MESH_RR_FREE                             1
#define MESH_RR_STRICT                           0
   uint32_t                             MeshDistributionBatchSize;
   uint32_t                             TaskDistributionBatchSize;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MESH_DISTRIB_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_MESH_DISTRIB * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MeshDistributionDisable, 0, 0) |
      util_bitpack_uint(values->TaskDistributionDisable, 1, 1) |
      util_bitpack_uint(values->DistributionMode, 2, 2) |
      util_bitpack_uint(values->MeshDistributionBatchSize, 4, 7) |
      util_bitpack_uint(values->TaskDistributionBatchSize, 10, 13);
}

#define GFX30_3DSTATE_MESH_SHADER_length       8
#define GFX30_3DSTATE_MESH_SHADER_length_bias      2
#define GFX30_3DSTATE_MESH_SHADER_header        \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =    122,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_MESH_SHADER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             KernelStartPointer;
   uint32_t                             LocalXMaximum;
   bool                                 SoftwareExceptionEnable;
   bool                                 MaskStackExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             SingleProgramFlow;
   uint32_t                             DenormMode;
#define Ftz                                      0
#define SetByKernel                              1
   uint32_t                             RegistersPerThread;
   uint32_t                             NumberofThreadsinGPGPUThreadGroup;
   uint32_t                             SharedLocalMemorySize;
   uint32_t                             RoundingMode;
#define RTNE                                     0
#define RU                                       1
#define RD                                       2
#define RTZ                                      3
   enum GFX30_PREF_SLM_ALLOCATION_SIZE  PreferredSLMAllocationSize;
   uint32_t                             NumberofBarriers;
   uint32_t                             IndirectDataLength;
   bool                                 L3PrefetchDisable;
   bool                                 EmitLocalIDX;
   bool                                 EmitInlineParameter;
   bool                                 SystolicModeEnable;
   bool                                 AccessesUAV;
   uint32_t                             XP0Required;
   uint32_t                             MessageSIMD;
#define SIMD16                                   1
#define SIMD32                                   2
   uint32_t                             SIMDSize;
#define SIMD16                                   1
#define SIMD32                                   2
   uint32_t                             ExecutionMask;
   uint32_t                             MaximumPrimitiveCount;
   uint32_t                             PerPrimitiveDataPitch;
   uint32_t                             PerVertexDataPitch;
   uint32_t                             IndexFormat;
#define INDEX_U888X                              0
#define INDEX_U101010X                           1
#define INDEX_U8                                 4
#define INDEX_U16                                5
#define INDEX_U32                                3
   uint32_t                             OutputTopology;
#define OUTPUT_POINT                             0
#define OUTPUT_LINE                              1
#define OUTPUT_TRI                               2
   uint32_t                             PerPrimitiveDataPresent;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MESH_SHADER_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_MESH_SHADER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->LocalXMaximum, 0, 9) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 10, 10) |
      util_bitpack_uint(values->MaskStackExceptionEnable, 11, 11) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->SingleProgramFlow, 18, 18) |
      util_bitpack_uint(values->DenormMode, 19, 19) |
      util_bitpack_uint(values->RegistersPerThread, 20, 25);

   dw[4] =
      util_bitpack_uint(values->NumberofThreadsinGPGPUThreadGroup, 0, 9) |
      util_bitpack_uint(values->SharedLocalMemorySize, 16, 20) |
      util_bitpack_uint(values->RoundingMode, 22, 23) |
      util_bitpack_uint(values->PreferredSLMAllocationSize, 24, 27) |
      util_bitpack_uint(values->NumberofBarriers, 28, 30);

   dw[5] =
      util_bitpack_uint(values->IndirectDataLength, 0, 16) |
      util_bitpack_uint(values->L3PrefetchDisable, 17, 17) |
      util_bitpack_uint(values->EmitLocalIDX, 18, 18) |
      util_bitpack_uint(values->EmitInlineParameter, 19, 19) |
      util_bitpack_uint(values->SystolicModeEnable, 20, 20) |
      util_bitpack_uint(values->AccessesUAV, 21, 21) |
      util_bitpack_uint(values->XP0Required, 22, 22) |
      util_bitpack_uint(values->MessageSIMD, 28, 29) |
      util_bitpack_uint(values->SIMDSize, 30, 31);

   dw[6] =
      util_bitpack_uint(values->ExecutionMask, 0, 31);

   dw[7] =
      util_bitpack_uint(values->MaximumPrimitiveCount, 0, 9) |
      util_bitpack_uint(values->PerPrimitiveDataPitch, 12, 16) |
      util_bitpack_uint(values->PerVertexDataPitch, 20, 24) |
      util_bitpack_uint(values->IndexFormat, 25, 27) |
      util_bitpack_uint(values->OutputTopology, 28, 29) |
      util_bitpack_uint(values->PerPrimitiveDataPresent, 31, 31);
}

#define GFX30_3DSTATE_MESH_SHADER_DATA_length     10
#define GFX30_3DSTATE_MESH_SHADER_DATA_length_bias      2
#define GFX30_3DSTATE_MESH_SHADER_DATA_header   \
   .DWordLength                         =      8,  \
   ._3DCommandSubOpcode                 =    123,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_MESH_SHADER_DATA {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             IndirectDataStartAddress;
   uint32_t                             InlineData[8];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MESH_SHADER_DATA_pack(__attribute__((unused)) __gen_user_data *data,
                                    __attribute__((unused)) void * restrict dst,
                                    __attribute__((unused)) const struct GFX30_3DSTATE_MESH_SHADER_DATA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->IndirectDataStartAddress, 6, 31);

   dw[2] =
      util_bitpack_uint(values->InlineData[0], 0, 31);

   dw[3] =
      util_bitpack_uint(values->InlineData[1], 0, 31);

   dw[4] =
      util_bitpack_uint(values->InlineData[2], 0, 31);

   dw[5] =
      util_bitpack_uint(values->InlineData[3], 0, 31);

   dw[6] =
      util_bitpack_uint(values->InlineData[4], 0, 31);

   dw[7] =
      util_bitpack_uint(values->InlineData[5], 0, 31);

   dw[8] =
      util_bitpack_uint(values->InlineData[6], 0, 31);

   dw[9] =
      util_bitpack_uint(values->InlineData[7], 0, 31);
}

#define GFX30_3DSTATE_MONOFILTER_SIZE_length      2
#define GFX30_3DSTATE_MONOFILTER_SIZE_length_bias      2
#define GFX30_3DSTATE_MONOFILTER_SIZE_header    \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     17,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_MONOFILTER_SIZE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MonochromeFilterHeight;
   uint32_t                             MonochromeFilterWidth;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MONOFILTER_SIZE_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_3DSTATE_MONOFILTER_SIZE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MonochromeFilterHeight, 0, 2) |
      util_bitpack_uint(values->MonochromeFilterWidth, 3, 5);
}

#define GFX30_3DSTATE_MULTISAMPLE_length       2
#define GFX30_3DSTATE_MULTISAMPLE_length_bias      2
#define GFX30_3DSTATE_MULTISAMPLE_header        \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     13,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3,  \
   .PixelPositionOffsetEnable           =      0

struct GFX30_3DSTATE_MULTISAMPLE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             NumberofMultisamples;
   uint32_t                             PixelLocation;
#define CENTER                                   0
#define UL_CORNER                                1
   bool                                 PixelPositionOffsetEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_MULTISAMPLE_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_MULTISAMPLE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->NumberofMultisamples, 1, 3) |
      util_bitpack_uint(values->PixelLocation, 4, 4) |
      util_bitpack_uint(values->PixelPositionOffsetEnable, 5, 5);
}

#define GFX30_3DSTATE_POLY_STIPPLE_OFFSET_length      2
#define GFX30_3DSTATE_POLY_STIPPLE_OFFSET_length_bias      2
#define GFX30_3DSTATE_POLY_STIPPLE_OFFSET_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =      6,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_POLY_STIPPLE_OFFSET {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             PolygonStippleYOffset;
   uint32_t                             PolygonStippleXOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_POLY_STIPPLE_OFFSET_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_3DSTATE_POLY_STIPPLE_OFFSET * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PolygonStippleYOffset, 0, 4) |
      util_bitpack_uint(values->PolygonStippleXOffset, 8, 12);
}

#define GFX30_3DSTATE_POLY_STIPPLE_PATTERN_length     33
#define GFX30_3DSTATE_POLY_STIPPLE_PATTERN_length_bias      2
#define GFX30_3DSTATE_POLY_STIPPLE_PATTERN_header\
   .DWordLength                         =     31,  \
   ._3DCommandSubOpcode                 =      7,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_POLY_STIPPLE_PATTERN {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             PatternRow[32];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_POLY_STIPPLE_PATTERN_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_3DSTATE_POLY_STIPPLE_PATTERN * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PatternRow[0], 0, 31);

   dw[2] =
      util_bitpack_uint(values->PatternRow[1], 0, 31);

   dw[3] =
      util_bitpack_uint(values->PatternRow[2], 0, 31);

   dw[4] =
      util_bitpack_uint(values->PatternRow[3], 0, 31);

   dw[5] =
      util_bitpack_uint(values->PatternRow[4], 0, 31);

   dw[6] =
      util_bitpack_uint(values->PatternRow[5], 0, 31);

   dw[7] =
      util_bitpack_uint(values->PatternRow[6], 0, 31);

   dw[8] =
      util_bitpack_uint(values->PatternRow[7], 0, 31);

   dw[9] =
      util_bitpack_uint(values->PatternRow[8], 0, 31);

   dw[10] =
      util_bitpack_uint(values->PatternRow[9], 0, 31);

   dw[11] =
      util_bitpack_uint(values->PatternRow[10], 0, 31);

   dw[12] =
      util_bitpack_uint(values->PatternRow[11], 0, 31);

   dw[13] =
      util_bitpack_uint(values->PatternRow[12], 0, 31);

   dw[14] =
      util_bitpack_uint(values->PatternRow[13], 0, 31);

   dw[15] =
      util_bitpack_uint(values->PatternRow[14], 0, 31);

   dw[16] =
      util_bitpack_uint(values->PatternRow[15], 0, 31);

   dw[17] =
      util_bitpack_uint(values->PatternRow[16], 0, 31);

   dw[18] =
      util_bitpack_uint(values->PatternRow[17], 0, 31);

   dw[19] =
      util_bitpack_uint(values->PatternRow[18], 0, 31);

   dw[20] =
      util_bitpack_uint(values->PatternRow[19], 0, 31);

   dw[21] =
      util_bitpack_uint(values->PatternRow[20], 0, 31);

   dw[22] =
      util_bitpack_uint(values->PatternRow[21], 0, 31);

   dw[23] =
      util_bitpack_uint(values->PatternRow[22], 0, 31);

   dw[24] =
      util_bitpack_uint(values->PatternRow[23], 0, 31);

   dw[25] =
      util_bitpack_uint(values->PatternRow[24], 0, 31);

   dw[26] =
      util_bitpack_uint(values->PatternRow[25], 0, 31);

   dw[27] =
      util_bitpack_uint(values->PatternRow[26], 0, 31);

   dw[28] =
      util_bitpack_uint(values->PatternRow[27], 0, 31);

   dw[29] =
      util_bitpack_uint(values->PatternRow[28], 0, 31);

   dw[30] =
      util_bitpack_uint(values->PatternRow[29], 0, 31);

   dw[31] =
      util_bitpack_uint(values->PatternRow[30], 0, 31);

   dw[32] =
      util_bitpack_uint(values->PatternRow[31], 0, 31);
}

#define GFX30_3DSTATE_PRIMITIVE_REPLICATION_length      6
#define GFX30_3DSTATE_PRIMITIVE_REPLICATION_length_bias      2
#define GFX30_3DSTATE_PRIMITIVE_REPLICATION_header\
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =    108,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PRIMITIVE_REPLICATION {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ReplicationCount;
   uint32_t                             ReplicaMask;
   uint32_t                             ViewportOffset[16];
   uint32_t                             RTAIOffset[16];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PRIMITIVE_REPLICATION_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_PRIMITIVE_REPLICATION * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ReplicationCount, 0, 3) |
      util_bitpack_uint(values->ReplicaMask, 16, 31);

   dw[2] =
      util_bitpack_uint(values->ViewportOffset[0], 0, 3) |
      util_bitpack_uint(values->ViewportOffset[1], 4, 7) |
      util_bitpack_uint(values->ViewportOffset[2], 8, 11) |
      util_bitpack_uint(values->ViewportOffset[3], 12, 15) |
      util_bitpack_uint(values->ViewportOffset[4], 16, 19) |
      util_bitpack_uint(values->ViewportOffset[5], 20, 23) |
      util_bitpack_uint(values->ViewportOffset[6], 24, 27) |
      util_bitpack_uint(values->ViewportOffset[7], 28, 31);

   dw[3] =
      util_bitpack_uint(values->ViewportOffset[8], 0, 3) |
      util_bitpack_uint(values->ViewportOffset[9], 4, 7) |
      util_bitpack_uint(values->ViewportOffset[10], 8, 11) |
      util_bitpack_uint(values->ViewportOffset[11], 12, 15) |
      util_bitpack_uint(values->ViewportOffset[12], 16, 19) |
      util_bitpack_uint(values->ViewportOffset[13], 20, 23) |
      util_bitpack_uint(values->ViewportOffset[14], 24, 27) |
      util_bitpack_uint(values->ViewportOffset[15], 28, 31);

   dw[4] =
      util_bitpack_uint(values->RTAIOffset[0], 0, 3) |
      util_bitpack_uint(values->RTAIOffset[1], 4, 7) |
      util_bitpack_uint(values->RTAIOffset[2], 8, 11) |
      util_bitpack_uint(values->RTAIOffset[3], 12, 15) |
      util_bitpack_uint(values->RTAIOffset[4], 16, 19) |
      util_bitpack_uint(values->RTAIOffset[5], 20, 23) |
      util_bitpack_uint(values->RTAIOffset[6], 24, 27) |
      util_bitpack_uint(values->RTAIOffset[7], 28, 31);

   dw[5] =
      util_bitpack_uint(values->RTAIOffset[8], 0, 3) |
      util_bitpack_uint(values->RTAIOffset[9], 4, 7) |
      util_bitpack_uint(values->RTAIOffset[10], 8, 11) |
      util_bitpack_uint(values->RTAIOffset[11], 12, 15) |
      util_bitpack_uint(values->RTAIOffset[12], 16, 19) |
      util_bitpack_uint(values->RTAIOffset[13], 20, 23) |
      util_bitpack_uint(values->RTAIOffset[14], 24, 27) |
      util_bitpack_uint(values->RTAIOffset[15], 28, 31);
}

#define GFX30_3DSTATE_PS_length               12
#define GFX30_3DSTATE_PS_length_bias           2
#define GFX30_3DSTATE_PS_header                 \
   .DWordLength                         =     10,  \
   ._3DCommandSubOpcode                 =     32,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 Kernel0Enable;
   uint32_t                             Kernel0SIMDWidth;
#define PS_SIMD16                                0
#define PS_SIMD32                                1
   uint64_t                             KernelStartPointer0;
   uint32_t                             RegistersPerThread;
   bool                                 SoftwareExceptionEnable;
   bool                                 MaskStackExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             RoundingMode;
#define RTNE                                     0
#define RU                                       1
#define RD                                       2
#define RTZ                                      3
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             ThreadDispatchPriority;
#define Normal                                   0
#define High                                     1
   uint32_t                             BindingTableEntryCount;
   uint32_t                             SinglePrecisionDenormalMode;
#define FlushedtoZero                            0
#define Retained                                 1
   uint32_t                             SamplerCount;
#define NoSamplers                               0
#define _14Samplers                              1
#define _58Samplers                              2
#define _912Samplers                             3
#define _1316Samplers                            4
   uint32_t                             VectorMaskEnable;
#define Dmask                                    0
#define Vmask                                    1
   uint32_t                             SingleProgramFlow;
#define Multiple                                 0
#define Single                                   1
   uint32_t                             ScratchSpaceBuffer;
   uint32_t                             PositionXYOffsetSelect;
#define POSOFFSET_NONE                           0
#define POSOFFSET_CENTROID                       2
#define POSOFFSET_SAMPLE                         3
   uint32_t                             RenderTargetResolveType;
#define RESOLVE_DISABLED                         0
   bool                                 RenderTargetFastClearEnable;
   bool                                 OverlappingSubspansEnable;
   uint32_t                             _3DScoreboardAddressSizeselect;
#define _2x2                                     0
#define _4x2                                     1
   uint32_t                             ClearResolveBTIforRenderTarget;
   bool                                 PixelScoreboardDisable;
   uint32_t                             MaximumNumberofThreadsPerPSD;
   uint32_t                             DispatchGRFStartRegisterForConstantSetupData1;
   uint32_t                             DispatchGRFStartRegisterForConstantSetupData0;
   uint32_t                             Kernel0PolyPackingPolicy;
#define POLY_PACK4_DYNAMIC                       0
#define POLY_PACK8_DYNAMIC                       1
#define POLY_PACK16_DYNAMIC                      2
#define POLY_PACK16_FIXED                        3
#define POLY_PACK8_FIXED                         4
   uint32_t                             Kernel0MaximumPolysperThread;
   uint32_t                             Kernel1SelectionPriority;
#define PRI_SIMD                                 0
#define PRI_POLY                                 1
   bool                                 Kernel1Enable;
   uint32_t                             Kernel1SIMDWidth;
#define PS_SIMD16                                0
#define PS_SIMD32                                1
   uint64_t                             KernelStartPointer1;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PS_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->Kernel0Enable, 0, 0) |
      util_bitpack_uint(values->Kernel0SIMDWidth, 1, 1) |
      __gen_offset(values->KernelStartPointer0, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->RegistersPerThread, 0, 5) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 7, 7) |
      util_bitpack_uint(values->MaskStackExceptionEnable, 11, 11) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->RoundingMode, 14, 15) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->ThreadDispatchPriority, 17, 17) |
      util_bitpack_uint(values->BindingTableEntryCount, 18, 25) |
      util_bitpack_uint(values->SinglePrecisionDenormalMode, 26, 26) |
      util_bitpack_uint(values->SamplerCount, 27, 29) |
      util_bitpack_uint(values->VectorMaskEnable, 30, 30) |
      util_bitpack_uint(values->SingleProgramFlow, 31, 31);

   dw[4] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[5] = 0;

   dw[6] =
      util_bitpack_uint(values->PositionXYOffsetSelect, 3, 4) |
      util_bitpack_uint(values->RenderTargetResolveType, 6, 7) |
      util_bitpack_uint(values->RenderTargetFastClearEnable, 8, 8) |
      util_bitpack_uint(values->OverlappingSubspansEnable, 9, 9) |
      util_bitpack_uint(values->_3DScoreboardAddressSizeselect, 10, 10) |
      util_bitpack_uint(values->ClearResolveBTIforRenderTarget, 12, 19) |
      util_bitpack_uint(values->PixelScoreboardDisable, 21, 21) |
      util_bitpack_uint(values->MaximumNumberofThreadsPerPSD, 23, 31);

   dw[7] =
      util_bitpack_uint(values->DispatchGRFStartRegisterForConstantSetupData1, 8, 15) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForConstantSetupData0, 16, 23) |
      util_bitpack_uint(values->Kernel0PolyPackingPolicy, 24, 26) |
      util_bitpack_uint(values->Kernel0MaximumPolysperThread, 27, 29) |
      util_bitpack_uint(values->Kernel1SelectionPriority, 31, 31);

   const uint64_t v8 =
      util_bitpack_uint(values->Kernel1Enable, 0, 0) |
      util_bitpack_uint(values->Kernel1SIMDWidth, 1, 1) |
      __gen_offset(values->KernelStartPointer1, 6, 63);
   dw[8] = v8;
   dw[9] = v8 >> 32;

   dw[10] = 0;

   dw[11] = 0;
}

#define GFX30_3DSTATE_PS_BLEND_length          2
#define GFX30_3DSTATE_PS_BLEND_length_bias      2
#define GFX30_3DSTATE_PS_BLEND_header           \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     77,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PS_BLEND {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 IndependentAlphaBlendEnable;
   bool                                 AlphaTestEnable;
   enum GFX30_3D_Color_Buffer_Blend_Factor DestinationBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Factor SourceBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Factor DestinationAlphaBlendFactor;
   enum GFX30_3D_Color_Buffer_Blend_Factor SourceAlphaBlendFactor;
   bool                                 ColorBufferBlendEnable;
   bool                                 HasWriteableRT;
   bool                                 AlphaToCoverageEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PS_BLEND_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_3DSTATE_PS_BLEND * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->IndependentAlphaBlendEnable, 7, 7) |
      util_bitpack_uint(values->AlphaTestEnable, 8, 8) |
      util_bitpack_uint(values->DestinationBlendFactor, 9, 13) |
      util_bitpack_uint(values->SourceBlendFactor, 14, 18) |
      util_bitpack_uint(values->DestinationAlphaBlendFactor, 19, 23) |
      util_bitpack_uint(values->SourceAlphaBlendFactor, 24, 28) |
      util_bitpack_uint(values->ColorBufferBlendEnable, 29, 29) |
      util_bitpack_uint(values->HasWriteableRT, 30, 30) |
      util_bitpack_uint(values->AlphaToCoverageEnable, 31, 31);
}

#define GFX30_3DSTATE_PS_EXTRA_length          2
#define GFX30_3DSTATE_PS_EXTRA_length_bias      2
#define GFX30_3DSTATE_PS_EXTRA_header           \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     79,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PS_EXTRA {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             InputCoverageMaskState;
#define ICMS_NONE                                0
#define ICMS_NORMAL                              1
#define ICMS_INNER_CONSERVATIVE                  2
#define ICMS_DEPTH_COVERAGE                      3
   bool                                 PixelShaderHasUAV;
   bool                                 PixelShaderIsPerCoarsePixel;
   bool                                 PixelShaderComputesStencil;
   bool                                 PixelShaderIsPerSample;
   bool                                 PixelShaderDisablesAlphaToCoverage;
   bool                                 SimplePSHint;
   bool                                 PixelShaderRequiresCentroidOffsets;
   bool                                 EnablePSDependencyOnCPsizeChange;
   bool                                 PixelShaderRequiresSubpixelSampleOffsets;
   bool                                 PixelShaderRequiresNonPerspectiveBaryPlaneCoefficients;
   bool                                 PixelShaderRequiresPerspectiveBaryPlaneCoefficients;
   bool                                 PixelShaderRequiresSourceDepthandorWPlaneCoefficients;
   bool                                 PixelShaderRequiresRequestedCoarsePixelShadingSize;
   bool                                 PixelShaderUsesSourceW;
   bool                                 PixelShaderUsesSourceDepth;
   bool                                 ForceComputedDepth;
   uint32_t                             PixelShaderComputedDepthMode;
#define PSCDEPTH_OFF                             0
#define PSCDEPTH_ON                              1
#define PSCDEPTH_ON_GE                           2
#define PSCDEPTH_ON_LE                           3
   bool                                 PixelShaderKillsPixel;
   bool                                 oMaskPresenttoRenderTarget;
   bool                                 PixelShaderValid;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PS_EXTRA_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_3DSTATE_PS_EXTRA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->InputCoverageMaskState, 0, 1) |
      util_bitpack_uint(values->PixelShaderHasUAV, 2, 2) |
      util_bitpack_uint(values->PixelShaderIsPerCoarsePixel, 4, 4) |
      util_bitpack_uint(values->PixelShaderComputesStencil, 5, 5) |
      util_bitpack_uint(values->PixelShaderIsPerSample, 6, 6) |
      util_bitpack_uint(values->PixelShaderDisablesAlphaToCoverage, 7, 7) |
      util_bitpack_uint(values->SimplePSHint, 9, 9) |
      util_bitpack_uint(values->PixelShaderRequiresCentroidOffsets, 16, 16) |
      util_bitpack_uint(values->EnablePSDependencyOnCPsizeChange, 17, 17) |
      util_bitpack_uint(values->PixelShaderRequiresSubpixelSampleOffsets, 18, 18) |
      util_bitpack_uint(values->PixelShaderRequiresNonPerspectiveBaryPlaneCoefficients, 19, 19) |
      util_bitpack_uint(values->PixelShaderRequiresPerspectiveBaryPlaneCoefficients, 20, 20) |
      util_bitpack_uint(values->PixelShaderRequiresSourceDepthandorWPlaneCoefficients, 21, 21) |
      util_bitpack_uint(values->PixelShaderRequiresRequestedCoarsePixelShadingSize, 22, 22) |
      util_bitpack_uint(values->PixelShaderUsesSourceW, 23, 23) |
      util_bitpack_uint(values->PixelShaderUsesSourceDepth, 24, 24) |
      util_bitpack_uint(values->ForceComputedDepth, 25, 25) |
      util_bitpack_uint(values->PixelShaderComputedDepthMode, 26, 27) |
      util_bitpack_uint(values->PixelShaderKillsPixel, 28, 28) |
      util_bitpack_uint(values->oMaskPresenttoRenderTarget, 29, 29) |
      util_bitpack_uint(values->PixelShaderValid, 31, 31);
}

#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS_length      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS_length_bias      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     20,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ConstantBufferSize;
   uint32_t                             ConstantBufferOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ConstantBufferSize, 0, 5) |
      util_bitpack_uint(values->ConstantBufferOffset, 16, 20);
}

#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS_length      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS_length_bias      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     21,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ConstantBufferSize;
   uint32_t                             ConstantBufferOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ConstantBufferSize, 0, 5) |
      util_bitpack_uint(values->ConstantBufferOffset, 16, 20);
}

#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS_length      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS_length_bias      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     19,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ConstantBufferSize;
   uint32_t                             ConstantBufferOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ConstantBufferSize, 0, 5) |
      util_bitpack_uint(values->ConstantBufferOffset, 16, 20);
}

#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length_bias      2
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     18,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ConstantBufferSize;
   uint32_t                             ConstantBufferOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ConstantBufferSize, 0, 5) |
      util_bitpack_uint(values->ConstantBufferOffset, 16, 20);
}

#define GFX30_3DSTATE_RASTER_length            5
#define GFX30_3DSTATE_RASTER_length_bias       2
#define GFX30_3DSTATE_RASTER_header             \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     80,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_RASTER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 ViewportZNearClipTestEnable;
   bool                                 ScissorRectangleEnable;
   bool                                 AntialiasingEnable;
   uint32_t                             BackFaceFillMode;
#define FILL_MODE_SOLID                          0
#define FILL_MODE_WIREFRAME                      1
#define FILL_MODE_POINT                          2
   uint32_t                             FrontFaceFillMode;
#define FILL_MODE_SOLID                          0
#define FILL_MODE_WIREFRAME                      1
#define FILL_MODE_POINT                          2
   bool                                 GlobalDepthOffsetEnablePoint;
   bool                                 GlobalDepthOffsetEnableWireframe;
   bool                                 GlobalDepthOffsetEnableSolid;
   uint32_t                             DXMultisampleRasterizationMode;
#define MSRASTMODE_OFF_PIXEL                     0
#define MSRASTMODE_OFF_PATTERN                   1
#define MSRASTMODE_ON_PIXEL                      2
#define MSRASTMODE_ON_PATTERN                    3
   bool                                 DXMultisampleRasterizationEnable;
   bool                                 SmoothPointEnable;
   uint32_t                             ForceMultisampling;
   uint32_t                             CullMode;
#define CULLMODE_BOTH                            0
#define CULLMODE_NONE                            1
#define CULLMODE_FRONT                           2
#define CULLMODE_BACK                            3
   uint32_t                             ForcedSampleCount;
#define FSC_NUMRASTSAMPLES_0                     0
#define FSC_NUMRASTSAMPLES_1                     1
#define FSC_NUMRASTSAMPLES_2                     2
#define FSC_NUMRASTSAMPLES_4                     3
#define FSC_NUMRASTSAMPLES_8                     4
#define FSC_NUMRASTSAMPLES_16                    5
   uint32_t                             FrontWinding;
#define Clockwise                                0
#define CounterClockwise                         1
   uint32_t                             APIMode;
#define DX9OGL                                   0
#define DX100                                    1
#define DX101                                    2
   bool                                 ConservativeRasterizationEnable;
   bool                                 ViewportZFarClipTestEnable;
   bool                                 LegacyBaryAssignmentDisable;
   float                                GlobalDepthOffsetConstant;
   float                                GlobalDepthOffsetScale;
   float                                GlobalDepthOffsetClamp;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_RASTER_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_3DSTATE_RASTER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ViewportZNearClipTestEnable, 0, 0) |
      util_bitpack_uint(values->ScissorRectangleEnable, 1, 1) |
      util_bitpack_uint(values->AntialiasingEnable, 2, 2) |
      util_bitpack_uint(values->BackFaceFillMode, 3, 4) |
      util_bitpack_uint(values->FrontFaceFillMode, 5, 6) |
      util_bitpack_uint(values->GlobalDepthOffsetEnablePoint, 7, 7) |
      util_bitpack_uint(values->GlobalDepthOffsetEnableWireframe, 8, 8) |
      util_bitpack_uint(values->GlobalDepthOffsetEnableSolid, 9, 9) |
      util_bitpack_uint(values->DXMultisampleRasterizationMode, 10, 11) |
      util_bitpack_uint(values->DXMultisampleRasterizationEnable, 12, 12) |
      util_bitpack_uint(values->SmoothPointEnable, 13, 13) |
      util_bitpack_uint(values->ForceMultisampling, 14, 14) |
      util_bitpack_uint(values->CullMode, 16, 17) |
      util_bitpack_uint(values->ForcedSampleCount, 18, 20) |
      util_bitpack_uint(values->FrontWinding, 21, 21) |
      util_bitpack_uint(values->APIMode, 22, 23) |
      util_bitpack_uint(values->ConservativeRasterizationEnable, 24, 24) |
      util_bitpack_uint(values->ViewportZFarClipTestEnable, 26, 26) |
      util_bitpack_uint(values->LegacyBaryAssignmentDisable, 27, 27);

   dw[2] =
      util_bitpack_float(values->GlobalDepthOffsetConstant);

   dw[3] =
      util_bitpack_float(values->GlobalDepthOffsetScale);

   dw[4] =
      util_bitpack_float(values->GlobalDepthOffsetClamp);
}

#define GFX30_3DSTATE_RS_CONSTANT_POINTER_length      4
#define GFX30_3DSTATE_RS_CONSTANT_POINTER_length_bias      2
#define GFX30_3DSTATE_RS_CONSTANT_POINTER_header\
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     84,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_RS_CONSTANT_POINTER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             OperationLoadorStore;
#define RS_Store                                 0
#define RS_Load                                  1
   uint32_t                             ShaderSelect;
#define VS                                       0
#define PS                                       4
   __gen_address_type                   GlobalConstantBufferAddress;
   __gen_address_type                   GlobalConstantBufferAddressHigh;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_RS_CONSTANT_POINTER_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_3DSTATE_RS_CONSTANT_POINTER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->OperationLoadorStore, 12, 12) |
      util_bitpack_uint(values->ShaderSelect, 28, 30);

   dw[2] = __gen_address(data, &dw[2], values->GlobalConstantBufferAddress, 0, 6, 31);

   dw[3] = __gen_address(data, &dw[3], values->GlobalConstantBufferAddressHigh, 0, 0, 31);
}

#define GFX30_3DSTATE_SAMPLER_PALETTE_LOAD0_length_bias      2
#define GFX30_3DSTATE_SAMPLER_PALETTE_LOAD0_header\
   ._3DCommandSubOpcode                 =      2,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_PALETTE_LOAD0 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_PALETTE_LOAD0_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_PALETTE_LOAD0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_3DSTATE_SAMPLER_PALETTE_LOAD1_length_bias      2
#define GFX30_3DSTATE_SAMPLER_PALETTE_LOAD1_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     12,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_PALETTE_LOAD1 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_PALETTE_LOAD1_pack(__attribute__((unused)) __gen_user_data *data,
                                         __attribute__((unused)) void * restrict dst,
                                         __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_PALETTE_LOAD1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS_length      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS_length_bias      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     45,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoDSSamplerState;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoDSSamplerState, 5, 31);
}

#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS_length      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS_length_bias      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     46,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoGSSamplerState;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoGSSamplerState, 5, 31);
}

#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS_length      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS_length_bias      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     44,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoHSSamplerState;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoHSSamplerState, 5, 31);
}

#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS_length      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS_length_bias      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     47,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoPSSamplerState;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_PS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoPSSamplerState, 5, 31);
}

#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS_length      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS_length_bias      2
#define GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     43,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             PointertoVSSamplerState;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                             __attribute__((unused)) void * restrict dst,
                                             __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLER_STATE_POINTERS_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->PointertoVSSamplerState, 5, 31);
}

#define GFX30_3DSTATE_SAMPLE_MASK_length       2
#define GFX30_3DSTATE_SAMPLE_MASK_length_bias      2
#define GFX30_3DSTATE_SAMPLE_MASK_header        \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     24,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLE_MASK {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SampleMask;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLE_MASK_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLE_MASK * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SampleMask, 0, 15);
}

#define GFX30_3DSTATE_SAMPLE_PATTERN_length      9
#define GFX30_3DSTATE_SAMPLE_PATTERN_length_bias      2
#define GFX30_3DSTATE_SAMPLE_PATTERN_header     \
   .DWordLength                         =      7,  \
   ._3DCommandSubOpcode                 =     28,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SAMPLE_PATTERN {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   float                                _16xSample0YOffset;
   float                                _16xSample0XOffset;
   float                                _16xSample1YOffset;
   float                                _16xSample1XOffset;
   float                                _16xSample2YOffset;
   float                                _16xSample2XOffset;
   float                                _16xSample3YOffset;
   float                                _16xSample3XOffset;
   float                                _16xSample4YOffset;
   float                                _16xSample4XOffset;
   float                                _16xSample5YOffset;
   float                                _16xSample5XOffset;
   float                                _16xSample6YOffset;
   float                                _16xSample6XOffset;
   float                                _16xSample7YOffset;
   float                                _16xSample7XOffset;
   float                                _16xSample8YOffset;
   float                                _16xSample8XOffset;
   float                                _16xSample9YOffset;
   float                                _16xSample9XOffset;
   float                                _16xSample10YOffset;
   float                                _16xSample10XOffset;
   float                                _16xSample11YOffset;
   float                                _16xSample11XOffset;
   float                                _16xSample12YOffset;
   float                                _16xSample12XOffset;
   float                                _16xSample13YOffset;
   float                                _16xSample13XOffset;
   float                                _16xSample14YOffset;
   float                                _16xSample14XOffset;
   float                                _16xSample15YOffset;
   float                                _16xSample15XOffset;
   float                                _8xSample4YOffset;
   float                                _8xSample4XOffset;
   float                                _8xSample5YOffset;
   float                                _8xSample5XOffset;
   float                                _8xSample6YOffset;
   float                                _8xSample6XOffset;
   float                                _8xSample7YOffset;
   float                                _8xSample7XOffset;
   float                                _8xSample0YOffset;
   float                                _8xSample0XOffset;
   float                                _8xSample1YOffset;
   float                                _8xSample1XOffset;
   float                                _8xSample2YOffset;
   float                                _8xSample2XOffset;
   float                                _8xSample3YOffset;
   float                                _8xSample3XOffset;
   float                                _4xSample0YOffset;
   float                                _4xSample0XOffset;
   float                                _4xSample1YOffset;
   float                                _4xSample1XOffset;
   float                                _4xSample2YOffset;
   float                                _4xSample2XOffset;
   float                                _4xSample3YOffset;
   float                                _4xSample3XOffset;
   float                                _2xSample0YOffset;
   float                                _2xSample0XOffset;
   float                                _2xSample1YOffset;
   float                                _2xSample1XOffset;
   float                                _1xSample0YOffset;
   float                                _1xSample0XOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SAMPLE_PATTERN_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_SAMPLE_PATTERN * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_ufixed(values->_16xSample0YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_16xSample0XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_16xSample1YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_16xSample1XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_16xSample2YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_16xSample2XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_16xSample3YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_16xSample3XOffset, 28, 31, 4);

   dw[2] =
      util_bitpack_ufixed(values->_16xSample4YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_16xSample4XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_16xSample5YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_16xSample5XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_16xSample6YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_16xSample6XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_16xSample7YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_16xSample7XOffset, 28, 31, 4);

   dw[3] =
      util_bitpack_ufixed(values->_16xSample8YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_16xSample8XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_16xSample9YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_16xSample9XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_16xSample10YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_16xSample10XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_16xSample11YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_16xSample11XOffset, 28, 31, 4);

   dw[4] =
      util_bitpack_ufixed(values->_16xSample12YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_16xSample12XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_16xSample13YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_16xSample13XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_16xSample14YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_16xSample14XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_16xSample15YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_16xSample15XOffset, 28, 31, 4);

   dw[5] =
      util_bitpack_ufixed(values->_8xSample4YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_8xSample4XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_8xSample5YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_8xSample5XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_8xSample6YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_8xSample6XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_8xSample7YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_8xSample7XOffset, 28, 31, 4);

   dw[6] =
      util_bitpack_ufixed(values->_8xSample0YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_8xSample0XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_8xSample1YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_8xSample1XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_8xSample2YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_8xSample2XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_8xSample3YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_8xSample3XOffset, 28, 31, 4);

   dw[7] =
      util_bitpack_ufixed(values->_4xSample0YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_4xSample0XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_4xSample1YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_4xSample1XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_4xSample2YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_4xSample2XOffset, 20, 23, 4) |
      util_bitpack_ufixed(values->_4xSample3YOffset, 24, 27, 4) |
      util_bitpack_ufixed(values->_4xSample3XOffset, 28, 31, 4);

   dw[8] =
      util_bitpack_ufixed(values->_2xSample0YOffset, 0, 3, 4) |
      util_bitpack_ufixed(values->_2xSample0XOffset, 4, 7, 4) |
      util_bitpack_ufixed(values->_2xSample1YOffset, 8, 11, 4) |
      util_bitpack_ufixed(values->_2xSample1XOffset, 12, 15, 4) |
      util_bitpack_ufixed(values->_1xSample0YOffset, 16, 19, 4) |
      util_bitpack_ufixed(values->_1xSample0XOffset, 20, 23, 4);
}

#define GFX30_3DSTATE_SBE_length               6
#define GFX30_3DSTATE_SBE_length_bias          2
#define GFX30_3DSTATE_SBE_header                \
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =     31,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SBE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             PrimitiveIDOverrideAttributeSelect;
   uint32_t                             VertexURBEntryReadOffset;
   uint32_t                             VertexURBEntryReadLength;
   bool                                 PrimitiveIDOverrideComponentX;
   bool                                 PrimitiveIDOverrideComponentY;
   bool                                 PrimitiveIDOverrideComponentZ;
   bool                                 PrimitiveIDOverrideComponentW;
   uint32_t                             PointSpriteTextureCoordinateOrigin;
#define UPPERLEFT                                0
#define LOWERLEFT                                1
   bool                                 AttributeSwizzleEnable;
   uint32_t                             NumberofSFOutputAttributes;
   bool                                 ForceVertexURBEntryReadOffset;
   bool                                 ForceVertexURBEntryReadLength;
   bool                                 VertexAttributesBypass;
   uint32_t                             PointSpriteTextureCoordinateEnable;
   uint32_t                             ConstantInterpolationEnable;
   uint32_t                             AttributeActiveComponentFormat[32];
#define ACTIVE_COMPONENT_DISABLED                0
#define ACTIVE_COMPONENT_XY                      1
#define ACTIVE_COMPONENT_XYZ                     2
#define ACTIVE_COMPONENT_XYZW                    3
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SBE_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_3DSTATE_SBE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PrimitiveIDOverrideAttributeSelect, 0, 4) |
      util_bitpack_uint(values->VertexURBEntryReadOffset, 5, 10) |
      util_bitpack_uint(values->VertexURBEntryReadLength, 11, 15) |
      util_bitpack_uint(values->PrimitiveIDOverrideComponentX, 16, 16) |
      util_bitpack_uint(values->PrimitiveIDOverrideComponentY, 17, 17) |
      util_bitpack_uint(values->PrimitiveIDOverrideComponentZ, 18, 18) |
      util_bitpack_uint(values->PrimitiveIDOverrideComponentW, 19, 19) |
      util_bitpack_uint(values->PointSpriteTextureCoordinateOrigin, 20, 20) |
      util_bitpack_uint(values->AttributeSwizzleEnable, 21, 21) |
      util_bitpack_uint(values->NumberofSFOutputAttributes, 22, 27) |
      util_bitpack_uint(values->ForceVertexURBEntryReadOffset, 28, 28) |
      util_bitpack_uint(values->ForceVertexURBEntryReadLength, 29, 29) |
      util_bitpack_uint(values->VertexAttributesBypass, 31, 31);

   dw[2] =
      util_bitpack_uint(values->PointSpriteTextureCoordinateEnable, 0, 31);

   dw[3] =
      util_bitpack_uint(values->ConstantInterpolationEnable, 0, 31);

   dw[4] =
      util_bitpack_uint(values->AttributeActiveComponentFormat[0], 0, 1) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[1], 2, 3) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[2], 4, 5) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[3], 6, 7) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[4], 8, 9) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[5], 10, 11) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[6], 12, 13) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[7], 14, 15) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[8], 16, 17) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[9], 18, 19) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[10], 20, 21) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[11], 22, 23) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[12], 24, 25) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[13], 26, 27) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[14], 28, 29) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[15], 30, 31);

   dw[5] =
      util_bitpack_uint(values->AttributeActiveComponentFormat[16], 0, 1) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[17], 2, 3) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[18], 4, 5) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[19], 6, 7) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[20], 8, 9) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[21], 10, 11) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[22], 12, 13) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[23], 14, 15) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[24], 16, 17) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[25], 18, 19) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[26], 20, 21) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[27], 22, 23) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[28], 24, 25) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[29], 26, 27) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[30], 28, 29) |
      util_bitpack_uint(values->AttributeActiveComponentFormat[31], 30, 31);
}

#define GFX30_3DSTATE_SBE_MESH_length          2
#define GFX30_3DSTATE_SBE_MESH_length_bias      2
#define GFX30_3DSTATE_SBE_MESH_header           \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    130,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SBE_MESH {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             PerVertexURBEntryOutputReadLength;
   uint32_t                             PerVertexURBEntryOutputReadOffset;
   uint32_t                             PerPrimitiveURBEntryOutputReadLength;
   uint32_t                             PerPrimitiveURBEntryOutputReadOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SBE_MESH_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_3DSTATE_SBE_MESH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PerVertexURBEntryOutputReadLength, 0, 4) |
      util_bitpack_uint(values->PerVertexURBEntryOutputReadOffset, 5, 10) |
      util_bitpack_uint(values->PerPrimitiveURBEntryOutputReadLength, 11, 15) |
      util_bitpack_uint(values->PerPrimitiveURBEntryOutputReadOffset, 16, 21);
}

#define GFX30_3DSTATE_SBE_SWIZ_length         11
#define GFX30_3DSTATE_SBE_SWIZ_length_bias      2
#define GFX30_3DSTATE_SBE_SWIZ_header           \
   .DWordLength                         =      9,  \
   ._3DCommandSubOpcode                 =     81,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SBE_SWIZ {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL Attribute[16];
   uint32_t                             AttributeWrapShortestEnables[16];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SBE_SWIZ_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_3DSTATE_SBE_SWIZ * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   uint32_t v1_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v1_0, &values->Attribute[0]);

   uint32_t v1_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v1_1, &values->Attribute[1]);

   dw[1] =
      util_bitpack_uint(v1_0, 0, 15) |
      util_bitpack_uint(v1_1, 16, 31);

   uint32_t v2_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v2_0, &values->Attribute[2]);

   uint32_t v2_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v2_1, &values->Attribute[3]);

   dw[2] =
      util_bitpack_uint(v2_0, 0, 15) |
      util_bitpack_uint(v2_1, 16, 31);

   uint32_t v3_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v3_0, &values->Attribute[4]);

   uint32_t v3_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v3_1, &values->Attribute[5]);

   dw[3] =
      util_bitpack_uint(v3_0, 0, 15) |
      util_bitpack_uint(v3_1, 16, 31);

   uint32_t v4_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v4_0, &values->Attribute[6]);

   uint32_t v4_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v4_1, &values->Attribute[7]);

   dw[4] =
      util_bitpack_uint(v4_0, 0, 15) |
      util_bitpack_uint(v4_1, 16, 31);

   uint32_t v5_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v5_0, &values->Attribute[8]);

   uint32_t v5_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v5_1, &values->Attribute[9]);

   dw[5] =
      util_bitpack_uint(v5_0, 0, 15) |
      util_bitpack_uint(v5_1, 16, 31);

   uint32_t v6_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v6_0, &values->Attribute[10]);

   uint32_t v6_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v6_1, &values->Attribute[11]);

   dw[6] =
      util_bitpack_uint(v6_0, 0, 15) |
      util_bitpack_uint(v6_1, 16, 31);

   uint32_t v7_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v7_0, &values->Attribute[12]);

   uint32_t v7_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v7_1, &values->Attribute[13]);

   dw[7] =
      util_bitpack_uint(v7_0, 0, 15) |
      util_bitpack_uint(v7_1, 16, 31);

   uint32_t v8_0;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v8_0, &values->Attribute[14]);

   uint32_t v8_1;
   GFX30_SF_OUTPUT_ATTRIBUTE_DETAIL_pack(data, &v8_1, &values->Attribute[15]);

   dw[8] =
      util_bitpack_uint(v8_0, 0, 15) |
      util_bitpack_uint(v8_1, 16, 31);

   dw[9] =
      util_bitpack_uint(values->AttributeWrapShortestEnables[0], 0, 3) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[1], 4, 7) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[2], 8, 11) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[3], 12, 15) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[4], 16, 19) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[5], 20, 23) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[6], 24, 27) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[7], 28, 31);

   dw[10] =
      util_bitpack_uint(values->AttributeWrapShortestEnables[8], 0, 3) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[9], 4, 7) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[10], 8, 11) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[11], 12, 15) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[12], 16, 19) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[13], 20, 23) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[14], 24, 27) |
      util_bitpack_uint(values->AttributeWrapShortestEnables[15], 28, 31);
}

#define GFX30_3DSTATE_SCISSOR_STATE_POINTERS_length      2
#define GFX30_3DSTATE_SCISSOR_STATE_POINTERS_length_bias      2
#define GFX30_3DSTATE_SCISSOR_STATE_POINTERS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     15,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SCISSOR_STATE_POINTERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             ScissorRectPointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SCISSOR_STATE_POINTERS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_3DSTATE_SCISSOR_STATE_POINTERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->ScissorRectPointer, 5, 31);
}

#define GFX30_3DSTATE_SF_length                4
#define GFX30_3DSTATE_SF_length_bias           2
#define GFX30_3DSTATE_SF_header                 \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     19,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SF {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 ViewportTransformEnable;
   bool                                 StatisticsEnable;
   bool                                 LegacyGlobalDepthBiasEnable;
   float                                LineWidth;
   uint32_t                             LineEndCapAntialiasingRegionWidth;
#define _05pixels                                0
#define _10pixels                                1
#define _20pixels                                2
#define _40pixels                                3
   uint32_t                             DerefBlockSize;
#define BlockDerefSize32                         0
#define PerPolyDerefMode                         1
#define BlockDerefSize8                          2
#define MeshShaderEODB                           3
   float                                PointWidth;
   uint32_t                             PointWidthSource;
#define Vertex                                   0
#define State                                    1
   uint32_t                             VertexSubPixelPrecisionSelect;
#define _8Bit                                    0
#define _4Bit                                    1
   bool                                 SmoothPointEnable;
   uint32_t                             AALineDistanceMode;
#define AALINEDISTANCE_TRUE                      1
   uint32_t                             TriangleStripOddProvokingVertexSelect;
   uint32_t                             TriangleFanProvokingVertexSelect;
   uint32_t                             LineStripListProvokingVertexSelect;
   uint32_t                             TriangleStripListProvokingVertexSelect;
   bool                                 LastPixelEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SF_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_SF * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ViewportTransformEnable, 1, 1) |
      util_bitpack_uint(values->StatisticsEnable, 10, 10) |
      util_bitpack_uint(values->LegacyGlobalDepthBiasEnable, 11, 11) |
      util_bitpack_ufixed(values->LineWidth, 12, 29, 7);

   dw[2] =
      util_bitpack_uint(values->LineEndCapAntialiasingRegionWidth, 16, 17) |
      util_bitpack_uint(values->DerefBlockSize, 29, 30);

   dw[3] =
      util_bitpack_ufixed(values->PointWidth, 0, 10, 3) |
      util_bitpack_uint(values->PointWidthSource, 11, 11) |
      util_bitpack_uint(values->VertexSubPixelPrecisionSelect, 12, 12) |
      util_bitpack_uint(values->SmoothPointEnable, 13, 13) |
      util_bitpack_uint(values->AALineDistanceMode, 14, 14) |
      util_bitpack_uint(values->TriangleStripOddProvokingVertexSelect, 23, 24) |
      util_bitpack_uint(values->TriangleFanProvokingVertexSelect, 25, 26) |
      util_bitpack_uint(values->LineStripListProvokingVertexSelect, 27, 28) |
      util_bitpack_uint(values->TriangleStripListProvokingVertexSelect, 29, 30) |
      util_bitpack_uint(values->LastPixelEnable, 31, 31);
}

#define GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS_length      2
#define GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS_length_bias      2
#define GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     32,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 SliceHashStatePointerValid;
   uint64_t                             SliceHashTableStatePointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS_pack(__attribute__((unused)) __gen_user_data *data,
                                              __attribute__((unused)) void * restrict dst,
                                              __attribute__((unused)) const struct GFX30_3DSTATE_SLICE_TABLE_STATE_POINTERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SliceHashStatePointerValid, 0, 0) |
      __gen_offset(values->SliceHashTableStatePointer, 6, 31);
}

#define GFX30_3DSTATE_SO_BUFFER_length         8
#define GFX30_3DSTATE_SO_BUFFER_length_bias      2
#define GFX30_3DSTATE_SO_BUFFER_header          \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     24,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 StreamOutputBufferOffsetAddressEnable;
   bool                                 StreamOffsetWriteEnable;
   uint32_t                             MOCS;
   uint32_t                             SOBufferIndex;
   bool                                 SOBufferEnable;
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             SurfaceSize;
   __gen_address_type                   StreamOutputBufferOffsetAddress;
   uint32_t                             StreamOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->StreamOutputBufferOffsetAddressEnable, 20, 20) |
      util_bitpack_uint(values->StreamOffsetWriteEnable, 21, 21) |
      util_bitpack_uint_nonzero(values->MOCS, 22, 28) |
      util_bitpack_uint(values->SOBufferIndex, 29, 30) |
      util_bitpack_uint(values->SOBufferEnable, 31, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SurfaceBaseAddress, 0, 2, 47);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->SurfaceSize, 0, 29);

   const uint64_t v5_address =
      __gen_address(data, &dw[5], values->StreamOutputBufferOffsetAddress, 0, 2, 47);
   dw[5] = v5_address;
   dw[6] = v5_address >> 32;

   dw[7] =
      util_bitpack_uint(values->StreamOffset, 0, 31);
}

#define GFX30_3DSTATE_SO_BUFFER_INDEX_0_length      8
#define GFX30_3DSTATE_SO_BUFFER_INDEX_0_length_bias      2
#define GFX30_3DSTATE_SO_BUFFER_INDEX_0_header  \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     96,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_BUFFER_INDEX_0 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY SOBufferIndexStateBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_INDEX_0_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER_INDEX_0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_pack(data, &dw[1], &values->SOBufferIndexStateBody);
}

#define GFX30_3DSTATE_SO_BUFFER_INDEX_1_length      8
#define GFX30_3DSTATE_SO_BUFFER_INDEX_1_length_bias      2
#define GFX30_3DSTATE_SO_BUFFER_INDEX_1_header  \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     97,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_BUFFER_INDEX_1 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY SOBufferIndexStateBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_INDEX_1_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER_INDEX_1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_pack(data, &dw[1], &values->SOBufferIndexStateBody);
}

#define GFX30_3DSTATE_SO_BUFFER_INDEX_2_length      8
#define GFX30_3DSTATE_SO_BUFFER_INDEX_2_length_bias      2
#define GFX30_3DSTATE_SO_BUFFER_INDEX_2_header  \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     98,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_BUFFER_INDEX_2 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY SOBufferIndexStateBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_INDEX_2_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER_INDEX_2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_pack(data, &dw[1], &values->SOBufferIndexStateBody);
}

#define GFX30_3DSTATE_SO_BUFFER_INDEX_3_length      8
#define GFX30_3DSTATE_SO_BUFFER_INDEX_3_length_bias      2
#define GFX30_3DSTATE_SO_BUFFER_INDEX_3_header  \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =     99,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_BUFFER_INDEX_3 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   struct GFX30_3DSTATE_SO_BUFFER_INDEX_BODY SOBufferIndexStateBody;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_BUFFER_INDEX_3_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_3DSTATE_SO_BUFFER_INDEX_3 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_3DSTATE_SO_BUFFER_INDEX_BODY_pack(data, &dw[1], &values->SOBufferIndexStateBody);
}

#define GFX30_3DSTATE_SO_DECL_LIST_length_bias      2
#define GFX30_3DSTATE_SO_DECL_LIST_header       \
   ._3DCommandSubOpcode                 =     23,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SO_DECL_LIST {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             StreamtoBufferSelects0;
   uint32_t                             StreamtoBufferSelects1;
   uint32_t                             StreamtoBufferSelects2;
   uint32_t                             StreamtoBufferSelects3;
   uint32_t                             NumEntries0;
   uint32_t                             NumEntries1;
   uint32_t                             NumEntries2;
   uint32_t                             NumEntries3;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SO_DECL_LIST_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_SO_DECL_LIST * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->StreamtoBufferSelects0, 0, 3) |
      util_bitpack_uint(values->StreamtoBufferSelects1, 4, 7) |
      util_bitpack_uint(values->StreamtoBufferSelects2, 8, 11) |
      util_bitpack_uint(values->StreamtoBufferSelects3, 12, 15);

   dw[2] =
      util_bitpack_uint(values->NumEntries0, 0, 7) |
      util_bitpack_uint(values->NumEntries1, 8, 15) |
      util_bitpack_uint(values->NumEntries2, 16, 23) |
      util_bitpack_uint(values->NumEntries3, 24, 31);
}

#define GFX30_3DSTATE_STENCIL_BUFFER_length      8
#define GFX30_3DSTATE_STENCIL_BUFFER_length_bias      2
#define GFX30_3DSTATE_STENCIL_BUFFER_header     \
   .DWordLength                         =      6,  \
   ._3DCommandSubOpcode                 =      6,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_STENCIL_BUFFER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SurfacePitch;
   bool                                 CornerTexelMode;
   bool                                 NullPageCoherencyEnable;
   bool                                 StencilWriteEnable;
   uint32_t                             SurfaceType;
#define SURFTYPE_2D                              1
#define SURFTYPE_CUBE                            3
#define SURFTYPE_NULL                            7
   __gen_address_type                   SurfaceBaseAddress;
   uint32_t                             Width;
   uint32_t                             Height;
   uint32_t                             MOCS;
   uint32_t                             MinimumArrayElement;
   uint32_t                             Depth;
   enum GFX30_UNIFIED_COMPRESSION_FORMAT CompressionFormat;
   uint32_t                             MipTailStartLOD;
   uint32_t                             TiledMode;
#define TILE64                                   1
#define TILE4                                    3
   uint32_t                             SurfaceQPitch;
   uint32_t                             SurfLOD;
   uint32_t                             RenderTargetViewExtent;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_STENCIL_BUFFER_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_STENCIL_BUFFER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SurfacePitch, 0, 16) |
      util_bitpack_uint(values->CornerTexelMode, 23, 23) |
      util_bitpack_uint(values->NullPageCoherencyEnable, 27, 27) |
      util_bitpack_uint(values->StencilWriteEnable, 28, 28) |
      util_bitpack_uint(values->SurfaceType, 29, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SurfaceBaseAddress, 0, 0, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->Width, 1, 14) |
      util_bitpack_uint(values->Height, 17, 30);

   dw[5] =
      util_bitpack_uint_nonzero(values->MOCS, 0, 6) |
      util_bitpack_uint(values->MinimumArrayElement, 8, 18) |
      util_bitpack_uint(values->Depth, 20, 30);

   dw[6] =
      util_bitpack_uint(values->CompressionFormat, 0, 3) |
      util_bitpack_uint(values->MipTailStartLOD, 26, 29) |
      util_bitpack_uint(values->TiledMode, 30, 31);

   dw[7] =
      util_bitpack_uint(values->SurfaceQPitch, 0, 14) |
      util_bitpack_uint(values->SurfLOD, 16, 19) |
      util_bitpack_uint(values->RenderTargetViewExtent, 21, 31);
}

#define GFX30_3DSTATE_STREAMOUT_length         5
#define GFX30_3DSTATE_STREAMOUT_length_bias      2
#define GFX30_3DSTATE_STREAMOUT_header          \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     30,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_STREAMOUT {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ForceRendering;
#define Resreved                                 1
#define Force_Off                                2
#define Force_on                                 3
   bool                                 SOStatisticsEnable;
   uint32_t                             ReorderMode;
#define LEADING                                  0
#define TRAILING                                 1
   uint32_t                             RenderStreamSelect;
   bool                                 RenderingDisable;
   bool                                 SOFunctionEnable;
   uint32_t                             Stream0VertexReadLength;
   uint32_t                             Stream0VertexReadOffset;
   uint32_t                             Stream1VertexReadLength;
   uint32_t                             Stream1VertexReadOffset;
   uint32_t                             Stream2VertexReadLength;
   uint32_t                             Stream2VertexReadOffset;
   uint32_t                             Stream3VertexReadLength;
   uint32_t                             Stream3VertexReadOffset;
   uint32_t                             Buffer0SurfacePitch;
   uint32_t                             Buffer1SurfacePitch;
   uint32_t                             Buffer2SurfacePitch;
   uint32_t                             Buffer3SurfacePitch;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_STREAMOUT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_3DSTATE_STREAMOUT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ForceRendering, 23, 24) |
      util_bitpack_uint(values->SOStatisticsEnable, 25, 25) |
      util_bitpack_uint(values->ReorderMode, 26, 26) |
      util_bitpack_uint(values->RenderStreamSelect, 27, 28) |
      util_bitpack_uint(values->RenderingDisable, 30, 30) |
      util_bitpack_uint(values->SOFunctionEnable, 31, 31);

   dw[2] =
      util_bitpack_uint(values->Stream0VertexReadLength, 0, 4) |
      util_bitpack_uint(values->Stream0VertexReadOffset, 5, 5) |
      util_bitpack_uint(values->Stream1VertexReadLength, 8, 12) |
      util_bitpack_uint(values->Stream1VertexReadOffset, 13, 13) |
      util_bitpack_uint(values->Stream2VertexReadLength, 16, 20) |
      util_bitpack_uint(values->Stream2VertexReadOffset, 21, 21) |
      util_bitpack_uint(values->Stream3VertexReadLength, 24, 28) |
      util_bitpack_uint(values->Stream3VertexReadOffset, 29, 29);

   dw[3] =
      util_bitpack_uint(values->Buffer0SurfacePitch, 0, 11) |
      util_bitpack_uint(values->Buffer1SurfacePitch, 16, 27);

   dw[4] =
      util_bitpack_uint(values->Buffer2SurfacePitch, 0, 11) |
      util_bitpack_uint(values->Buffer3SurfacePitch, 16, 27);
}

#define GFX30_3DSTATE_SUBSLICE_HASH_TABLE_length     14
#define GFX30_3DSTATE_SUBSLICE_HASH_TABLE_length_bias      2
#define GFX30_3DSTATE_SUBSLICE_HASH_TABLE_header\
   .DWordLength                         =     12,  \
   ._3DCommandSubOpcode                 =     31,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_SUBSLICE_HASH_TABLE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             SliceHashControl[8];
#define COMPUTED                                 0
#define UNBALANCED_TABLE_0                       1
#define TABLE_0                                  2
#define TABLE_1                                  3
   uint32_t                             SliceTableMode;
#define SINGLE_TABLE                             0
#define DUAL_TABLE                               1
   uint32_t                             TwoWayTableEntry[8][16];
   uint32_t                             ThreeWayTableEntry[8][16];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_SUBSLICE_HASH_TABLE_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_3DSTATE_SUBSLICE_HASH_TABLE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SliceHashControl[0], 0, 1) |
      util_bitpack_uint(values->SliceHashControl[1], 2, 3) |
      util_bitpack_uint(values->SliceHashControl[2], 4, 5) |
      util_bitpack_uint(values->SliceHashControl[3], 6, 7) |
      util_bitpack_uint(values->SliceHashControl[4], 8, 9) |
      util_bitpack_uint(values->SliceHashControl[5], 10, 11) |
      util_bitpack_uint(values->SliceHashControl[6], 12, 13) |
      util_bitpack_uint(values->SliceHashControl[7], 14, 15) |
      util_bitpack_uint(values->SliceTableMode, 30, 31);

   dw[2] =
      util_bitpack_uint(values->TwoWayTableEntry[0][0], 0, 0) |
      util_bitpack_uint(values->TwoWayTableEntry[0][1], 1, 1) |
      util_bitpack_uint(values->TwoWayTableEntry[0][2], 2, 2) |
      util_bitpack_uint(values->TwoWayTableEntry[0][3], 3, 3) |
      util_bitpack_uint(values->TwoWayTableEntry[0][4], 4, 4) |
      util_bitpack_uint(values->TwoWayTableEntry[0][5], 5, 5) |
      util_bitpack_uint(values->TwoWayTableEntry[0][6], 6, 6) |
      util_bitpack_uint(values->TwoWayTableEntry[0][7], 7, 7) |
      util_bitpack_uint(values->TwoWayTableEntry[0][8], 8, 8) |
      util_bitpack_uint(values->TwoWayTableEntry[0][9], 9, 9) |
      util_bitpack_uint(values->TwoWayTableEntry[0][10], 10, 10) |
      util_bitpack_uint(values->TwoWayTableEntry[0][11], 11, 11) |
      util_bitpack_uint(values->TwoWayTableEntry[0][12], 12, 12) |
      util_bitpack_uint(values->TwoWayTableEntry[0][13], 13, 13) |
      util_bitpack_uint(values->TwoWayTableEntry[0][14], 14, 14) |
      util_bitpack_uint(values->TwoWayTableEntry[0][15], 15, 15) |
      util_bitpack_uint(values->TwoWayTableEntry[1][0], 16, 16) |
      util_bitpack_uint(values->TwoWayTableEntry[1][1], 17, 17) |
      util_bitpack_uint(values->TwoWayTableEntry[1][2], 18, 18) |
      util_bitpack_uint(values->TwoWayTableEntry[1][3], 19, 19) |
      util_bitpack_uint(values->TwoWayTableEntry[1][4], 20, 20) |
      util_bitpack_uint(values->TwoWayTableEntry[1][5], 21, 21) |
      util_bitpack_uint(values->TwoWayTableEntry[1][6], 22, 22) |
      util_bitpack_uint(values->TwoWayTableEntry[1][7], 23, 23) |
      util_bitpack_uint(values->TwoWayTableEntry[1][8], 24, 24) |
      util_bitpack_uint(values->TwoWayTableEntry[1][9], 25, 25) |
      util_bitpack_uint(values->TwoWayTableEntry[1][10], 26, 26) |
      util_bitpack_uint(values->TwoWayTableEntry[1][11], 27, 27) |
      util_bitpack_uint(values->TwoWayTableEntry[1][12], 28, 28) |
      util_bitpack_uint(values->TwoWayTableEntry[1][13], 29, 29) |
      util_bitpack_uint(values->TwoWayTableEntry[1][14], 30, 30) |
      util_bitpack_uint(values->TwoWayTableEntry[1][15], 31, 31);

   dw[3] =
      util_bitpack_uint(values->TwoWayTableEntry[2][0], 0, 0) |
      util_bitpack_uint(values->TwoWayTableEntry[2][1], 1, 1) |
      util_bitpack_uint(values->TwoWayTableEntry[2][2], 2, 2) |
      util_bitpack_uint(values->TwoWayTableEntry[2][3], 3, 3) |
      util_bitpack_uint(values->TwoWayTableEntry[2][4], 4, 4) |
      util_bitpack_uint(values->TwoWayTableEntry[2][5], 5, 5) |
      util_bitpack_uint(values->TwoWayTableEntry[2][6], 6, 6) |
      util_bitpack_uint(values->TwoWayTableEntry[2][7], 7, 7) |
      util_bitpack_uint(values->TwoWayTableEntry[2][8], 8, 8) |
      util_bitpack_uint(values->TwoWayTableEntry[2][9], 9, 9) |
      util_bitpack_uint(values->TwoWayTableEntry[2][10], 10, 10) |
      util_bitpack_uint(values->TwoWayTableEntry[2][11], 11, 11) |
      util_bitpack_uint(values->TwoWayTableEntry[2][12], 12, 12) |
      util_bitpack_uint(values->TwoWayTableEntry[2][13], 13, 13) |
      util_bitpack_uint(values->TwoWayTableEntry[2][14], 14, 14) |
      util_bitpack_uint(values->TwoWayTableEntry[2][15], 15, 15) |
      util_bitpack_uint(values->TwoWayTableEntry[3][0], 16, 16) |
      util_bitpack_uint(values->TwoWayTableEntry[3][1], 17, 17) |
      util_bitpack_uint(values->TwoWayTableEntry[3][2], 18, 18) |
      util_bitpack_uint(values->TwoWayTableEntry[3][3], 19, 19) |
      util_bitpack_uint(values->TwoWayTableEntry[3][4], 20, 20) |
      util_bitpack_uint(values->TwoWayTableEntry[3][5], 21, 21) |
      util_bitpack_uint(values->TwoWayTableEntry[3][6], 22, 22) |
      util_bitpack_uint(values->TwoWayTableEntry[3][7], 23, 23) |
      util_bitpack_uint(values->TwoWayTableEntry[3][8], 24, 24) |
      util_bitpack_uint(values->TwoWayTableEntry[3][9], 25, 25) |
      util_bitpack_uint(values->TwoWayTableEntry[3][10], 26, 26) |
      util_bitpack_uint(values->TwoWayTableEntry[3][11], 27, 27) |
      util_bitpack_uint(values->TwoWayTableEntry[3][12], 28, 28) |
      util_bitpack_uint(values->TwoWayTableEntry[3][13], 29, 29) |
      util_bitpack_uint(values->TwoWayTableEntry[3][14], 30, 30) |
      util_bitpack_uint(values->TwoWayTableEntry[3][15], 31, 31);

   dw[4] =
      util_bitpack_uint(values->TwoWayTableEntry[4][0], 0, 0) |
      util_bitpack_uint(values->TwoWayTableEntry[4][1], 1, 1) |
      util_bitpack_uint(values->TwoWayTableEntry[4][2], 2, 2) |
      util_bitpack_uint(values->TwoWayTableEntry[4][3], 3, 3) |
      util_bitpack_uint(values->TwoWayTableEntry[4][4], 4, 4) |
      util_bitpack_uint(values->TwoWayTableEntry[4][5], 5, 5) |
      util_bitpack_uint(values->TwoWayTableEntry[4][6], 6, 6) |
      util_bitpack_uint(values->TwoWayTableEntry[4][7], 7, 7) |
      util_bitpack_uint(values->TwoWayTableEntry[4][8], 8, 8) |
      util_bitpack_uint(values->TwoWayTableEntry[4][9], 9, 9) |
      util_bitpack_uint(values->TwoWayTableEntry[4][10], 10, 10) |
      util_bitpack_uint(values->TwoWayTableEntry[4][11], 11, 11) |
      util_bitpack_uint(values->TwoWayTableEntry[4][12], 12, 12) |
      util_bitpack_uint(values->TwoWayTableEntry[4][13], 13, 13) |
      util_bitpack_uint(values->TwoWayTableEntry[4][14], 14, 14) |
      util_bitpack_uint(values->TwoWayTableEntry[4][15], 15, 15) |
      util_bitpack_uint(values->TwoWayTableEntry[5][0], 16, 16) |
      util_bitpack_uint(values->TwoWayTableEntry[5][1], 17, 17) |
      util_bitpack_uint(values->TwoWayTableEntry[5][2], 18, 18) |
      util_bitpack_uint(values->TwoWayTableEntry[5][3], 19, 19) |
      util_bitpack_uint(values->TwoWayTableEntry[5][4], 20, 20) |
      util_bitpack_uint(values->TwoWayTableEntry[5][5], 21, 21) |
      util_bitpack_uint(values->TwoWayTableEntry[5][6], 22, 22) |
      util_bitpack_uint(values->TwoWayTableEntry[5][7], 23, 23) |
      util_bitpack_uint(values->TwoWayTableEntry[5][8], 24, 24) |
      util_bitpack_uint(values->TwoWayTableEntry[5][9], 25, 25) |
      util_bitpack_uint(values->TwoWayTableEntry[5][10], 26, 26) |
      util_bitpack_uint(values->TwoWayTableEntry[5][11], 27, 27) |
      util_bitpack_uint(values->TwoWayTableEntry[5][12], 28, 28) |
      util_bitpack_uint(values->TwoWayTableEntry[5][13], 29, 29) |
      util_bitpack_uint(values->TwoWayTableEntry[5][14], 30, 30) |
      util_bitpack_uint(values->TwoWayTableEntry[5][15], 31, 31);

   dw[5] =
      util_bitpack_uint(values->TwoWayTableEntry[6][0], 0, 0) |
      util_bitpack_uint(values->TwoWayTableEntry[6][1], 1, 1) |
      util_bitpack_uint(values->TwoWayTableEntry[6][2], 2, 2) |
      util_bitpack_uint(values->TwoWayTableEntry[6][3], 3, 3) |
      util_bitpack_uint(values->TwoWayTableEntry[6][4], 4, 4) |
      util_bitpack_uint(values->TwoWayTableEntry[6][5], 5, 5) |
      util_bitpack_uint(values->TwoWayTableEntry[6][6], 6, 6) |
      util_bitpack_uint(values->TwoWayTableEntry[6][7], 7, 7) |
      util_bitpack_uint(values->TwoWayTableEntry[6][8], 8, 8) |
      util_bitpack_uint(values->TwoWayTableEntry[6][9], 9, 9) |
      util_bitpack_uint(values->TwoWayTableEntry[6][10], 10, 10) |
      util_bitpack_uint(values->TwoWayTableEntry[6][11], 11, 11) |
      util_bitpack_uint(values->TwoWayTableEntry[6][12], 12, 12) |
      util_bitpack_uint(values->TwoWayTableEntry[6][13], 13, 13) |
      util_bitpack_uint(values->TwoWayTableEntry[6][14], 14, 14) |
      util_bitpack_uint(values->TwoWayTableEntry[6][15], 15, 15) |
      util_bitpack_uint(values->TwoWayTableEntry[7][0], 16, 16) |
      util_bitpack_uint(values->TwoWayTableEntry[7][1], 17, 17) |
      util_bitpack_uint(values->TwoWayTableEntry[7][2], 18, 18) |
      util_bitpack_uint(values->TwoWayTableEntry[7][3], 19, 19) |
      util_bitpack_uint(values->TwoWayTableEntry[7][4], 20, 20) |
      util_bitpack_uint(values->TwoWayTableEntry[7][5], 21, 21) |
      util_bitpack_uint(values->TwoWayTableEntry[7][6], 22, 22) |
      util_bitpack_uint(values->TwoWayTableEntry[7][7], 23, 23) |
      util_bitpack_uint(values->TwoWayTableEntry[7][8], 24, 24) |
      util_bitpack_uint(values->TwoWayTableEntry[7][9], 25, 25) |
      util_bitpack_uint(values->TwoWayTableEntry[7][10], 26, 26) |
      util_bitpack_uint(values->TwoWayTableEntry[7][11], 27, 27) |
      util_bitpack_uint(values->TwoWayTableEntry[7][12], 28, 28) |
      util_bitpack_uint(values->TwoWayTableEntry[7][13], 29, 29) |
      util_bitpack_uint(values->TwoWayTableEntry[7][14], 30, 30) |
      util_bitpack_uint(values->TwoWayTableEntry[7][15], 31, 31);

   dw[6] =
      util_bitpack_uint(values->ThreeWayTableEntry[0][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[0][15], 30, 31);

   dw[7] =
      util_bitpack_uint(values->ThreeWayTableEntry[1][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[1][15], 30, 31);

   dw[8] =
      util_bitpack_uint(values->ThreeWayTableEntry[2][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[2][15], 30, 31);

   dw[9] =
      util_bitpack_uint(values->ThreeWayTableEntry[3][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[3][15], 30, 31);

   dw[10] =
      util_bitpack_uint(values->ThreeWayTableEntry[4][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[4][15], 30, 31);

   dw[11] =
      util_bitpack_uint(values->ThreeWayTableEntry[5][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[5][15], 30, 31);

   dw[12] =
      util_bitpack_uint(values->ThreeWayTableEntry[6][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[6][15], 30, 31);

   dw[13] =
      util_bitpack_uint(values->ThreeWayTableEntry[7][0], 0, 1) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][1], 2, 3) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][2], 4, 5) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][3], 6, 7) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][4], 8, 9) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][5], 10, 11) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][6], 12, 13) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][7], 14, 15) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][8], 16, 17) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][9], 18, 19) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][10], 20, 21) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][11], 22, 23) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][12], 24, 25) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][13], 26, 27) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][14], 28, 29) |
      util_bitpack_uint(values->ThreeWayTableEntry[7][15], 30, 31);
}

#define GFX30_3DSTATE_TASK_CONTROL_length      3
#define GFX30_3DSTATE_TASK_CONTROL_length_bias      2
#define GFX30_3DSTATE_TASK_CONTROL_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =    124,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TASK_CONTROL {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MaximumNumberofThreadGroups;
   uint32_t                             ThreadDispatchPriority;
#define High                                     1
   bool                                 FusedEUDispatch;
   bool                                 StatisticsEnable;
   uint32_t                             TaskShaderEnable;
   uint32_t                             ScratchSpaceBuffer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TASK_CONTROL_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_TASK_CONTROL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MaximumNumberofThreadGroups, 0, 8) |
      util_bitpack_uint(values->ThreadDispatchPriority, 28, 28) |
      util_bitpack_uint(values->FusedEUDispatch, 29, 29) |
      util_bitpack_uint(values->StatisticsEnable, 30, 30) |
      util_bitpack_uint(values->TaskShaderEnable, 31, 31);

   dw[2] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);
}

#define GFX30_3DSTATE_TASK_REDISTRIB_length      2
#define GFX30_3DSTATE_TASK_REDISTRIB_length_bias      2
#define GFX30_3DSTATE_TASK_REDISTRIB_header     \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =    121,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TASK_REDISTRIB {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             TaskRedistributionMode;
#define TASKREDISTRIB_OFF                        0
#define TASKREDISTRIB_RR_STRICT                  1
#define TASKREDISTRIB_RR_FREE                    2
   uint32_t                             TaskRedistributionLevel;
#define TASKREDISTRIB_TASK                       0
#define TASKREDISTRIB_BOM                        1
   uint32_t                             TargetMeshBatchSize;
   uint32_t                             SmallTaskThreshold;
   uint32_t                             LocalBOTAccumulatorThreshold;
#define MULTIPLIER_0                             0
#define MULTIPLIER_1                             1
#define MULTIPLIER_2                             2
#define MULTIPLIER_4                             3
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TASK_REDISTRIB_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_TASK_REDISTRIB * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TaskRedistributionMode, 0, 1) |
      util_bitpack_uint(values->TaskRedistributionLevel, 2, 3) |
      util_bitpack_uint(values->TargetMeshBatchSize, 4, 7) |
      util_bitpack_uint(values->SmallTaskThreshold, 10, 12) |
      util_bitpack_uint(values->LocalBOTAccumulatorThreshold, 16, 17);
}

#define GFX30_3DSTATE_TASK_SHADER_length       7
#define GFX30_3DSTATE_TASK_SHADER_length_bias      2
#define GFX30_3DSTATE_TASK_SHADER_header        \
   .DWordLength                         =      5,  \
   ._3DCommandSubOpcode                 =    125,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TASK_SHADER {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             KernelStartPointer;
   uint32_t                             LocalXMaximum;
   bool                                 SoftwareExceptionEnable;
   bool                                 MaskStackExceptionEnable;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             SingleProgramFlow;
   uint32_t                             DenormMode;
#define Ftz                                      0
#define SetByKernel                              1
   uint32_t                             RegistersPerThread;
   uint32_t                             NumberofThreadsinGPGPUThreadGroup;
   uint32_t                             SharedLocalMemorySize;
   bool                                 BarrierEnable;
   uint32_t                             RoundingMode;
#define RTNE                                     0
#define RU                                       1
#define RD                                       2
#define RTZ                                      3
   uint32_t                             PreferredSLMAllocationSize;
   uint32_t                             NumberofBarriers;
   uint32_t                             IndirectDataLength;
   bool                                 L3PrefetchDisable;
   bool                                 EmitLocalIDX;
   bool                                 EmitInlineParameter;
   bool                                 SystolicModeEnable;
   bool                                 AccessesUAV;
   uint32_t                             XP0Required;
   uint32_t                             MessageSIMD;
#define SIMD16                                   1
#define SIMD32                                   2
   uint32_t                             SIMDSize;
#define SIMD16                                   1
#define SIMD32                                   2
   uint32_t                             ExecutionMask;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TASK_SHADER_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_TASK_SHADER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->LocalXMaximum, 0, 9) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 10, 10) |
      util_bitpack_uint(values->MaskStackExceptionEnable, 11, 11) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->SingleProgramFlow, 18, 18) |
      util_bitpack_uint(values->DenormMode, 19, 19) |
      util_bitpack_uint(values->RegistersPerThread, 20, 25);

   dw[4] =
      util_bitpack_uint(values->NumberofThreadsinGPGPUThreadGroup, 0, 9) |
      util_bitpack_uint(values->SharedLocalMemorySize, 16, 20) |
      util_bitpack_uint(values->BarrierEnable, 21, 21) |
      util_bitpack_uint(values->RoundingMode, 22, 23) |
      util_bitpack_uint(values->PreferredSLMAllocationSize, 24, 27) |
      util_bitpack_uint(values->NumberofBarriers, 28, 30);

   dw[5] =
      util_bitpack_uint(values->IndirectDataLength, 0, 16) |
      util_bitpack_uint(values->L3PrefetchDisable, 17, 17) |
      util_bitpack_uint(values->EmitLocalIDX, 18, 18) |
      util_bitpack_uint(values->EmitInlineParameter, 19, 19) |
      util_bitpack_uint(values->SystolicModeEnable, 20, 20) |
      util_bitpack_uint(values->AccessesUAV, 21, 21) |
      util_bitpack_uint(values->XP0Required, 22, 22) |
      util_bitpack_uint(values->MessageSIMD, 28, 29) |
      util_bitpack_uint(values->SIMDSize, 30, 31);

   dw[6] =
      util_bitpack_uint(values->ExecutionMask, 0, 31);
}

#define GFX30_3DSTATE_TASK_SHADER_DATA_length     10
#define GFX30_3DSTATE_TASK_SHADER_DATA_length_bias      2
#define GFX30_3DSTATE_TASK_SHADER_DATA_header   \
   .DWordLength                         =      8,  \
   ._3DCommandSubOpcode                 =    126,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TASK_SHADER_DATA {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             IndirectDataStartAddress;
   uint32_t                             InlineData[8];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TASK_SHADER_DATA_pack(__attribute__((unused)) __gen_user_data *data,
                                    __attribute__((unused)) void * restrict dst,
                                    __attribute__((unused)) const struct GFX30_3DSTATE_TASK_SHADER_DATA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->IndirectDataStartAddress, 6, 31);

   dw[2] =
      util_bitpack_uint(values->InlineData[0], 0, 31);

   dw[3] =
      util_bitpack_uint(values->InlineData[1], 0, 31);

   dw[4] =
      util_bitpack_uint(values->InlineData[2], 0, 31);

   dw[5] =
      util_bitpack_uint(values->InlineData[3], 0, 31);

   dw[6] =
      util_bitpack_uint(values->InlineData[4], 0, 31);

   dw[7] =
      util_bitpack_uint(values->InlineData[5], 0, 31);

   dw[8] =
      util_bitpack_uint(values->InlineData[6], 0, 31);

   dw[9] =
      util_bitpack_uint(values->InlineData[7], 0, 31);
}

#define GFX30_3DSTATE_TBIMR_TILE_PASS_INFO_length      4
#define GFX30_3DSTATE_TBIMR_TILE_PASS_INFO_length_bias      2
#define GFX30_3DSTATE_TBIMR_TILE_PASS_INFO_header\
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =    110,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TBIMR_TILE_PASS_INFO {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             TileRectangleHeight;
   uint32_t                             TileRectangleWidth;
   uint32_t                             VerticalTileCount;
   uint32_t                             HorizontalTileCount;
   uint32_t                             WalkPattern;
   uint32_t                             TileBoxCheck;
   uint32_t                             TBIMRBatchSize;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TBIMR_TILE_PASS_INFO_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_3DSTATE_TBIMR_TILE_PASS_INFO * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TileRectangleHeight, 0, 14) |
      util_bitpack_uint(values->TileRectangleWidth, 16, 30);

   dw[2] =
      util_bitpack_uint(values->VerticalTileCount, 0, 15) |
      util_bitpack_uint(values->HorizontalTileCount, 16, 31);

   dw[3] =
      util_bitpack_uint(values->WalkPattern, 0, 0) |
      util_bitpack_uint(values->TileBoxCheck, 2, 2) |
      util_bitpack_uint(values->TBIMRBatchSize, 3, 5);
}

#define GFX30_3DSTATE_TE_length                5
#define GFX30_3DSTATE_TE_length_bias           2
#define GFX30_3DSTATE_TE_header                 \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     28,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_TE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 TEEnable;
   uint32_t                             TEMode;
#define HW_TESS                                  0
#define FORCE_MIN                                3
   uint32_t                             TEDomain;
#define QUAD                                     0
#define TRI                                      1
#define ISOLINE                                  2
   uint32_t                             OutputTopology;
#define OUTPUT_POINT                             0
#define OUTPUT_LINE                              1
#define OUTPUT_TRI_CW                            2
#define OUTPUT_TRI_CCW                           3
   uint32_t                             NumberOfRegionsPerPatch;
   uint32_t                             Partitioning;
#define INTEGER                                  0
#define ODD_FRACTIONAL                           1
#define EVEN_FRACTIONAL                          2
#define POW2                                     3
   uint32_t                             TessellationDistributionMode;
#define TEDMODE_OFF                              0
#define TEDMODE_RR_STRICT                        1
#define TEDMODE_RR_FREE                          2
   uint32_t                             TessellationDistributionLevel;
#define TEDLEVEL_PATCH                           0
#define TEDLEVEL_REGION                          1
   bool                                 TessellationScaleFactorEnable;
   uint32_t                             TessellationFactorFormat;
#define FLOAT32                                  0
#define FLOAT16                                  1
   uint32_t                             PatchHeaderLayout;
#define LEGACY                                   0
#define REVERSED                                 2
#define REVERSED_TRI_INSIDE_SEPARATE             3
   uint32_t                             SmallPatchThreshold;
#define SPT_8_TRIANGLES                          0
#define SPT_16_TRIANGLES                         1
#define SPT_32_TRIANGLES                         2
#define SPT_64_TRIANGLES                         3
   uint32_t                             TargetBlockSize;
#define _64_TRIANGLES                            0
#define _96_TRIANGLES                            1
#define _128_TRIANGLES                           2
#define _192_TRIANGLES                           3
#define _256_TRIANGLES                           4
#define _384_TRIANGLES                           5
#define _512_TRIANGLES                           6
#define _768_TRIANGLES                           7
#define _1K_TRIANGLES                            8
#define _15K_TRIANGLES                           9
#define _2K_TRIANGLES                            10
#define _3K_TRIANGLES                            11
#define _4K_TRIANGLES                            12
#define _6K_TRIANGLES                            13
#define _8K_TRIANGLES                            14
#define _12K_TRIANGLES                           15
   uint32_t                             LocalBOPAccumulatorThreshold;
#define MULTIPLIER_0                             0
#define MULTIPLIER_1                             1
#define MULTIPLIER_2                             2
#define MULTIPLIER_4                             3
   float                                MaximumTessellationFactorOdd;
   float                                MaximumTessellationFactorNotOdd;
   float                                TessellationScaleFactor;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_TE_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_TE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TEEnable, 0, 0) |
      util_bitpack_uint(values->TEMode, 1, 2) |
      util_bitpack_uint(values->TEDomain, 4, 5) |
      util_bitpack_uint(values->OutputTopology, 8, 9) |
      util_bitpack_uint(values->NumberOfRegionsPerPatch, 10, 11) |
      util_bitpack_uint(values->Partitioning, 12, 13) |
      util_bitpack_uint(values->TessellationDistributionMode, 14, 15) |
      util_bitpack_uint(values->TessellationDistributionLevel, 17, 18) |
      util_bitpack_uint(values->TessellationScaleFactorEnable, 19, 19) |
      util_bitpack_uint(values->TessellationFactorFormat, 20, 20) |
      util_bitpack_uint(values->PatchHeaderLayout, 22, 23) |
      util_bitpack_uint(values->SmallPatchThreshold, 24, 25) |
      util_bitpack_uint(values->TargetBlockSize, 26, 29) |
      util_bitpack_uint(values->LocalBOPAccumulatorThreshold, 30, 31);

   dw[2] =
      util_bitpack_float(values->MaximumTessellationFactorOdd);

   dw[3] =
      util_bitpack_float(values->MaximumTessellationFactorNotOdd);

   dw[4] =
      util_bitpack_float(values->TessellationScaleFactor);
}

#define GFX30_3DSTATE_URB_ALLOC_DS_length      3
#define GFX30_3DSTATE_URB_ALLOC_DS_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_DS_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     90,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             DSURBEntryAllocationSize;
   uint32_t                             DSURBStartingAddressSlice0;
   uint32_t                             DSURBStartingAddressSliceN;
   uint32_t                             DSNumberofURBEntriesSlice0;
   uint32_t                             DSNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_DS_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DSURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->DSURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->DSURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->DSNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->DSNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_ALLOC_GS_length      3
#define GFX30_3DSTATE_URB_ALLOC_GS_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_GS_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     91,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             GSURBEntryAllocationSize;
   uint32_t                             GSURBStartingAddressSlice0;
   uint32_t                             GSURBStartingAddressSliceN;
   uint32_t                             GSNumberofURBEntriesSlice0;
   uint32_t                             GSNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_GS_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->GSURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->GSURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->GSURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->GSNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->GSNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_ALLOC_HS_length      3
#define GFX30_3DSTATE_URB_ALLOC_HS_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_HS_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     89,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             HSURBEntryAllocationSize;
   uint32_t                             HSURBStartingAddressSlice0;
   uint32_t                             HSURBStartingAddressSliceN;
   uint32_t                             HSNumberofURBEntriesSlice0;
   uint32_t                             HSNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_HS_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->HSURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->HSURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->HSURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->HSNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->HSNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_ALLOC_MESH_length      3
#define GFX30_3DSTATE_URB_ALLOC_MESH_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_MESH_header     \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =    127,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_MESH {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MESHURBEntryAllocationSize;
   uint32_t                             MESHURBStartingAddressSlice0;
   uint32_t                             MESHURBStartingAddressSliceN;
   uint32_t                             MESHNumberofURBEntriesSlice0;
   uint32_t                             MESHNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_MESH_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_MESH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MESHURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->MESHURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->MESHURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->MESHNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->MESHNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_ALLOC_TASK_length      3
#define GFX30_3DSTATE_URB_ALLOC_TASK_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_TASK_header     \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =    128,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_TASK {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             TASKURBEntryAllocationSize;
   uint32_t                             TASKURBStartingAddressSlice0;
   uint32_t                             TASKURBStartingAddressSliceN;
   uint32_t                             TASKNumberofURBEntriesSlice0;
   uint32_t                             TASKNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_TASK_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_TASK * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TASKURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->TASKURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->TASKURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->TASKNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->TASKNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_ALLOC_VS_length      3
#define GFX30_3DSTATE_URB_ALLOC_VS_length_bias      2
#define GFX30_3DSTATE_URB_ALLOC_VS_header       \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     88,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_ALLOC_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             VSURBEntryAllocationSize;
   uint32_t                             VSURBStartingAddressSlice0;
   uint32_t                             VSURBStartingAddressSliceN;
   uint32_t                             VSNumberofURBEntriesSlice0;
   uint32_t                             VSNumberofURBEntriesSliceN;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_ALLOC_VS_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_URB_ALLOC_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->VSURBEntryAllocationSize, 0, 9) |
      util_bitpack_uint(values->VSURBStartingAddressSlice0, 10, 17) |
      util_bitpack_uint(values->VSURBStartingAddressSliceN, 21, 28);

   dw[2] =
      util_bitpack_uint(values->VSNumberofURBEntriesSlice0, 0, 15) |
      util_bitpack_uint(values->VSNumberofURBEntriesSliceN, 16, 31);
}

#define GFX30_3DSTATE_URB_CLEAR_length         2
#define GFX30_3DSTATE_URB_CLEAR_length_bias      2
#define GFX30_3DSTATE_URB_CLEAR_header          \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     29,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_CLEAR {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             URBAddress;
   uint32_t                             URBClearLength;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_CLEAR_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_3DSTATE_URB_CLEAR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->URBAddress, 0, 14) |
      util_bitpack_uint(values->URBClearLength, 16, 29);
}

#define GFX30_3DSTATE_URB_DS_length            2
#define GFX30_3DSTATE_URB_DS_length_bias       2
#define GFX30_3DSTATE_URB_DS_header             \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     50,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_DS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             DSNumberofURBEntries;
   uint32_t                             DSURBEntryAllocationSize;
   uint32_t                             DSURBStartingAddress;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_DS_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_3DSTATE_URB_DS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DSNumberofURBEntries, 0, 15) |
      util_bitpack_uint(values->DSURBEntryAllocationSize, 16, 24) |
      util_bitpack_uint(values->DSURBStartingAddress, 25, 31);
}

#define GFX30_3DSTATE_URB_GS_length            2
#define GFX30_3DSTATE_URB_GS_length_bias       2
#define GFX30_3DSTATE_URB_GS_header             \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     51,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_GS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             GSNumberofURBEntries;
   uint32_t                             GSURBEntryAllocationSize;
   uint32_t                             GSURBStartingAddress;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_GS_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_3DSTATE_URB_GS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->GSNumberofURBEntries, 0, 15) |
      util_bitpack_uint(values->GSURBEntryAllocationSize, 16, 24) |
      util_bitpack_uint(values->GSURBStartingAddress, 25, 31);
}

#define GFX30_3DSTATE_URB_HS_length            2
#define GFX30_3DSTATE_URB_HS_length_bias       2
#define GFX30_3DSTATE_URB_HS_header             \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     49,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_HS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             HSNumberofURBEntries;
   uint32_t                             HSURBEntryAllocationSize;
   uint32_t                             HSURBStartingAddress;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_HS_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_3DSTATE_URB_HS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->HSNumberofURBEntries, 0, 15) |
      util_bitpack_uint(values->HSURBEntryAllocationSize, 16, 24) |
      util_bitpack_uint(values->HSURBStartingAddress, 25, 31);
}

#define GFX30_3DSTATE_URB_VS_length            2
#define GFX30_3DSTATE_URB_VS_length_bias       2
#define GFX30_3DSTATE_URB_VS_header             \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     48,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_URB_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             VSNumberofURBEntries;
   uint32_t                             VSURBEntryAllocationSize;
   uint32_t                             VSURBStartingAddress;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_URB_VS_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_3DSTATE_URB_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->VSNumberofURBEntries, 0, 15) |
      util_bitpack_uint(values->VSURBEntryAllocationSize, 16, 24) |
      util_bitpack_uint(values->VSURBStartingAddress, 25, 31);
}

#define GFX30_3DSTATE_VERTEX_BUFFERS_length_bias      2
#define GFX30_3DSTATE_VERTEX_BUFFERS_header     \
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =      8,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VERTEX_BUFFERS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VERTEX_BUFFERS_pack(__attribute__((unused)) __gen_user_data *data,
                                  __attribute__((unused)) void * restrict dst,
                                  __attribute__((unused)) const struct GFX30_3DSTATE_VERTEX_BUFFERS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_3DSTATE_VERTEX_ELEMENTS_length_bias      2
#define GFX30_3DSTATE_VERTEX_ELEMENTS_header    \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =      9,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VERTEX_ELEMENTS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VERTEX_ELEMENTS_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_3DSTATE_VERTEX_ELEMENTS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_3DSTATE_VF_length                2
#define GFX30_3DSTATE_VF_length_bias           2
#define GFX30_3DSTATE_VF_header                 \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     12,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF {
   uint32_t                             DWordLength;
   bool                                 IndexedDrawCutIndexEnable;
   bool                                 ComponentPackingEnable;
   bool                                 SequentialDrawCutIndexEnable;
   bool                                 VertexIDOffsetEnable;
   bool                                 GeometryDistributionEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             CutIndex;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_VF * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->IndexedDrawCutIndexEnable, 8, 8) |
      util_bitpack_uint(values->ComponentPackingEnable, 9, 9) |
      util_bitpack_uint(values->SequentialDrawCutIndexEnable, 10, 10) |
      util_bitpack_uint(values->VertexIDOffsetEnable, 11, 11) |
      util_bitpack_uint(values->GeometryDistributionEnable, 12, 12) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->CutIndex, 0, 31);
}

#define GFX30_3DSTATE_VFG_length               4
#define GFX30_3DSTATE_VFG_length_bias          2
#define GFX30_3DSTATE_VFG_header                \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     87,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VFG {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             DistributionGranularity;
#define BatchLevelGranularity                    0
#define InstanceLevelGranularity                 1
#define DrawLevelGranularity                     2
   bool                                 GranularityThresholdDisable;
   bool                                 ListCutIndexEnable;
   uint32_t                             DistributionMode;
#define RR_STRICT                                0
#define RR_FREE                                  1
   uint32_t                             List1BatchSizeScale;
   uint32_t                             List2BatchSizeScale;
   uint32_t                             List3BatchSizeScale;
   uint32_t                             ListNBatchSizeScale;
   uint32_t                             StripBatchSizeScale;
   uint32_t                             PatchBatchSizeScale;
   uint32_t                             PatchBatchSizeMultiplier;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VFG_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_3DSTATE_VFG * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DistributionGranularity, 0, 1) |
      util_bitpack_uint(values->GranularityThresholdDisable, 2, 2) |
      util_bitpack_uint(values->ListCutIndexEnable, 3, 3) |
      util_bitpack_uint(values->DistributionMode, 4, 4);

   dw[2] =
      util_bitpack_uint(values->List1BatchSizeScale, 0, 2) |
      util_bitpack_uint(values->List2BatchSizeScale, 8, 10) |
      util_bitpack_uint(values->List3BatchSizeScale, 16, 18) |
      util_bitpack_uint(values->ListNBatchSizeScale, 24, 26);

   dw[3] =
      util_bitpack_uint(values->StripBatchSizeScale, 0, 2) |
      util_bitpack_uint(values->PatchBatchSizeScale, 8, 10) |
      util_bitpack_uint(values->PatchBatchSizeMultiplier, 16, 20);
}

#define GFX30_3DSTATE_VF_COMPONENT_PACKING_length      5
#define GFX30_3DSTATE_VF_COMPONENT_PACKING_length_bias      2
#define GFX30_3DSTATE_VF_COMPONENT_PACKING_header\
   .DWordLength                         =      3,  \
   ._3DCommandSubOpcode                 =     85,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_COMPONENT_PACKING {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             VertexElementEnables[32];
   uint32_t                             VertexElementEnablesDW[4];
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_COMPONENT_PACKING_pack(__attribute__((unused)) __gen_user_data *data,
                                        __attribute__((unused)) void * restrict dst,
                                        __attribute__((unused)) const struct GFX30_3DSTATE_VF_COMPONENT_PACKING * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->VertexElementEnables[0], 0, 3) |
      util_bitpack_uint(values->VertexElementEnables[1], 4, 7) |
      util_bitpack_uint(values->VertexElementEnables[2], 8, 11) |
      util_bitpack_uint(values->VertexElementEnables[3], 12, 15) |
      util_bitpack_uint(values->VertexElementEnables[4], 16, 19) |
      util_bitpack_uint(values->VertexElementEnables[5], 20, 23) |
      util_bitpack_uint(values->VertexElementEnables[6], 24, 27) |
      util_bitpack_uint(values->VertexElementEnables[7], 28, 31) |
      util_bitpack_uint(values->VertexElementEnablesDW[0], 0, 31);

   dw[2] =
      util_bitpack_uint(values->VertexElementEnables[8], 0, 3) |
      util_bitpack_uint(values->VertexElementEnables[9], 4, 7) |
      util_bitpack_uint(values->VertexElementEnables[10], 8, 11) |
      util_bitpack_uint(values->VertexElementEnables[11], 12, 15) |
      util_bitpack_uint(values->VertexElementEnables[12], 16, 19) |
      util_bitpack_uint(values->VertexElementEnables[13], 20, 23) |
      util_bitpack_uint(values->VertexElementEnables[14], 24, 27) |
      util_bitpack_uint(values->VertexElementEnables[15], 28, 31) |
      util_bitpack_uint(values->VertexElementEnablesDW[1], 0, 31);

   dw[3] =
      util_bitpack_uint(values->VertexElementEnables[16], 0, 3) |
      util_bitpack_uint(values->VertexElementEnables[17], 4, 7) |
      util_bitpack_uint(values->VertexElementEnables[18], 8, 11) |
      util_bitpack_uint(values->VertexElementEnables[19], 12, 15) |
      util_bitpack_uint(values->VertexElementEnables[20], 16, 19) |
      util_bitpack_uint(values->VertexElementEnables[21], 20, 23) |
      util_bitpack_uint(values->VertexElementEnables[22], 24, 27) |
      util_bitpack_uint(values->VertexElementEnables[23], 28, 31) |
      util_bitpack_uint(values->VertexElementEnablesDW[2], 0, 31);

   dw[4] =
      util_bitpack_uint(values->VertexElementEnables[24], 0, 3) |
      util_bitpack_uint(values->VertexElementEnables[25], 4, 7) |
      util_bitpack_uint(values->VertexElementEnables[26], 8, 11) |
      util_bitpack_uint(values->VertexElementEnables[27], 12, 15) |
      util_bitpack_uint(values->VertexElementEnables[28], 16, 19) |
      util_bitpack_uint(values->VertexElementEnables[29], 20, 23) |
      util_bitpack_uint(values->VertexElementEnables[30], 24, 27) |
      util_bitpack_uint(values->VertexElementEnables[31], 28, 31) |
      util_bitpack_uint(values->VertexElementEnablesDW[3], 0, 31);
}

#define GFX30_3DSTATE_VF_INSTANCING_length      3
#define GFX30_3DSTATE_VF_INSTANCING_length_bias      2
#define GFX30_3DSTATE_VF_INSTANCING_header      \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     73,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_INSTANCING {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             VertexElementIndex;
   bool                                 InstancingEnable;
   uint32_t                             InstanceDataStepRate;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_INSTANCING_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_3DSTATE_VF_INSTANCING * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->VertexElementIndex, 0, 5) |
      util_bitpack_uint(values->InstancingEnable, 8, 8);

   dw[2] =
      util_bitpack_uint(values->InstanceDataStepRate, 0, 31);
}

#define GFX30_3DSTATE_VF_SGVS_length           2
#define GFX30_3DSTATE_VF_SGVS_length_bias      2
#define GFX30_3DSTATE_VF_SGVS_header            \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     74,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_SGVS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             VertexIDElementOffset;
   uint32_t                             VertexIDComponentNumber;
#define COMP_0                                   0
#define COMP_1                                   1
#define COMP_2                                   2
#define COMP_3                                   3
   bool                                 VertexIDEnable;
   uint32_t                             InstanceIDElementOffset;
   uint32_t                             InstanceIDComponentNumber;
#define COMP_0                                   0
#define COMP_1                                   1
#define COMP_2                                   2
#define COMP_3                                   3
   bool                                 InstanceIDEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_SGVS_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_3DSTATE_VF_SGVS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->VertexIDElementOffset, 0, 5) |
      util_bitpack_uint(values->VertexIDComponentNumber, 13, 14) |
      util_bitpack_uint(values->VertexIDEnable, 15, 15) |
      util_bitpack_uint(values->InstanceIDElementOffset, 16, 21) |
      util_bitpack_uint(values->InstanceIDComponentNumber, 29, 30) |
      util_bitpack_uint(values->InstanceIDEnable, 31, 31);
}

#define GFX30_3DSTATE_VF_SGVS_2_length         3
#define GFX30_3DSTATE_VF_SGVS_2_length_bias      2
#define GFX30_3DSTATE_VF_SGVS_2_header          \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =     86,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_SGVS_2 {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             XP0ElementOffset;
   uint32_t                             XP0SourceSelect;
#define VERTEX_LOCATION                          1
#define XP0_PARAMETER                            0
   uint32_t                             XP0ComponentNumber;
#define COMP_0                                   0
#define COMP_1                                   1
#define COMP_2                                   2
#define COMP_3                                   3
   uint32_t                             XP0Enable;
   uint32_t                             XP1ElementOffset;
   uint32_t                             XP1SourceSelect;
#define StartingInstanceLocation                 1
#define XP1_PARAMETER                            0
   uint32_t                             XP1ComponentNumber;
#define COMP_0                                   0
#define COMP_1                                   1
#define COMP_2                                   2
#define COMP_3                                   3
   uint32_t                             XP1Enable;
   uint32_t                             XP2ElementOffset;
   uint32_t                             XP2ComponentNumber;
#define COMP_0                                   0
#define COMP_1                                   1
#define COMP_2                                   2
#define COMP_3                                   3
   uint32_t                             XP2Enable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_SGVS_2_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_3DSTATE_VF_SGVS_2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->XP0ElementOffset, 0, 5) |
      util_bitpack_uint(values->XP0SourceSelect, 12, 12) |
      util_bitpack_uint(values->XP0ComponentNumber, 13, 14) |
      util_bitpack_uint(values->XP0Enable, 15, 15) |
      util_bitpack_uint(values->XP1ElementOffset, 16, 21) |
      util_bitpack_uint(values->XP1SourceSelect, 28, 28) |
      util_bitpack_uint(values->XP1ComponentNumber, 29, 30) |
      util_bitpack_uint(values->XP1Enable, 31, 31);

   dw[2] =
      util_bitpack_uint(values->XP2ElementOffset, 0, 5) |
      util_bitpack_uint(values->XP2ComponentNumber, 13, 14) |
      util_bitpack_uint(values->XP2Enable, 15, 15);
}

#define GFX30_3DSTATE_VF_STATISTICS_length      1
#define GFX30_3DSTATE_VF_STATISTICS_length_bias      1
#define GFX30_3DSTATE_VF_STATISTICS_header      \
   ._3DCommandSubOpcode                 =     11,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      1,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_STATISTICS {
   bool                                 StatisticsEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_STATISTICS_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_3DSTATE_VF_STATISTICS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->StatisticsEnable, 0, 0) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_3DSTATE_VF_TOPOLOGY_length       2
#define GFX30_3DSTATE_VF_TOPOLOGY_length_bias      2
#define GFX30_3DSTATE_VF_TOPOLOGY_header        \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     75,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VF_TOPOLOGY {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   enum GFX30_3D_Prim_Topo_Type         PrimitiveTopologyType;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VF_TOPOLOGY_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_3DSTATE_VF_TOPOLOGY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->PrimitiveTopologyType, 0, 5);
}

#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC_length      2
#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC_length_bias      2
#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     35,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             CCViewportPointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC_pack(__attribute__((unused)) __gen_user_data *data,
                                              __attribute__((unused)) void * restrict dst,
                                              __attribute__((unused)) const struct GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_CC * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->CCViewportPointer, 5, 31);
}

#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP_length      2
#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP_length_bias      2
#define GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP_header\
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     33,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             SFClipViewportPointer;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP_pack(__attribute__((unused)) __gen_user_data *data,
                                                   __attribute__((unused)) void * restrict dst,
                                                   __attribute__((unused)) const struct GFX30_3DSTATE_VIEWPORT_STATE_POINTERS_SF_CLIP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->SFClipViewportPointer, 6, 31);
}

#define GFX30_3DSTATE_VS_length                9
#define GFX30_3DSTATE_VS_length_bias           2
#define GFX30_3DSTATE_VS_header                 \
   .DWordLength                         =      7,  \
   ._3DCommandSubOpcode                 =     16,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_VS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             KernelStartPointer;
   uint32_t                             RegistersPerThread;
   bool                                 SoftwareExceptionEnable;
   bool                                 AccessesUAV;
   bool                                 IllegalOpcodeExceptionEnable;
   uint32_t                             FloatingPointMode;
#define IEEE754                                  0
#define Alternate                                1
   uint32_t                             ThreadDispatchPriority;
#define Normal                                   0
#define High                                     1
   uint32_t                             BindingTableEntryCount;
   uint32_t                             SamplerCount;
#define NoSamplers                               0
#define _14Samplers                              1
#define _58Samplers                              2
#define _912Samplers                             3
#define _1316Samplers                            4
   uint32_t                             VectorMaskEnable;
#define Dmask                                    0
   uint32_t                             ScratchSpaceBuffer;
   uint32_t                             VertexURBEntryReadOffset;
   uint32_t                             VertexURBEntryReadLength;
   uint32_t                             DispatchGRFStartRegisterForURBData;
   bool                                 Enable;
   bool                                 VertexCacheDisable;
   bool                                 SingleInstanceDispatchEnable;
   bool                                 StatisticsEnable;
   uint32_t                             MaximumNumberofThreads;
   uint32_t                             UserClipDistanceCullTestEnableBitmask;
   uint32_t                             UserClipDistanceClipTestEnableBitmask;
   uint32_t                             VertexURBEntryOutputLength;
   uint32_t                             VertexURBEntryOutputReadOffset;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_VS_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_VS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->KernelStartPointer, 6, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;

   dw[3] =
      util_bitpack_uint(values->RegistersPerThread, 0, 5) |
      util_bitpack_uint(values->SoftwareExceptionEnable, 7, 7) |
      util_bitpack_uint(values->AccessesUAV, 12, 12) |
      util_bitpack_uint(values->IllegalOpcodeExceptionEnable, 13, 13) |
      util_bitpack_uint(values->FloatingPointMode, 16, 16) |
      util_bitpack_uint(values->ThreadDispatchPriority, 17, 17) |
      util_bitpack_uint(values->BindingTableEntryCount, 18, 25) |
      util_bitpack_uint(values->SamplerCount, 27, 29) |
      util_bitpack_uint(values->VectorMaskEnable, 30, 30);

   dw[4] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[5] = 0;

   dw[6] =
      util_bitpack_uint(values->VertexURBEntryReadOffset, 4, 9) |
      util_bitpack_uint(values->VertexURBEntryReadLength, 11, 16) |
      util_bitpack_uint(values->DispatchGRFStartRegisterForURBData, 20, 24);

   dw[7] =
      util_bitpack_uint(values->Enable, 0, 0) |
      util_bitpack_uint(values->VertexCacheDisable, 1, 1) |
      util_bitpack_uint(values->SingleInstanceDispatchEnable, 9, 9) |
      util_bitpack_uint(values->StatisticsEnable, 10, 10) |
      util_bitpack_uint(values->MaximumNumberofThreads, 22, 31);

   dw[8] =
      util_bitpack_uint(values->UserClipDistanceCullTestEnableBitmask, 0, 7) |
      util_bitpack_uint(values->UserClipDistanceClipTestEnableBitmask, 8, 15) |
      util_bitpack_uint(values->VertexURBEntryOutputLength, 16, 20) |
      util_bitpack_uint(values->VertexURBEntryOutputReadOffset, 21, 26);
}

#define GFX30_3DSTATE_WM_length                2
#define GFX30_3DSTATE_WM_length_bias           2
#define GFX30_3DSTATE_WM_header                 \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     20,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3,  \
   .WalkerDirection                     =      0,  \
   .WalkingGranularity                  =      2

struct GFX30_3DSTATE_WM {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ForceKillPixelEnable;
#define ForceOff                                 1
#define ForceON                                  2
   uint32_t                             PointRasterizationRule;
#define RASTRULE_UPPER_LEFT                      0
#define RASTRULE_UPPER_RIGHT                     1
   bool                                 LineStippleEnable;
   bool                                 PolygonStippleEnable;
   uint32_t                             LineAntialiasingRegionWidth;
#define _05pixels                                0
#define _10pixels                                1
#define _20pixels                                2
#define _40pixels                                3
   uint32_t                             LineEndCapAntialiasingRegionWidth;
#define _05pixels                                0
#define _10pixels                                1
#define _20pixels                                2
#define _40pixels                                3
   uint32_t                             BarycentricInterpolationMode;
#define BIM_PERSPECTIVE_PIXEL                    1
#define BIM_PERSPECTIVE_CENTROID                 2
#define BIM_PERSPECTIVE_SAMPLE                   4
#define BIM_LINEAR_PIXEL                         8
#define BIM_LINEAR_CENTROID                      16
#define BIM_LINEAR_SAMPLE                        32
   uint32_t                             PositionZWInterpolationMode;
#define INTERP_PIXEL                             0
#define INTERP_CENTROID                          2
#define INTERP_SAMPLE                            3
   uint32_t                             ForceThreadDispatchEnable;
#define ForceOff                                 1
#define ForceON                                  2
   uint32_t                             EarlyDepthStencilControl;
#define EDSC_NORMAL                              0
#define EDSC_PSEXEC                              1
#define EDSC_PREPS                               2
   uint32_t                             WalkerDirection;
#define SnakeWalk                                0
#define ZWalk                                    1
   uint32_t                             WalkingGranularity;
#define _16x16granularity                        0
#define _32x32granularity                        1
#define _64x64granularity                        2
   bool                                 LegacyDiamondLineRasterization;
   bool                                 LegacyHierarchicalDepthBufferResolveEnable;
   bool                                 LegacyDepthBufferResolveEnable;
   bool                                 LegacyDepthBufferClearEnable;
   bool                                 StatisticsEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_WM_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_3DSTATE_WM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ForceKillPixelEnable, 0, 1) |
      util_bitpack_uint(values->PointRasterizationRule, 2, 2) |
      util_bitpack_uint(values->LineStippleEnable, 3, 3) |
      util_bitpack_uint(values->PolygonStippleEnable, 4, 4) |
      util_bitpack_uint(values->LineAntialiasingRegionWidth, 6, 7) |
      util_bitpack_uint(values->LineEndCapAntialiasingRegionWidth, 8, 9) |
      util_bitpack_uint(values->BarycentricInterpolationMode, 11, 16) |
      util_bitpack_uint(values->PositionZWInterpolationMode, 17, 18) |
      util_bitpack_uint(values->ForceThreadDispatchEnable, 19, 20) |
      util_bitpack_uint(values->EarlyDepthStencilControl, 21, 22) |
      util_bitpack_uint(values->WalkerDirection, 23, 23) |
      util_bitpack_uint(values->WalkingGranularity, 24, 25) |
      util_bitpack_uint(values->LegacyDiamondLineRasterization, 26, 26) |
      util_bitpack_uint(values->LegacyHierarchicalDepthBufferResolveEnable, 27, 27) |
      util_bitpack_uint(values->LegacyDepthBufferResolveEnable, 28, 28) |
      util_bitpack_uint(values->LegacyDepthBufferClearEnable, 30, 30) |
      util_bitpack_uint(values->StatisticsEnable, 31, 31);
}

#define GFX30_3DSTATE_WM_CHROMAKEY_length      2
#define GFX30_3DSTATE_WM_CHROMAKEY_length_bias      2
#define GFX30_3DSTATE_WM_CHROMAKEY_header       \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =     76,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_WM_CHROMAKEY {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 ChromaKeyKillEnable;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_WM_CHROMAKEY_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_3DSTATE_WM_CHROMAKEY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ChromaKeyKillEnable, 31, 31);
}

#define GFX30_3DSTATE_WM_DEPTH_STENCIL_length      4
#define GFX30_3DSTATE_WM_DEPTH_STENCIL_length_bias      2
#define GFX30_3DSTATE_WM_DEPTH_STENCIL_header   \
   .DWordLength                         =      2,  \
   ._3DCommandSubOpcode                 =     78,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_WM_DEPTH_STENCIL {
   uint32_t                             DWordLength;
   bool                                 StencilReferenceValueModifyDisable;
   bool                                 StencilTestMaskModifyDisable;
   bool                                 StencilWriteMaskModifyDisable;
   bool                                 StencilStateModifyDisable;
   bool                                 DepthStateModifyDisable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 DepthBufferWriteEnable;
   bool                                 DepthTestEnable;
   bool                                 StencilBufferWriteEnable;
   bool                                 StencilTestEnable;
   bool                                 DoubleSidedStencilEnable;
   enum GFX30_3D_Compare_Function       DepthTestFunction;
   enum GFX30_3D_Compare_Function       StencilTestFunction;
   enum GFX30_3D_Stencil_Operation      BackfaceStencilPassDepthPassOp;
   enum GFX30_3D_Stencil_Operation      BackfaceStencilPassDepthFailOp;
   enum GFX30_3D_Stencil_Operation      BackfaceStencilFailOp;
   enum GFX30_3D_Compare_Function       BackfaceStencilTestFunction;
   enum GFX30_3D_Stencil_Operation      StencilPassDepthPassOp;
   enum GFX30_3D_Stencil_Operation      StencilPassDepthFailOp;
   enum GFX30_3D_Stencil_Operation      StencilFailOp;
   uint32_t                             BackfaceStencilWriteMask;
   uint32_t                             BackfaceStencilTestMask;
   uint32_t                             StencilWriteMask;
   uint32_t                             StencilTestMask;
   uint32_t                             BackfaceStencilReferenceValue;
   uint32_t                             StencilReferenceValue;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_WM_DEPTH_STENCIL_pack(__attribute__((unused)) __gen_user_data *data,
                                    __attribute__((unused)) void * restrict dst,
                                    __attribute__((unused)) const struct GFX30_3DSTATE_WM_DEPTH_STENCIL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->StencilReferenceValueModifyDisable, 8, 8) |
      util_bitpack_uint(values->StencilTestMaskModifyDisable, 9, 9) |
      util_bitpack_uint(values->StencilWriteMaskModifyDisable, 10, 10) |
      util_bitpack_uint(values->StencilStateModifyDisable, 11, 11) |
      util_bitpack_uint(values->DepthStateModifyDisable, 12, 12) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DepthBufferWriteEnable, 0, 0) |
      util_bitpack_uint(values->DepthTestEnable, 1, 1) |
      util_bitpack_uint(values->StencilBufferWriteEnable, 2, 2) |
      util_bitpack_uint(values->StencilTestEnable, 3, 3) |
      util_bitpack_uint(values->DoubleSidedStencilEnable, 4, 4) |
      util_bitpack_uint(values->DepthTestFunction, 5, 7) |
      util_bitpack_uint(values->StencilTestFunction, 8, 10) |
      util_bitpack_uint(values->BackfaceStencilPassDepthPassOp, 11, 13) |
      util_bitpack_uint(values->BackfaceStencilPassDepthFailOp, 14, 16) |
      util_bitpack_uint(values->BackfaceStencilFailOp, 17, 19) |
      util_bitpack_uint(values->BackfaceStencilTestFunction, 20, 22) |
      util_bitpack_uint(values->StencilPassDepthPassOp, 23, 25) |
      util_bitpack_uint(values->StencilPassDepthFailOp, 26, 28) |
      util_bitpack_uint(values->StencilFailOp, 29, 31);

   dw[2] =
      util_bitpack_uint(values->BackfaceStencilWriteMask, 0, 7) |
      util_bitpack_uint(values->BackfaceStencilTestMask, 8, 15) |
      util_bitpack_uint(values->StencilWriteMask, 16, 23) |
      util_bitpack_uint(values->StencilTestMask, 24, 31);

   dw[3] =
      util_bitpack_uint(values->BackfaceStencilReferenceValue, 0, 7) |
      util_bitpack_uint(values->StencilReferenceValue, 8, 15);
}

#define GFX30_3DSTATE_WM_HZ_OP_length          6
#define GFX30_3DSTATE_WM_HZ_OP_length_bias      2
#define GFX30_3DSTATE_WM_HZ_OP_header           \
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =     82,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_3DSTATE_WM_HZ_OP {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 DepthBufferPartialResolveEnable;
   uint32_t                             NumberofMultisamples;
#define SAMPLES_1                                0
#define SAMPLES_2                                1
#define SAMPLES_4                                2
#define SAMPLES_8                                3
#define SAMPLES_16                               4
   uint32_t                             StencilClearValue;
   bool                                 StencilBufferResolveEnable;
   bool                                 FullSurfaceDepthandStencilClear;
   bool                                 PixelPositionOffsetEnable;
   bool                                 HierarchicalDepthBufferResolveEnable;
   bool                                 DepthBufferResolveEnable;
   bool                                 ScissorRectangleEnable;
   bool                                 DepthBufferClearEnable;
   bool                                 StencilBufferClearEnable;
   uint32_t                             ClearRectangleXMin;
   uint32_t                             ClearRectangleYMin;
   uint32_t                             ClearRectangleXMax;
   uint32_t                             ClearRectangleYMax;
   uint32_t                             SampleMask;
   float                                DepthClearValue;
};

static inline __attribute__((always_inline)) void
GFX30_3DSTATE_WM_HZ_OP_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_3DSTATE_WM_HZ_OP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DepthBufferPartialResolveEnable, 9, 9) |
      util_bitpack_uint(values->NumberofMultisamples, 13, 15) |
      util_bitpack_uint(values->StencilClearValue, 16, 23) |
      util_bitpack_uint(values->StencilBufferResolveEnable, 24, 24) |
      util_bitpack_uint(values->FullSurfaceDepthandStencilClear, 25, 25) |
      util_bitpack_uint(values->PixelPositionOffsetEnable, 26, 26) |
      util_bitpack_uint(values->HierarchicalDepthBufferResolveEnable, 27, 27) |
      util_bitpack_uint(values->DepthBufferResolveEnable, 28, 28) |
      util_bitpack_uint(values->ScissorRectangleEnable, 29, 29) |
      util_bitpack_uint(values->DepthBufferClearEnable, 30, 30) |
      util_bitpack_uint(values->StencilBufferClearEnable, 31, 31);

   dw[2] =
      util_bitpack_uint(values->ClearRectangleXMin, 0, 15) |
      util_bitpack_uint(values->ClearRectangleYMin, 16, 31);

   dw[3] =
      util_bitpack_uint(values->ClearRectangleXMax, 0, 15) |
      util_bitpack_uint(values->ClearRectangleYMax, 16, 31);

   dw[4] =
      util_bitpack_uint(values->SampleMask, 0, 15);

   dw[5] =
      util_bitpack_float(values->DepthClearValue);
}

#define GFX30_CFE_STATE_length                 6
#define GFX30_CFE_STATE_length_bias            2
#define GFX30_CFE_STATE_header                  \
   .DWordLength                         =      4,  \
   .CFESubOpcodeVariant                 =      0,  \
   .CFESubOpcode                        =      0,  \
   .ComputeCommandOpcode                =      2,  \
   .Pipeline                            =      2,  \
   .CommandType                         =      3

struct GFX30_CFE_STATE {
   uint32_t                             DWordLength;
   uint32_t                             CFESubOpcodeVariant;
   uint32_t                             CFESubOpcode;
   uint32_t                             ComputeCommandOpcode;
   uint32_t                             Pipeline;
   uint32_t                             CommandType;
   uint32_t                             ScratchSpaceBuffer;
   uint32_t                             StackIDControl;
#define StackIDs2048                             0
#define StackIDs1024                             1
#define StackIDs512                              2
#define StackIDs256                              3
   bool                                 DynamicStackIDControl;
   bool                                 ComputeDispatchAllwalkerEnable;
   bool                                 SingleSliceDispatchCCSMode;
   uint32_t                             OverDispatchControl;
#define OverDispatchNone                         0
#define OverDispatchLow                          1
#define OverDispatchNormal                       2
#define OverDispatchHigh                         3
   uint32_t                             MaximumNumberofThreads;
};

static inline __attribute__((always_inline)) void
GFX30_CFE_STATE_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_CFE_STATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->CFESubOpcodeVariant, 16, 17) |
      util_bitpack_uint(values->CFESubOpcode, 18, 23) |
      util_bitpack_uint(values->ComputeCommandOpcode, 24, 26) |
      util_bitpack_uint(values->Pipeline, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ScratchSpaceBuffer, 10, 31);

   dw[2] = 0;

   dw[3] =
      util_bitpack_uint(values->StackIDControl, 0, 1) |
      util_bitpack_uint(values->DynamicStackIDControl, 2, 2) |
      util_bitpack_uint(values->ComputeDispatchAllwalkerEnable, 12, 12) |
      util_bitpack_uint(values->SingleSliceDispatchCCSMode, 13, 13) |
      util_bitpack_uint(values->OverDispatchControl, 14, 15) |
      util_bitpack_uint(values->MaximumNumberofThreads, 16, 31);

   dw[4] = 0;

   dw[5] = 0;
}

#define GFX30_COMPUTE_WALKER_length           40
#define GFX30_COMPUTE_WALKER_length_bias       2
#define GFX30_COMPUTE_WALKER_header             \
   .DWordLength                         =     38,  \
   .CFESubOpcodeVariant                 =      0,  \
   .CFESubOpcode                        =      2,  \
   .ComputeCommandOpcode                =      2,  \
   .Pipeline                            =      2,  \
   .CommandType                         =      3

struct GFX30_COMPUTE_WALKER {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   bool                                 WorkloadPartitionEnable;
   bool                                 IndirectParameterEnable;
   bool                                 SystolicModeEnable;
   uint32_t                             CFESubOpcodeVariant;
   uint32_t                             CFESubOpcode;
   uint32_t                             ComputeCommandOpcode;
   uint32_t                             Pipeline;
   uint32_t                             CommandType;
   struct GFX30_COMPUTE_WALKER_BODY     body;
};

static inline __attribute__((always_inline)) void
GFX30_COMPUTE_WALKER_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_COMPUTE_WALKER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->WorkloadPartitionEnable, 9, 9) |
      util_bitpack_uint(values->IndirectParameterEnable, 10, 10) |
      util_bitpack_uint(values->SystolicModeEnable, 14, 14) |
      util_bitpack_uint(values->CFESubOpcodeVariant, 15, 17) |
      util_bitpack_uint(values->CFESubOpcode, 18, 23) |
      util_bitpack_uint(values->ComputeCommandOpcode, 24, 26) |
      util_bitpack_uint(values->Pipeline, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_COMPUTE_WALKER_BODY_pack(data, &dw[1], &values->body);
}

#define GFX30_EXECUTE_INDIRECT_DISPATCH_length     45
#define GFX30_EXECUTE_INDIRECT_DISPATCH_length_bias      2
#define GFX30_EXECUTE_INDIRECT_DISPATCH_header  \
   .DWordLength                         =     43,  \
   ._3DCommandSubOpcode                 =      1,  \
   ._3DCommandOpcode                    =      4,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_EXECUTE_INDIRECT_DISPATCH {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   uint32_t                             MOCSIndex;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MaxCount;
   bool                                 CountBufferIndirectEnable;
   __gen_address_type                   CountBufferAddress;
   __gen_address_type                   ArgumentBufferStartAddress;
   struct GFX30_COMPUTE_WALKER_BODY     body;
};

static inline __attribute__((always_inline)) void
GFX30_EXECUTE_INDIRECT_DISPATCH_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_EXECUTE_INDIRECT_DISPATCH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint_nonzero(values->MOCSIndex, 12, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MaxCount, 0, 31);

   const uint64_t v2 =
      util_bitpack_uint(values->CountBufferIndirectEnable, 0, 0);
   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->CountBufferAddress, v2, 2, 63);
   dw[2] = v2_address;
   dw[3] = (v2_address >> 32) | (v2 >> 32);

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->ArgumentBufferStartAddress, 0, 2, 63);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;

   GFX30_COMPUTE_WALKER_BODY_pack(data, &dw[6], &values->body);
}

#define GFX30_EXECUTE_INDIRECT_DRAW_length      6
#define GFX30_EXECUTE_INDIRECT_DRAW_length_bias      2
#define GFX30_EXECUTE_INDIRECT_DRAW_header      \
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      4,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_EXECUTE_INDIRECT_DRAW {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   uint32_t                             TBIMREnabled;
   uint32_t                             ArgumentFormat;
#define XI_DRAW                                  0
#define XI_DRAWINDEXED                           1
#define XI_MESH_3D                               2
#define XI_MESH_1D                               3
   uint32_t                             MOCSIndex;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             MaxCount;
   bool                                 CountBufferIndirectEnable;
   __gen_address_type                   CountBufferAddress;
   __gen_address_type                   ArgumentBufferStartAddress;
};

static inline __attribute__((always_inline)) void
GFX30_EXECUTE_INDIRECT_DRAW_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_EXECUTE_INDIRECT_DRAW * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 8, 8) |
      util_bitpack_uint(values->TBIMREnabled, 9, 9) |
      util_bitpack_uint(values->ArgumentFormat, 10, 11) |
      util_bitpack_uint_nonzero(values->MOCSIndex, 12, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->MaxCount, 0, 31);

   const uint64_t v2 =
      util_bitpack_uint(values->CountBufferIndirectEnable, 0, 0);
   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->CountBufferAddress, v2, 2, 63);
   dw[2] = v2_address;
   dw[3] = (v2_address >> 32) | (v2 >> 32);

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->ArgumentBufferStartAddress, 0, 2, 63);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;
}

#define GFX30_MI_ARB_CHECK_length              1
#define GFX30_MI_ARB_CHECK_length_bias         1
#define GFX30_MI_ARB_CHECK_header               \
   .MICommandOpcode                     =      5,  \
   .CommandType                         =      0

struct GFX30_MI_ARB_CHECK {
   bool                                 PreParserDisable;
   bool                                 PreParserDisableMask;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_ARB_CHECK_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_MI_ARB_CHECK * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PreParserDisable, 0, 0) |
      util_bitpack_uint(values->PreParserDisableMask, 8, 8) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_ARB_ON_OFF_length             1
#define GFX30_MI_ARB_ON_OFF_length_bias        1
#define GFX30_MI_ARB_ON_OFF_header              \
   .ArbitrationEnable                   =      1,  \
   .MICommandOpcode                     =      8,  \
   .CommandType                         =      0

struct GFX30_MI_ARB_ON_OFF {
   bool                                 ArbitrationEnable;
   bool                                 AllowLiteRestore;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_ARB_ON_OFF_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_MI_ARB_ON_OFF * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ArbitrationEnable, 0, 0) |
      util_bitpack_uint(values->AllowLiteRestore, 1, 1) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_ATOMIC_length                 3
#define GFX30_MI_ATOMIC_length_bias            2
#define GFX30_MI_ATOMIC_header                  \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     47,  \
   .CommandType                         =      0

struct GFX30_MI_ATOMIC {
   uint32_t                             DWordLength;
   enum GFX30_Atomic_OPCODE             ATOMICOPCODE;
   bool                                 ReturnDataControl;
   bool                                 CSSTALL;
   bool                                 InlineData;
   uint32_t                             DataSize;
#define MI_ATOMIC_DWORD                          0
#define MI_ATOMIC_QWORD                          1
#define MI_ATOMIC_OCTWORD                        2
#define MI_ATOMIC_RESERVED                       3
   bool                                 PostSyncOperation;
   uint32_t                             MemoryType;
#define PerProcessGraphicsAddress                0
#define GlobalGraphicsAddress                    1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   __gen_address_type                   MemoryAddress;
   uint32_t                             Operand1DataDword0;
   uint32_t                             Operand2DataDword0;
   uint32_t                             Operand1DataDword1;
   uint32_t                             Operand2DataDword1;
   uint32_t                             Operand1DataDword2;
   uint32_t                             Operand2DataDword2;
   uint32_t                             Operand1DataDword3;
   uint32_t                             Operand2DataDword3;
};

static inline __attribute__((always_inline)) void
GFX30_MI_ATOMIC_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_MI_ATOMIC * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->ATOMICOPCODE, 8, 15) |
      util_bitpack_uint(values->ReturnDataControl, 16, 16) |
      util_bitpack_uint(values->CSSTALL, 17, 17) |
      util_bitpack_uint(values->InlineData, 18, 18) |
      util_bitpack_uint(values->DataSize, 19, 20) |
      util_bitpack_uint(values->PostSyncOperation, 21, 21) |
      util_bitpack_uint(values->MemoryType, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->MemoryAddress, 0, 2, 47);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;
}

#define GFX30_MI_BATCH_BUFFER_END_length       1
#define GFX30_MI_BATCH_BUFFER_END_length_bias      1
#define GFX30_MI_BATCH_BUFFER_END_header        \
   .MICommandOpcode                     =     10,  \
   .CommandType                         =      0

struct GFX30_MI_BATCH_BUFFER_END {
   bool                                 EndContext;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_BATCH_BUFFER_END_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_MI_BATCH_BUFFER_END * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->EndContext, 0, 0) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_BATCH_BUFFER_START_length      3
#define GFX30_MI_BATCH_BUFFER_START_length_bias      2
#define GFX30_MI_BATCH_BUFFER_START_header      \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     49,  \
   .CommandType                         =      0

struct GFX30_MI_BATCH_BUFFER_START {
   uint32_t                             DWordLength;
   uint32_t                             AddressSpaceIndicator;
#define ASI_GGTT                                 0
#define ASI_PPGTT                                1
   bool                                 ResourceStreamerEnable;
   bool                                 PredicationEnable;
   uint32_t                             SecondLevelBatchBuffer;
#define Firstlevelbatch                          0
#define Secondlevelbatch                         1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   __gen_address_type                   BatchBufferStartAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_BATCH_BUFFER_START_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_MI_BATCH_BUFFER_START * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->AddressSpaceIndicator, 8, 8) |
      util_bitpack_uint(values->ResourceStreamerEnable, 10, 10) |
      util_bitpack_uint(values->PredicationEnable, 15, 15) |
      util_bitpack_uint(values->SecondLevelBatchBuffer, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->BatchBufferStartAddress, 0, 2, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;
}

#define GFX30_MI_CLFLUSH_length_bias           2
#define GFX30_MI_CLFLUSH_header                 \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     39,  \
   .CommandType                         =      0

struct GFX30_MI_CLFLUSH {
   uint32_t                             DWordLength;
   bool                                 UseGlobalGTT;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             StartingCachelineOffset;
   __gen_address_type                   PageBaseAddress;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_MI_CLFLUSH_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_MI_CLFLUSH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 9) |
      util_bitpack_uint(values->UseGlobalGTT, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->StartingCachelineOffset, 6, 11);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->PageBaseAddress, v1, 12, 47);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);
}

#define GFX30_MI_CONDITIONAL_BATCH_BUFFER_END_length      4
#define GFX30_MI_CONDITIONAL_BATCH_BUFFER_END_length_bias      2
#define GFX30_MI_CONDITIONAL_BATCH_BUFFER_END_header\
   .DWordLength                         =      2,  \
   .CompareSemaphore                    =      0,  \
   .MICommandOpcode                     =     54,  \
   .CommandType                         =      0

struct GFX30_MI_CONDITIONAL_BATCH_BUFFER_END {
   uint32_t                             DWordLength;
   uint32_t                             CompareMaskMode;
#define CompareMaskModeDisabled                  0
#define CompareMaskModeEnabled                   1
   uint32_t                             CompareSemaphore;
   bool                                 UseGlobalGTT;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             CompareDataDword;
   __gen_address_type                   CompareAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_CONDITIONAL_BATCH_BUFFER_END_pack(__attribute__((unused)) __gen_user_data *data,
                                           __attribute__((unused)) void * restrict dst,
                                           __attribute__((unused)) const struct GFX30_MI_CONDITIONAL_BATCH_BUFFER_END * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->CompareMaskMode, 19, 19) |
      util_bitpack_uint(values->CompareSemaphore, 21, 21) |
      util_bitpack_uint(values->UseGlobalGTT, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->CompareDataDword, 0, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->CompareAddress, 0, 3, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;
}

#define GFX30_MI_COPY_MEM_MEM_length           5
#define GFX30_MI_COPY_MEM_MEM_length_bias      2
#define GFX30_MI_COPY_MEM_MEM_header            \
   .DWordLength                         =      3,  \
   .MICommandOpcode                     =     46,  \
   .CommandType                         =      0

struct GFX30_MI_COPY_MEM_MEM {
   uint32_t                             DWordLength;
   bool                                 UseGlobalGTTDestination;
   bool                                 UseGlobalGTTSource;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   __gen_address_type                   DestinationMemoryAddress;
   __gen_address_type                   SourceMemoryAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_COPY_MEM_MEM_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_MI_COPY_MEM_MEM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->UseGlobalGTTDestination, 21, 21) |
      util_bitpack_uint(values->UseGlobalGTTSource, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->DestinationMemoryAddress, 0, 2, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;

   const uint64_t v3_address =
      __gen_address(data, &dw[3], values->SourceMemoryAddress, 0, 2, 63);
   dw[3] = v3_address;
   dw[4] = v3_address >> 32;
}

#define GFX30_MI_DISPLAY_FLIP_length           3
#define GFX30_MI_DISPLAY_FLIP_length_bias      2
#define GFX30_MI_DISPLAY_FLIP_header            \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     20,  \
   .CommandType                         =      0

struct GFX30_MI_DISPLAY_FLIP {
   uint32_t                             DWordLength;
   uint32_t                             DisplayPlaneSelect;
#define DisplayPlane1                            0
#define DisplayPlane2                            1
#define DisplayPlane3                            2
#define DisplayPlane4                            4
#define DisplayPlane5                            5
#define DisplayPlane6                            6
#define DisplayPlane7                            7
#define DisplayPlane8                            8
#define DisplayPlane9                            9
#define DisplayPlane10                           10
#define DisplayPlane11                           11
#define DisplayPlane12                           12
#define DisplayPlane13                           13
#define DisplayPlane14                           14
#define DisplayPlane15                           15
#define DisplayPlane16                           16
#define DisplayPlane17                           17
#define DisplayPlane18                           18
#define DisplayPlane19                           19
#define DisplayPlane20                           20
#define DisplayPlane21                           21
#define DisplayPlane22                           22
#define DisplayPlane23                           23
#define DisplayPlane24                           24
#define DisplayPlane25                           25
#define DisplayPlane26                           26
#define DisplayPlane27                           27
#define DisplayPlane28                           28
#define DisplayPlane29                           29
#define DisplayPlane30                           30
#define DisplayPlane31                           31
#define DisplayPlane32                           32
   bool                                 AsyncFlipIndicator;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             TileParameter;
   uint32_t                             DisplayBufferPitch;
   bool                                 Stereoscopic3DMode;
   uint32_t                             FlipType;
#define SyncFlip                                 0
#define AsyncFlip                                1
#define Stereo3DFlip                             2
   uint32_t                             VRRMasterFlip;
   __gen_address_type                   DisplayBufferBaseAddress;
   __gen_address_type                   LeftEyeDisplayBufferBaseAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_DISPLAY_FLIP_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_MI_DISPLAY_FLIP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->DisplayPlaneSelect, 8, 13) |
      util_bitpack_uint(values->AsyncFlipIndicator, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TileParameter, 0, 2) |
      util_bitpack_uint(values->DisplayBufferPitch, 6, 15) |
      util_bitpack_uint(values->Stereoscopic3DMode, 31, 31);

   const uint32_t v2 =
      util_bitpack_uint(values->FlipType, 0, 1) |
      util_bitpack_uint(values->VRRMasterFlip, 11, 11);
   dw[2] = __gen_address(data, &dw[2], values->DisplayBufferBaseAddress, v2, 12, 31);
}

#define GFX30_MI_FLUSH_DW_length               5
#define GFX30_MI_FLUSH_DW_length_bias          2
#define GFX30_MI_FLUSH_DW_header                \
   .DWordLength                         =      3,  \
   .MICommandOpcode                     =     38,  \
   .CommandType                         =      0

struct GFX30_MI_FLUSH_DW {
   uint32_t                             DWordLength;
   bool                                 VideoPipelineCacheInvalidate;
   bool                                 NotifyEnable;
   bool                                 FlushLLC;
   uint32_t                             PostSyncOperation;
#define NoWrite                                  0
#define WriteImmediateData                       1
#define WriteTimestamp                           3
   bool                                 FlushCCS;
   bool                                 TLBInvalidate;
   uint32_t                             StoreDataIndex;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             DestinationAddressType;
#define DAT_PPGTT                                0
#define DAT_GGTT                                 1
   __gen_address_type                   Address;
   uint64_t                             ImmediateData;
};

static inline __attribute__((always_inline)) void
GFX30_MI_FLUSH_DW_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_MI_FLUSH_DW * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 5) |
      util_bitpack_uint(values->VideoPipelineCacheInvalidate, 7, 7) |
      util_bitpack_uint(values->NotifyEnable, 8, 8) |
      util_bitpack_uint(values->FlushLLC, 9, 9) |
      util_bitpack_uint(values->PostSyncOperation, 14, 15) |
      util_bitpack_uint(values->FlushCCS, 16, 16) |
      util_bitpack_uint(values->TLBInvalidate, 18, 18) |
      util_bitpack_uint(values->StoreDataIndex, 21, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->DestinationAddressType, 2, 2);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->Address, v1, 3, 47);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   const uint64_t v3 =
      util_bitpack_uint(values->ImmediateData, 0, 63);
   dw[3] = v3;
   dw[4] = v3 >> 32;
}

#define GFX30_MI_FORCE_WAKEUP_length           2
#define GFX30_MI_FORCE_WAKEUP_length_bias      2
#define GFX30_MI_FORCE_WAKEUP_header            \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     29,  \
   .CommandType                         =      0

struct GFX30_MI_FORCE_WAKEUP {
   uint32_t                             DWordLength;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             ForceMediaSlice0Awake;
   uint32_t                             ForceRenderAwake;
   uint32_t                             ForceMediaSlice1Awake;
   uint32_t                             ForceMediaSlice2Awake;
   uint32_t                             ForceMediaSlice3Awake;
   bool                                 HEVCPowerWellControl;
   bool                                 MFXPowerWellControl;
   uint32_t                             MaskBits;
};

static inline __attribute__((always_inline)) void
GFX30_MI_FORCE_WAKEUP_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_MI_FORCE_WAKEUP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ForceMediaSlice0Awake, 0, 0) |
      util_bitpack_uint(values->ForceRenderAwake, 1, 1) |
      util_bitpack_uint(values->ForceMediaSlice1Awake, 2, 2) |
      util_bitpack_uint(values->ForceMediaSlice2Awake, 3, 3) |
      util_bitpack_uint(values->ForceMediaSlice3Awake, 4, 4) |
      util_bitpack_uint(values->HEVCPowerWellControl, 8, 8) |
      util_bitpack_uint(values->MFXPowerWellControl, 9, 9) |
      util_bitpack_uint(values->MaskBits, 16, 31);
}

#define GFX30_MI_LOAD_REGISTER_IMM_length      3
#define GFX30_MI_LOAD_REGISTER_IMM_length_bias      2
#define GFX30_MI_LOAD_REGISTER_IMM_header       \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     34,  \
   .CommandType                         =      0

struct GFX30_MI_LOAD_REGISTER_IMM {
   uint32_t                             DWordLength;
   uint32_t                             ByteWriteDisables;
   uint32_t                             AddCSMMIOStartOffset;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint64_t                             RegisterOffset;
   uint32_t                             DataDWord;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_MI_LOAD_REGISTER_IMM_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MI_LOAD_REGISTER_IMM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->ByteWriteDisables, 8, 11) |
      util_bitpack_uint(values->AddCSMMIOStartOffset, 19, 19) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->RegisterOffset, 2, 22);

   dw[2] =
      util_bitpack_uint(values->DataDWord, 0, 31);
}

#define GFX30_MI_LOAD_REGISTER_MEM_length      4
#define GFX30_MI_LOAD_REGISTER_MEM_length_bias      2
#define GFX30_MI_LOAD_REGISTER_MEM_header       \
   .DWordLength                         =      2,  \
   .MICommandOpcode                     =     41,  \
   .CommandType                         =      0

struct GFX30_MI_LOAD_REGISTER_MEM {
   uint32_t                             DWordLength;
   uint32_t                             AddCSMMIOStartOffset;
   uint32_t                             AddLoopVariable;
   bool                                 AsyncModeEnable;
   bool                                 UseGlobalGTT;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint64_t                             RegisterAddress;
   __gen_address_type                   MemoryAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_LOAD_REGISTER_MEM_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MI_LOAD_REGISTER_MEM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->AddCSMMIOStartOffset, 19, 19) |
      util_bitpack_uint(values->AddLoopVariable, 20, 20) |
      util_bitpack_uint(values->AsyncModeEnable, 21, 21) |
      util_bitpack_uint(values->UseGlobalGTT, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->RegisterAddress, 2, 22);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->MemoryAddress, 0, 2, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;
}

#define GFX30_MI_LOAD_REGISTER_REG_length      3
#define GFX30_MI_LOAD_REGISTER_REG_length_bias      2
#define GFX30_MI_LOAD_REGISTER_REG_header       \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     42,  \
   .CommandType                         =      0

struct GFX30_MI_LOAD_REGISTER_REG {
   uint32_t                             DWordLength;
   uint32_t                             AddCSMMIOStartOffsetSource;
   uint32_t                             AddCSMMIOStartOffsetDestination;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint64_t                             SourceRegisterAddress;
   uint64_t                             DestinationRegisterAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_LOAD_REGISTER_REG_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MI_LOAD_REGISTER_REG * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->AddCSMMIOStartOffsetSource, 18, 18) |
      util_bitpack_uint(values->AddCSMMIOStartOffsetDestination, 19, 19) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->SourceRegisterAddress, 2, 22);

   dw[2] =
      __gen_offset(values->DestinationRegisterAddress, 2, 22);
}

#define GFX30_MI_LOAD_SCAN_LINES_EXCL_length      2
#define GFX30_MI_LOAD_SCAN_LINES_EXCL_length_bias      2
#define GFX30_MI_LOAD_SCAN_LINES_EXCL_header    \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     19,  \
   .CommandType                         =      0

struct GFX30_MI_LOAD_SCAN_LINES_EXCL {
   uint32_t                             DWordLength;
   uint32_t                             DisplayPlaneSelect;
#define DisplayPlaneA                            0
#define DisplayPlaneB                            1
#define DisplayPlaneC                            4
#define DisplayPlaneD                            5
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             EndScanLineNumber;
   uint32_t                             StartScanLineNumber;
};

static inline __attribute__((always_inline)) void
GFX30_MI_LOAD_SCAN_LINES_EXCL_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_MI_LOAD_SCAN_LINES_EXCL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 5) |
      util_bitpack_uint(values->DisplayPlaneSelect, 19, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->EndScanLineNumber, 0, 12) |
      util_bitpack_uint(values->StartScanLineNumber, 16, 28);
}

#define GFX30_MI_LOAD_SCAN_LINES_INCL_length      2
#define GFX30_MI_LOAD_SCAN_LINES_INCL_length_bias      2
#define GFX30_MI_LOAD_SCAN_LINES_INCL_header    \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     18,  \
   .CommandType                         =      0

struct GFX30_MI_LOAD_SCAN_LINES_INCL {
   uint32_t                             DWordLength;
   uint32_t                             ScanLineEventDoneForward;
   uint32_t                             DisplayPlaneSelect;
#define DisplayPlane1A                           0
#define DisplayPlane1B                           1
#define DisplayPlane1C                           4
#define DisplayPlane1D                           5
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             EndScanLineNumber;
   uint32_t                             StartScanLineNumber;
};

static inline __attribute__((always_inline)) void
GFX30_MI_LOAD_SCAN_LINES_INCL_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_MI_LOAD_SCAN_LINES_INCL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 5) |
      util_bitpack_uint(values->ScanLineEventDoneForward, 17, 18) |
      util_bitpack_uint(values->DisplayPlaneSelect, 19, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->EndScanLineNumber, 0, 12) |
      util_bitpack_uint(values->StartScanLineNumber, 16, 28);
}

#define GFX30_MI_MATH_length_bias              2
#define GFX30_MI_MATH_header                    \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     26,  \
   .CommandType                         =      0

struct GFX30_MI_MATH {
   uint32_t                             DWordLength;
   uint32_t                             MOCS;
   bool                                 PredicationEnable;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   /* variable length fields follow */
};

static inline __attribute__((always_inline)) void
GFX30_MI_MATH_pack(__attribute__((unused)) __gen_user_data *data,
                   __attribute__((unused)) void * restrict dst,
                   __attribute__((unused)) const struct GFX30_MI_MATH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint_nonzero(values->MOCS, 8, 14) |
      util_bitpack_uint(values->PredicationEnable, 15, 15) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_MEM_FENCE_length              1
#define GFX30_MI_MEM_FENCE_length_bias         1
#define GFX30_MI_MEM_FENCE_header               \
   .MICommandSubOpcode                  =      0,  \
   .MICommandOpcode                     =      9,  \
   .CommandType                         =      0

struct GFX30_MI_MEM_FENCE {
   uint32_t                             FenceType;
#define FENCE_TYPE_RELEASE                       0
#define FENCE_TYPE_ACQUIRE                       1
#define FENCE_TYPE_MI_WRITE                      3
   uint32_t                             MICommandSubOpcode;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_MEM_FENCE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_MI_MEM_FENCE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->FenceType, 0, 1) |
      util_bitpack_uint(values->MICommandSubOpcode, 17, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_NOOP_length                   1
#define GFX30_MI_NOOP_length_bias              1
#define GFX30_MI_NOOP_header                    \
   .MICommandOpcode                     =      0,  \
   .CommandType                         =      0

struct GFX30_MI_NOOP {
   uint32_t                             IdentificationNumber;
   bool                                 IdentificationNumberRegisterWriteEnable;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_NOOP_pack(__attribute__((unused)) __gen_user_data *data,
                   __attribute__((unused)) void * restrict dst,
                   __attribute__((unused)) const struct GFX30_MI_NOOP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->IdentificationNumber, 0, 21) |
      util_bitpack_uint(values->IdentificationNumberRegisterWriteEnable, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_PREDICATE_length              1
#define GFX30_MI_PREDICATE_length_bias         1
#define GFX30_MI_PREDICATE_header               \
   .MICommandOpcode                     =     12,  \
   .CommandType                         =      0

struct GFX30_MI_PREDICATE {
   uint32_t                             CompareOperation;
#define COMPARE_TRUE                             0
#define COMPARE_FALSE                            1
#define COMPARE_SRCS_EQUAL                       2
#define COMPARE_DELTAS_EQUAL                     3
   uint32_t                             CombineOperation;
#define COMBINE_SET                              0
#define COMBINE_AND                              1
#define COMBINE_OR                               2
#define COMBINE_XOR                              3
   uint32_t                             LoadOperation;
#define LOAD_KEEP                                0
#define LOAD_LOAD                                2
#define LOAD_LOADINV                             3
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_PREDICATE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_MI_PREDICATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->CompareOperation, 0, 1) |
      util_bitpack_uint(values->CombineOperation, 3, 4) |
      util_bitpack_uint(values->LoadOperation, 6, 7) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_REPORT_HEAD_length            1
#define GFX30_MI_REPORT_HEAD_length_bias       1
#define GFX30_MI_REPORT_HEAD_header             \
   .MICommandOpcode                     =      7,  \
   .CommandType                         =      0

struct GFX30_MI_REPORT_HEAD {
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_REPORT_HEAD_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_MI_REPORT_HEAD * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_REPORT_PERF_COUNT_length      4
#define GFX30_MI_REPORT_PERF_COUNT_length_bias      2
#define GFX30_MI_REPORT_PERF_COUNT_header       \
   .DWordLength                         =      2,  \
   .MICommandOpcode                     =     40,  \
   .CommandType                         =      0

struct GFX30_MI_REPORT_PERF_COUNT {
   uint32_t                             DWordLength;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   bool                                 UseGlobalGTT;
   uint32_t                             CoreModeEnable;
   __gen_address_type                   MemoryAddress;
   uint32_t                             ReportID;
};

static inline __attribute__((always_inline)) void
GFX30_MI_REPORT_PERF_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MI_REPORT_PERF_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 5) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->UseGlobalGTT, 0, 0) |
      util_bitpack_uint(values->CoreModeEnable, 4, 4);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->MemoryAddress, v1, 6, 63);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   dw[3] =
      util_bitpack_uint(values->ReportID, 0, 31);
}

#define GFX30_MI_RS_CONTEXT_length             1
#define GFX30_MI_RS_CONTEXT_length_bias        1
#define GFX30_MI_RS_CONTEXT_header              \
   .MICommandOpcode                     =     15,  \
   .CommandType                         =      0

struct GFX30_MI_RS_CONTEXT {
   uint32_t                             ResourceStreamerSave;
#define RS_Restore                               0
#define RS_Save                                  1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_RS_CONTEXT_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_MI_RS_CONTEXT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ResourceStreamerSave, 0, 0) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_RS_CONTROL_length             1
#define GFX30_MI_RS_CONTROL_length_bias        1
#define GFX30_MI_RS_CONTROL_header              \
   .MICommandOpcode                     =      6,  \
   .CommandType                         =      0

struct GFX30_MI_RS_CONTROL {
   uint32_t                             ResourceStreamerControl;
#define RS_Stop                                  0
#define RS_Start                                 1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_RS_CONTROL_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_MI_RS_CONTROL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ResourceStreamerControl, 0, 0) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_RS_STORE_DATA_IMM_length      4
#define GFX30_MI_RS_STORE_DATA_IMM_length_bias      2
#define GFX30_MI_RS_STORE_DATA_IMM_header       \
   .DWordLength                         =      2,  \
   .MICommandOpcode                     =     43,  \
   .CommandType                         =      0

struct GFX30_MI_RS_STORE_DATA_IMM {
   uint32_t                             DWordLength;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             CoreModeEnable;
   __gen_address_type                   DestinationAddress;
   uint32_t                             DataDWord0;
};

static inline __attribute__((always_inline)) void
GFX30_MI_RS_STORE_DATA_IMM_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MI_RS_STORE_DATA_IMM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->CoreModeEnable, 0, 0);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->DestinationAddress, v1, 2, 63);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   dw[3] =
      util_bitpack_uint(values->DataDWord0, 0, 31);
}

#define GFX30_MI_SEMAPHORE_SIGNAL_length       2
#define GFX30_MI_SEMAPHORE_SIGNAL_length_bias      2
#define GFX30_MI_SEMAPHORE_SIGNAL_header        \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     27,  \
   .CommandType                         =      0

struct GFX30_MI_SEMAPHORE_SIGNAL {
   uint32_t                             DWordLength;
   uint32_t                             TargetEngineSelect;
#define RCS                                      0
#define VCS0                                     1
#define BCS                                      2
#define VECS                                     3
#define VCS1                                     4
#define VCS2                                     6
#define VCS3                                     7
#define VCS4                                     8
#define VCS5                                     9
#define VCS6                                     10
#define VCS7                                     11
#define VECS1                                    12
#define VECS2                                    13
#define VECS3                                    14
   bool                                 PostSyncOperation;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             TargetContextID;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SEMAPHORE_SIGNAL_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_MI_SEMAPHORE_SIGNAL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->TargetEngineSelect, 15, 18) |
      util_bitpack_uint(values->PostSyncOperation, 21, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->TargetContextID, 0, 31);
}

#define GFX30_MI_SEMAPHORE_WAIT_length         5
#define GFX30_MI_SEMAPHORE_WAIT_length_bias      2
#define GFX30_MI_SEMAPHORE_WAIT_header          \
   .DWordLength                         =      3,  \
   .MICommandOpcode                     =     28,  \
   .CommandType                         =      0

struct GFX30_MI_SEMAPHORE_WAIT {
   uint32_t                             DWordLength;
   uint32_t                             CompareOperation;
#define COMPARE_SAD_GREATER_THAN_SDD             0
#define COMPARE_SAD_GREATER_THAN_OR_EQUAL_SDD    1
#define COMPARE_SAD_LESS_THAN_SDD                2
#define COMPARE_SAD_LESS_THAN_OR_EQUAL_SDD       3
#define COMPARE_SAD_EQUAL_SDD                    4
#define COMPARE_SAD_NOT_EQUAL_SDD                5
   uint32_t                             WaitMode;
#define PollingMode                              1
#define SignalMode                               0
   bool                                 RegisterPollMode;
   bool                                 IndirectSemaphoreDataDword;
   bool                                 WorkloadPartitionIDOffsetEnable;
   uint32_t                             MemoryType;
#define PerProcessGraphicsAddress                0
#define GlobalGraphicsAddress                    1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             SemaphoreDataDword;
   __gen_address_type                   SemaphoreAddress;
   uint32_t                             WaitTokenNumber;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SEMAPHORE_WAIT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_MI_SEMAPHORE_WAIT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->CompareOperation, 12, 14) |
      util_bitpack_uint(values->WaitMode, 15, 15) |
      util_bitpack_uint(values->RegisterPollMode, 16, 16) |
      util_bitpack_uint(values->IndirectSemaphoreDataDword, 17, 17) |
      util_bitpack_uint(values->WorkloadPartitionIDOffsetEnable, 18, 18) |
      util_bitpack_uint(values->MemoryType, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->SemaphoreDataDword, 0, 31);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->SemaphoreAddress, 0, 2, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   dw[4] =
      util_bitpack_uint(values->WaitTokenNumber, 2, 9);
}

#define GFX30_MI_SET_APPID_length              1
#define GFX30_MI_SET_APPID_length_bias         1
#define GFX30_MI_SET_APPID_header               \
   .MICommandOpcode                     =     14,  \
   .CommandType                         =      0

struct GFX30_MI_SET_APPID {
   uint32_t                             ProtectedMemoryApplicationID;
   uint32_t                             ProtectedMemoryApplicationIDType;
#define DISPLAY_APP                              0
#define TRANSCODE_APP                            1
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SET_APPID_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_MI_SET_APPID * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->ProtectedMemoryApplicationID, 0, 6) |
      util_bitpack_uint(values->ProtectedMemoryApplicationIDType, 7, 7) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_SET_CONTEXT_length            2
#define GFX30_MI_SET_CONTEXT_length_bias       2
#define GFX30_MI_SET_CONTEXT_header             \
   .DWordLength                         =      0,  \
   .MICommandOpcode                     =     24,  \
   .CommandType                         =      0

struct GFX30_MI_SET_CONTEXT {
   uint32_t                             DWordLength;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             RestoreInhibit;
   uint32_t                             ForceRestore;
   bool                                 ResourceStreamerStateRestoreEnable;
   bool                                 ResourceStreamerStateSaveEnable;
   bool                                 CoreModeEnable;
   uint32_t                             ReservedMustbe1;
   __gen_address_type                   LogicalContextAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SET_CONTEXT_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_MI_SET_CONTEXT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint32_t v1 =
      util_bitpack_uint(values->RestoreInhibit, 0, 0) |
      util_bitpack_uint(values->ForceRestore, 1, 1) |
      util_bitpack_uint(values->ResourceStreamerStateRestoreEnable, 2, 2) |
      util_bitpack_uint(values->ResourceStreamerStateSaveEnable, 3, 3) |
      util_bitpack_uint(values->CoreModeEnable, 4, 4) |
      util_bitpack_uint(values->ReservedMustbe1, 8, 8);
   dw[1] = __gen_address(data, &dw[1], values->LogicalContextAddress, v1, 12, 31);
}

#define GFX30_MI_SET_PREDICATE_length          1
#define GFX30_MI_SET_PREDICATE_length_bias      1
#define GFX30_MI_SET_PREDICATE_header           \
   .MICommandOpcode                     =      1,  \
   .CommandType                         =      0

struct GFX30_MI_SET_PREDICATE {
   uint32_t                             PredicateEnable;
#define NOOPNever                                0
#define NOOPOnResult2Clear                       1
#define NOOPOnResult2Set                         2
#define NOOPOnResultClear                        3
#define NOOPOnResultSet                          4
#define NOOPAlways                               15
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SET_PREDICATE_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_MI_SET_PREDICATE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PredicateEnable, 0, 3) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_STORE_DATA_IMM_length         4
#define GFX30_MI_STORE_DATA_IMM_length_bias      2
#define GFX30_MI_STORE_DATA_IMM_header          \
   .DWordLength                         =      2,  \
   .MICommandOpcode                     =     32,  \
   .CommandType                         =      0

struct GFX30_MI_STORE_DATA_IMM {
   uint32_t                             DWordLength;
   bool                                 ForceWriteCompletionCheck;
   uint32_t                             StoreQword;
   bool                                 UseGlobalGTT;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             CoreModeEnable;
   __gen_address_type                   Address;
   uint64_t                             ImmediateData;
};

static inline __attribute__((always_inline)) void
GFX30_MI_STORE_DATA_IMM_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_MI_STORE_DATA_IMM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 9) |
      util_bitpack_uint(values->ForceWriteCompletionCheck, 10, 10) |
      util_bitpack_uint(values->StoreQword, 21, 21) |
      util_bitpack_uint(values->UseGlobalGTT, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->CoreModeEnable, 0, 0);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->Address, v1, 2, 47);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   const uint64_t v3 =
      util_bitpack_uint(values->ImmediateData, 0, 63);
   dw[3] = v3;
   dw[4] = v3 >> 32;
}

#define GFX30_MI_STORE_DATA_INDEX_length       3
#define GFX30_MI_STORE_DATA_INDEX_length_bias      2
#define GFX30_MI_STORE_DATA_INDEX_header        \
   .DWordLength                         =      1,  \
   .MICommandOpcode                     =     33,  \
   .CommandType                         =      0

struct GFX30_MI_STORE_DATA_INDEX {
   uint32_t                             DWordLength;
   uint32_t                             UsePerProcessHardwareStatusPage;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint32_t                             Offset;
   uint32_t                             DataDWord0;
   uint32_t                             DataDWord1;
};

static inline __attribute__((always_inline)) void
GFX30_MI_STORE_DATA_INDEX_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_MI_STORE_DATA_INDEX * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->UsePerProcessHardwareStatusPage, 21, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->Offset, 2, 11);

   dw[2] =
      util_bitpack_uint(values->DataDWord0, 0, 31);
}

#define GFX30_MI_STORE_REGISTER_MEM_length      4
#define GFX30_MI_STORE_REGISTER_MEM_length_bias      2
#define GFX30_MI_STORE_REGISTER_MEM_header      \
   .DWordLength                         =      2,  \
   .MICommandOpcode                     =     36,  \
   .CommandType                         =      0

struct GFX30_MI_STORE_REGISTER_MEM {
   uint32_t                             DWordLength;
   uint32_t                             AddCSMMIOStartOffset;
   bool                                 PredicateEnable;
   bool                                 UseGlobalGTT;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
   uint64_t                             RegisterAddress;
   __gen_address_type                   MemoryAddress;
};

static inline __attribute__((always_inline)) void
GFX30_MI_STORE_REGISTER_MEM_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_MI_STORE_REGISTER_MEM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->AddCSMMIOStartOffset, 19, 19) |
      util_bitpack_uint(values->PredicateEnable, 21, 21) |
      util_bitpack_uint(values->UseGlobalGTT, 22, 22) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      __gen_offset(values->RegisterAddress, 2, 22);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->MemoryAddress, 0, 2, 63);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;
}

#define GFX30_MI_SUSPEND_FLUSH_length          1
#define GFX30_MI_SUSPEND_FLUSH_length_bias      1
#define GFX30_MI_SUSPEND_FLUSH_header           \
   .MICommandOpcode                     =     11,  \
   .CommandType                         =      0

struct GFX30_MI_SUSPEND_FLUSH {
   bool                                 SuspendFlush;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_SUSPEND_FLUSH_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_MI_SUSPEND_FLUSH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->SuspendFlush, 0, 0) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_TOPOLOGY_FILTER_length        1
#define GFX30_MI_TOPOLOGY_FILTER_length_bias      1
#define GFX30_MI_TOPOLOGY_FILTER_header         \
   .MICommandOpcode                     =     13,  \
   .CommandType                         =      0

struct GFX30_MI_TOPOLOGY_FILTER {
   enum GFX30_3D_Prim_Topo_Type         TopologyFilterValue;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_TOPOLOGY_FILTER_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_MI_TOPOLOGY_FILTER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TopologyFilterValue, 0, 5) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_USER_INTERRUPT_length         1
#define GFX30_MI_USER_INTERRUPT_length_bias      1
#define GFX30_MI_USER_INTERRUPT_header          \
   .MICommandOpcode                     =      2,  \
   .CommandType                         =      0

struct GFX30_MI_USER_INTERRUPT {
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_USER_INTERRUPT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_MI_USER_INTERRUPT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_WAIT_FOR_EVENT_length         1
#define GFX30_MI_WAIT_FOR_EVENT_length_bias      1
#define GFX30_MI_WAIT_FOR_EVENT_header          \
   .MICommandOpcode                     =      3,  \
   .CommandType                         =      0

struct GFX30_MI_WAIT_FOR_EVENT {
   bool                                 DisplayPlnae1AScanLineWaitEnable;
   bool                                 DisplayPlane1FlipPendingWaitEnable;
   bool                                 DisplayPlane4FlipPendingWaitEnable;
   bool                                 DisplayPlane1AVerticalBlankWaitEnable;
   bool                                 DisplayPlane7FlipPendingWaitEnable;
   bool                                 DisplayPlane8FlipPendingWaitEnable;
   bool                                 DisplayPlane1BScanLineWaitEnable;
   bool                                 DisplayPlane2FlipPendingWaitEnable;
   bool                                 DisplayPlane5FlipPendingWaitEnable;
   bool                                 DisplayPlane1BVerticalBlankWaitEnable;
   bool                                 DisplayPlane1CScanLineWaitEnable;
   bool                                 DisplayPlane3FlipPendingWaitEnable;
   bool                                 DisplayPlane9FlipPendingWaitEnable;
   bool                                 DisplayPlane10FlipPendingWaitEnable;
   bool                                 DisplayPlane11FlipPendingWaitEnable;
   bool                                 DisplayPlane12FlipPendingWaitEnable;
   bool                                 DisplayPlane6FlipPendingWaitEnable;
   bool                                 DisplayPlane1CVerticalBlankWaitEnable;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_WAIT_FOR_EVENT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_MI_WAIT_FOR_EVENT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DisplayPlnae1AScanLineWaitEnable, 0, 0) |
      util_bitpack_uint(values->DisplayPlane1FlipPendingWaitEnable, 1, 1) |
      util_bitpack_uint(values->DisplayPlane4FlipPendingWaitEnable, 2, 2) |
      util_bitpack_uint(values->DisplayPlane1AVerticalBlankWaitEnable, 3, 3) |
      util_bitpack_uint(values->DisplayPlane7FlipPendingWaitEnable, 6, 6) |
      util_bitpack_uint(values->DisplayPlane8FlipPendingWaitEnable, 7, 7) |
      util_bitpack_uint(values->DisplayPlane1BScanLineWaitEnable, 8, 8) |
      util_bitpack_uint(values->DisplayPlane2FlipPendingWaitEnable, 9, 9) |
      util_bitpack_uint(values->DisplayPlane5FlipPendingWaitEnable, 10, 10) |
      util_bitpack_uint(values->DisplayPlane1BVerticalBlankWaitEnable, 11, 11) |
      util_bitpack_uint(values->DisplayPlane1CScanLineWaitEnable, 14, 14) |
      util_bitpack_uint(values->DisplayPlane3FlipPendingWaitEnable, 15, 15) |
      util_bitpack_uint(values->DisplayPlane9FlipPendingWaitEnable, 16, 16) |
      util_bitpack_uint(values->DisplayPlane10FlipPendingWaitEnable, 17, 17) |
      util_bitpack_uint(values->DisplayPlane11FlipPendingWaitEnable, 18, 18) |
      util_bitpack_uint(values->DisplayPlane12FlipPendingWaitEnable, 19, 19) |
      util_bitpack_uint(values->DisplayPlane6FlipPendingWaitEnable, 20, 20) |
      util_bitpack_uint(values->DisplayPlane1CVerticalBlankWaitEnable, 21, 21) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_MI_WAIT_FOR_EVENT_2_length       1
#define GFX30_MI_WAIT_FOR_EVENT_2_length_bias      1
#define GFX30_MI_WAIT_FOR_EVENT_2_header        \
   .MICommandOpcode                     =      4,  \
   .CommandType                         =      0

struct GFX30_MI_WAIT_FOR_EVENT_2 {
   uint32_t                             DisplayPlaneFlipPendingWaitEnable;
   uint32_t                             DisplayPipeVerticalBlankWaitEnable;
   uint32_t                             DisplayPipeScanLineWaitEnable;
   uint32_t                             MICommandOpcode;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_MI_WAIT_FOR_EVENT_2_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_MI_WAIT_FOR_EVENT_2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DisplayPlaneFlipPendingWaitEnable, 0, 5) |
      util_bitpack_uint(values->DisplayPipeVerticalBlankWaitEnable, 8, 10) |
      util_bitpack_uint(values->DisplayPipeScanLineWaitEnable, 12, 14) |
      util_bitpack_uint(values->MICommandOpcode, 23, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_PIPELINE_SELECT_length           1
#define GFX30_PIPELINE_SELECT_length_bias      1
#define GFX30_PIPELINE_SELECT_header            \
   ._3DCommandSubOpcode                 =      4,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      1,  \
   .CommandType                         =      3

struct GFX30_PIPELINE_SELECT {
   uint32_t                             PipelineSelection;
#define _3D                                      0
#define Media                                    1
#define GPGPU                                    2
   uint32_t                             MaskBits;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
};

static inline __attribute__((always_inline)) void
GFX30_PIPELINE_SELECT_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_PIPELINE_SELECT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PipelineSelection, 0, 1) |
      util_bitpack_uint(values->MaskBits, 8, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);
}

#define GFX30_PIPE_CONTROL_length              6
#define GFX30_PIPE_CONTROL_length_bias         2
#define GFX30_PIPE_CONTROL_header               \
   .DWordLength                         =      4,  \
   ._3DCommandSubOpcode                 =      0,  \
   ._3DCommandOpcode                    =      2,  \
   .CommandSubType                      =      3,  \
   .CommandType                         =      3

struct GFX30_PIPE_CONTROL {
   uint32_t                             DWordLength;
   bool                                 HDCPipelineFlushEnable;
   bool                                 L3ReadOnlyCacheInvalidationEnable;
   bool                                 UntypedDataPortCacheFlushEnable;
   bool                                 CCSFlushEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 DepthCacheFlushEnable;
   bool                                 StallAtPixelScoreboard;
   bool                                 StateCacheInvalidationEnable;
   bool                                 ConstantCacheInvalidationEnable;
   bool                                 VFCacheInvalidationEnable;
   bool                                 ForceDeviceCoherency;
   bool                                 PipeControlFlushEnable;
   bool                                 NotifyEnable;
   bool                                 IndirectStatePointersDisable;
   bool                                 TextureCacheInvalidationEnable;
   bool                                 InstructionCacheInvalidateEnable;
   bool                                 RenderTargetCacheFlushEnable;
   bool                                 DepthStallEnable;
   uint32_t                             PostSyncOperation;
#define NoWrite                                  0
#define WriteImmediateData                       1
#define WritePSDepthCount                        2
#define WriteTimestamp                           3
   bool                                 PSSStallSyncEnable;
   bool                                 TLBInvalidate;
   bool                                 DepthStallSyncEnable;
   bool                                 CommandStreamerStallEnable;
   uint32_t                             StoreDataIndex;
   bool                                 ProtectedMemoryEnable;
   uint32_t                             LRIPostSyncOperation;
#define NoLRIOperation                           0
#define MMIOWriteImmediateData                   1
   uint32_t                             DestinationAddressType;
#define DAT_PPGTT                                0
#define DAT_GGTT                                 1
   bool                                 FlushLLC;
   bool                                 ProtectedMemoryDisable;
   bool                                 CommandCacheInvalidateEnable;
   __gen_address_type                   Address;
   uint64_t                             ImmediateData;
};

static inline __attribute__((always_inline)) void
GFX30_PIPE_CONTROL_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_PIPE_CONTROL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->HDCPipelineFlushEnable, 9, 9) |
      util_bitpack_uint(values->L3ReadOnlyCacheInvalidationEnable, 10, 10) |
      util_bitpack_uint(values->UntypedDataPortCacheFlushEnable, 11, 11) |
      util_bitpack_uint(values->CCSFlushEnable, 13, 13) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DepthCacheFlushEnable, 0, 0) |
      util_bitpack_uint(values->StallAtPixelScoreboard, 1, 1) |
      util_bitpack_uint(values->StateCacheInvalidationEnable, 2, 2) |
      util_bitpack_uint(values->ConstantCacheInvalidationEnable, 3, 3) |
      util_bitpack_uint(values->VFCacheInvalidationEnable, 4, 4) |
      util_bitpack_uint(values->ForceDeviceCoherency, 5, 5) |
      util_bitpack_uint(values->PipeControlFlushEnable, 7, 7) |
      util_bitpack_uint(values->NotifyEnable, 8, 8) |
      util_bitpack_uint(values->IndirectStatePointersDisable, 9, 9) |
      util_bitpack_uint(values->TextureCacheInvalidationEnable, 10, 10) |
      util_bitpack_uint(values->InstructionCacheInvalidateEnable, 11, 11) |
      util_bitpack_uint(values->RenderTargetCacheFlushEnable, 12, 12) |
      util_bitpack_uint(values->DepthStallEnable, 13, 13) |
      util_bitpack_uint(values->PostSyncOperation, 14, 15) |
      util_bitpack_uint(values->PSSStallSyncEnable, 17, 17) |
      util_bitpack_uint(values->TLBInvalidate, 18, 18) |
      util_bitpack_uint(values->DepthStallSyncEnable, 19, 19) |
      util_bitpack_uint(values->CommandStreamerStallEnable, 20, 20) |
      util_bitpack_uint(values->StoreDataIndex, 21, 21) |
      util_bitpack_uint(values->ProtectedMemoryEnable, 22, 22) |
      util_bitpack_uint(values->LRIPostSyncOperation, 23, 23) |
      util_bitpack_uint(values->DestinationAddressType, 24, 24) |
      util_bitpack_uint(values->FlushLLC, 26, 26) |
      util_bitpack_uint(values->ProtectedMemoryDisable, 27, 27) |
      util_bitpack_uint(values->CommandCacheInvalidateEnable, 29, 29);

   const uint64_t v2_address =
      __gen_address(data, &dw[2], values->Address, 0, 2, 47);
   dw[2] = v2_address;
   dw[3] = v2_address >> 32;

   const uint64_t v4 =
      util_bitpack_uint(values->ImmediateData, 0, 63);
   dw[4] = v4;
   dw[5] = v4 >> 32;
}

#define GFX30_RESOURCE_BARRIER_length          5
#define GFX30_RESOURCE_BARRIER_length_bias      2
#define GFX30_RESOURCE_BARRIER_header           \
   .DWordLength                         =      3,  \
   .Opcode                              =      3,  \
   .CommandType                         =      5

struct GFX30_RESOURCE_BARRIER {
   uint32_t                             DWordLength;
   bool                                 PredicateEnable;
   uint32_t                             Opcode;
#define RESOURCE_BARRIER                         3
   uint32_t                             CommandType;
   struct GFX30_RESOURCE_BARRIER_BODY   ResourceBarrierBody;
};

static inline __attribute__((always_inline)) void
GFX30_RESOURCE_BARRIER_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_RESOURCE_BARRIER * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->PredicateEnable, 24, 24) |
      util_bitpack_uint(values->Opcode, 26, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   GFX30_RESOURCE_BARRIER_BODY_pack(data, &dw[1], &values->ResourceBarrierBody);
}

#define GFX30_STATE_BASE_ADDRESS_length       22
#define GFX30_STATE_BASE_ADDRESS_length_bias      2
#define GFX30_STATE_BASE_ADDRESS_header         \
   .DWordLength                         =     20,  \
   ._3DCommandSubOpcode                 =      1,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3

struct GFX30_STATE_BASE_ADDRESS {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   bool                                 GeneralStateBaseAddressModifyEnable;
   uint32_t                             GeneralStateMOCS;
   __gen_address_type                   GeneralStateBaseAddress;
   uint32_t                             StatelessDataPortAccessMOCS;
   uint32_t                             L1CacheControl;
#define L1CC_WBP                                 0
#define L1CC_UC                                  1
#define L1CC_WB                                  2
#define L1CC_WT                                  3
#define L1CC_WS                                  4
   bool                                 SurfaceStateBaseAddressModifyEnable;
   uint32_t                             SurfaceStateMOCS;
   __gen_address_type                   SurfaceStateBaseAddress;
   bool                                 DynamicStateBaseAddressModifyEnable;
   uint32_t                             DynamicStateMOCS;
   __gen_address_type                   DynamicStateBaseAddress;
   bool                                 IndirectObjectBaseAddressModifyEnable;
   uint32_t                             IndirectObjectMOCS;
   __gen_address_type                   IndirectObjectBaseAddress;
   bool                                 InstructionBaseAddressModifyEnable;
   uint32_t                             InstructionMOCS;
   __gen_address_type                   InstructionBaseAddress;
   bool                                 GeneralStateBufferSizeModifyEnable;
   uint32_t                             GeneralStateBufferSize;
   bool                                 DynamicStateBufferSizeModifyEnable;
   uint32_t                             DynamicStateBufferSize;
   bool                                 IndirectObjectBufferSizeModifyEnable;
   uint32_t                             IndirectObjectBufferSize;
   bool                                 InstructionBuffersizeModifyEnable;
   uint32_t                             InstructionBufferSize;
   bool                                 BindlessSurfaceStateBaseAddressModifyEnable;
   uint32_t                             BindlessSurfaceStateMOCS;
   __gen_address_type                   BindlessSurfaceStateBaseAddress;
   uint32_t                             BindlessSurfaceStateSize;
   bool                                 BindlessSamplerStateBaseAddressModifyEnable;
   uint32_t                             BindlessSamplerStateMOCS;
   __gen_address_type                   BindlessSamplerStateBaseAddress;
   uint32_t                             BindlessSamplerStateBufferSize;
};

static inline __attribute__((always_inline)) void
GFX30_STATE_BASE_ADDRESS_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_STATE_BASE_ADDRESS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      util_bitpack_uint(values->GeneralStateBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->GeneralStateMOCS, 4, 10);
   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->GeneralStateBaseAddress, v1, 12, 63);
   dw[1] = v1_address;
   dw[2] = (v1_address >> 32) | (v1 >> 32);

   assert(values->L1CacheControl != L1CC_WBP);
   assert(values->L1CacheControl != L1CC_UC);
   dw[3] =
      util_bitpack_uint_nonzero(values->StatelessDataPortAccessMOCS, 16, 22) |
      util_bitpack_uint(values->L1CacheControl, 23, 25);

   const uint64_t v4 =
      util_bitpack_uint(values->SurfaceStateBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->SurfaceStateMOCS, 4, 10);
   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->SurfaceStateBaseAddress, v4, 12, 63);
   dw[4] = v4_address;
   dw[5] = (v4_address >> 32) | (v4 >> 32);

   const uint64_t v6 =
      util_bitpack_uint(values->DynamicStateBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->DynamicStateMOCS, 4, 10);
   const uint64_t v6_address =
      __gen_address(data, &dw[6], values->DynamicStateBaseAddress, v6, 12, 63);
   dw[6] = v6_address;
   dw[7] = (v6_address >> 32) | (v6 >> 32);

   const uint64_t v8 =
      util_bitpack_uint(values->IndirectObjectBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->IndirectObjectMOCS, 4, 10);
   const uint64_t v8_address =
      __gen_address(data, &dw[8], values->IndirectObjectBaseAddress, v8, 12, 63);
   dw[8] = v8_address;
   dw[9] = (v8_address >> 32) | (v8 >> 32);

   const uint64_t v10 =
      util_bitpack_uint(values->InstructionBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->InstructionMOCS, 4, 10);
   const uint64_t v10_address =
      __gen_address(data, &dw[10], values->InstructionBaseAddress, v10, 12, 63);
   dw[10] = v10_address;
   dw[11] = (v10_address >> 32) | (v10 >> 32);

   dw[12] =
      util_bitpack_uint(values->GeneralStateBufferSizeModifyEnable, 0, 0) |
      util_bitpack_uint(values->GeneralStateBufferSize, 12, 31);

   dw[13] =
      util_bitpack_uint(values->DynamicStateBufferSizeModifyEnable, 0, 0) |
      util_bitpack_uint(values->DynamicStateBufferSize, 12, 31);

   dw[14] =
      util_bitpack_uint(values->IndirectObjectBufferSizeModifyEnable, 0, 0) |
      util_bitpack_uint(values->IndirectObjectBufferSize, 12, 31);

   dw[15] =
      util_bitpack_uint(values->InstructionBuffersizeModifyEnable, 0, 0) |
      util_bitpack_uint(values->InstructionBufferSize, 12, 31);

   const uint64_t v16 =
      util_bitpack_uint(values->BindlessSurfaceStateBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->BindlessSurfaceStateMOCS, 4, 10);
   const uint64_t v16_address =
      __gen_address(data, &dw[16], values->BindlessSurfaceStateBaseAddress, v16, 12, 63);
   dw[16] = v16_address;
   dw[17] = (v16_address >> 32) | (v16 >> 32);

   dw[18] =
      util_bitpack_uint(values->BindlessSurfaceStateSize, 0, 31);

   const uint64_t v19 =
      util_bitpack_uint(values->BindlessSamplerStateBaseAddressModifyEnable, 0, 0) |
      util_bitpack_uint_nonzero(values->BindlessSamplerStateMOCS, 4, 10);
   const uint64_t v19_address =
      __gen_address(data, &dw[19], values->BindlessSamplerStateBaseAddress, v19, 12, 63);
   dw[19] = v19_address;
   dw[20] = (v19_address >> 32) | (v19 >> 32);

   dw[21] =
      util_bitpack_uint(values->BindlessSamplerStateBufferSize, 12, 31);
}

#define GFX30_STATE_BYTE_STRIDE_length         2
#define GFX30_STATE_BYTE_STRIDE_length_bias      2
#define GFX30_STATE_BYTE_STRIDE_header          \
   .DWordLength                         =      0,  \
   ._3DCommandSubOpcode                 =      5,  \
   ._3DCommandOpcode                    =      0,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3

struct GFX30_STATE_BYTE_STRIDE {
   uint32_t                             DWordLength;
   bool                                 ByteStrideEnable;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ByteStride;
};

static inline __attribute__((always_inline)) void
GFX30_STATE_BYTE_STRIDE_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_STATE_BYTE_STRIDE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->ByteStrideEnable, 8, 8) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ByteStride, 2, 31);
}

#define GFX30_STATE_COMPUTE_MODE_length        3
#define GFX30_STATE_COMPUTE_MODE_length_bias      2
#define GFX30_STATE_COMPUTE_MODE_header         \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =      5,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3

struct GFX30_STATE_COMPUTE_MODE {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint32_t                             ZPassAsyncComputeThreadLimit;
#define ZPACTL_Max60                             0
#define ZPACTL_Max64                             1
#define ZPACTL_Max56                             2
#define ZPACTL_Max48                             3
#define ZPACTL_Max40                             4
#define ZPACTL_Max32                             5
   uint32_t                             ZAsyncThrottlesettings;
#define ZATS_DefertoAsyncComputeThreadLimit      0
#define ZATS_Max32                               1
#define ZATS_Max40                               2
#define ZATS_Max48                               3
   uint32_t                             AsyncComputeThreadLimit;
#define ACTL_Disabled                            0
#define ACTL_Max2                                1
#define ACTL_Max8                                2
#define ACTL_Max16                               3
#define ACTL_Max24                               4
#define ACTL_Max32                               5
#define ACTL_Max40                               6
#define ACTL_Max48                               7
   bool                                 EnableVariableRegisterSizeAllocation;
   uint32_t                             EUThreadSchedulingModeOverride;
   bool                                 LargeGRFMode;
   uint32_t                             ZPassAsyncComputeThreadLimitMask;
   uint32_t                             Mask1;
   uint32_t                             ZAsyncThrottlesettingsMask;
   uint32_t                             AsyncComputeThreadLimitMask;
   uint32_t                             EnableVariableRegisterSizeAllocationMask;
   uint32_t                             EUThreadSchedulingModeOverrideMask;
   bool                                 LargeGRFModeMask;
   uint32_t                             MidthreadPreemptionDelayTimer;
#define MTP_TIMER_VAL_0                          0
#define MTP_TIMER_VAL_50                         1
#define MTP_TIMER_VAL_100                        2
#define MTP_TIMER_VAL_150                        3
   uint32_t                             MidthreadPreemptionOverdispatchThreadgroupcount;
#define OD_TG_M2                                 0
#define OD_TG_M4                                 1
#define OD_TG_M8                                 2
#define OD_TG_M16                                3
   uint32_t                             MidthreadPreemptionOverdispatchTestmode;
#define Regular                                  0
#define Testmode                                 1
   uint32_t                             UAVCoherencyMode;
#define DrainDataPortMode                        0
#define FlushDataportL1                          1
   uint32_t                             MidthreadPreemptionDelayTimerMask;
   uint32_t                             Mask2;
   uint32_t                             MidthreadPreemptionOverdispatchThreadgroupcountMask;
   uint32_t                             MidthreadPreemptionOverdispatchTestmodeMask;
   uint32_t                             UAVCoherencyModeMask;
   uint32_t                             MemoryallocationforScratchandMidthreadPreemptionbuffersMask;
};

static inline __attribute__((always_inline)) void
GFX30_STATE_COMPUTE_MODE_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_STATE_COMPUTE_MODE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   dw[1] =
      util_bitpack_uint(values->ZPassAsyncComputeThreadLimit, 0, 2) |
      util_bitpack_uint(values->ZAsyncThrottlesettings, 3, 4) |
      util_bitpack_uint(values->AsyncComputeThreadLimit, 7, 9) |
      util_bitpack_uint(values->EnableVariableRegisterSizeAllocation, 10, 10) |
      util_bitpack_uint(values->EUThreadSchedulingModeOverride, 13, 14) |
      util_bitpack_uint(values->LargeGRFMode, 15, 15) |
      util_bitpack_uint(values->ZPassAsyncComputeThreadLimitMask, 16, 18) |
      util_bitpack_uint(values->Mask1, 16, 31) |
      util_bitpack_uint(values->ZAsyncThrottlesettingsMask, 19, 20) |
      util_bitpack_uint(values->AsyncComputeThreadLimitMask, 23, 25) |
      util_bitpack_uint(values->EnableVariableRegisterSizeAllocationMask, 26, 26) |
      util_bitpack_uint(values->EUThreadSchedulingModeOverrideMask, 29, 30) |
      util_bitpack_uint(values->LargeGRFModeMask, 31, 31);

   dw[2] =
      util_bitpack_uint(values->MidthreadPreemptionDelayTimer, 0, 2) |
      util_bitpack_uint(values->MidthreadPreemptionOverdispatchThreadgroupcount, 3, 4) |
      util_bitpack_uint(values->MidthreadPreemptionOverdispatchTestmode, 5, 5) |
      util_bitpack_uint(values->UAVCoherencyMode, 6, 6) |
      util_bitpack_uint(values->MidthreadPreemptionDelayTimerMask, 16, 18) |
      util_bitpack_uint(values->Mask2, 16, 31) |
      util_bitpack_uint(values->MidthreadPreemptionOverdispatchThreadgroupcountMask, 19, 20) |
      util_bitpack_uint(values->MidthreadPreemptionOverdispatchTestmodeMask, 21, 21) |
      util_bitpack_uint(values->UAVCoherencyModeMask, 22, 22) |
      util_bitpack_uint(values->MemoryallocationforScratchandMidthreadPreemptionbuffersMask, 27, 27);
}

#define GFX30_STATE_SIP_length                 3
#define GFX30_STATE_SIP_length_bias            2
#define GFX30_STATE_SIP_header                  \
   .DWordLength                         =      1,  \
   ._3DCommandSubOpcode                 =      2,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3

struct GFX30_STATE_SIP {
   uint32_t                             DWordLength;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   uint64_t                             SystemInstructionPointer;
};

static inline __attribute__((always_inline)) void
GFX30_STATE_SIP_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_STATE_SIP * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1 =
      __gen_offset(values->SystemInstructionPointer, 4, 63);
   dw[1] = v1;
   dw[2] = v1 >> 32;
}

#define GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS_length      3
#define GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS_length_bias      2
#define GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS_header\
   .DWordLength                         =      1,  \
   .ContextRestoreInvalid               =      0,  \
   ._3DCommandSubOpcode                 =      9,  \
   ._3DCommandOpcode                    =      1,  \
   .CommandSubType                      =      0,  \
   .CommandType                         =      3

struct GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS {
   uint32_t                             DWordLength;
   bool                                 ContextRestoreInvalid;
   uint32_t                             _3DCommandSubOpcode;
   uint32_t                             _3DCommandOpcode;
   uint32_t                             CommandSubType;
   uint32_t                             CommandType;
   __gen_address_type                   SystemMemoryFenceAddress;
};

static inline __attribute__((always_inline)) void
GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS_pack(__attribute__((unused)) __gen_user_data *data,
                                          __attribute__((unused)) void * restrict dst,
                                          __attribute__((unused)) const struct GFX30_STATE_SYSTEM_MEM_FENCE_ADDRESS * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->ContextRestoreInvalid, 15, 15) |
      util_bitpack_uint(values->_3DCommandSubOpcode, 16, 23) |
      util_bitpack_uint(values->_3DCommandOpcode, 24, 26) |
      util_bitpack_uint(values->CommandSubType, 27, 28) |
      util_bitpack_uint(values->CommandType, 29, 31);

   const uint64_t v1_address =
      __gen_address(data, &dw[1], values->SystemMemoryFenceAddress, 0, 12, 63);
   dw[1] = v1_address;
   dw[2] = v1_address >> 32;
}

#define GFX30_XY_BLOCK_COPY_BLT_length        22
#define GFX30_XY_BLOCK_COPY_BLT_length_bias      2
#define GFX30_XY_BLOCK_COPY_BLT_header          \
   .DWordLength                         =     20,  \
   .Opcode                              =     65,  \
   .Client                              =      2

struct GFX30_XY_BLOCK_COPY_BLT {
   uint32_t                             DWordLength;
   uint32_t                             SpecialModeofOperation;
#define NONE                                     0
#define FULL_RESOLVE                             1
#define PARTIAL_RESOLVE                          2
   uint32_t                             ColorDepth;
#define XY_BPP_8_BIT                             0
#define XY_BPP_16_BIT                            1
#define XY_BPP_32_BIT                            2
#define XY_BPP_64_BIT                            3
#define XY_BPP_96_BIT                            4
#define XY_BPP_128_BIT                           5
   uint32_t                             Opcode;
   uint32_t                             Client;
   uint32_t                             DestinationPitch;
   uint32_t                             DestinationEncryptEn;
   uint32_t                             DestinationMOCSindex;
   uint32_t                             DestinationTiling;
#define XY_TILE_LINEAR                           0
#define XY_TILE_X                                1
#define XY_TILE_4                                2
#define XY_TILE_64                               3
   int32_t                              DestinationX1;
   int32_t                              DestinationY1;
   int32_t                              DestinationX2;
   int32_t                              DestinationY2;
   __gen_address_type                   DestinationBaseAddress;
   uint32_t                             DestinationXOffset;
   uint32_t                             DestinationYOffset;
   uint32_t                             DestinationTargetMemory;
#define XY_MEM_LOCAL                             0
#define XY_MEM_SYSTEM                            1
   int32_t                              SourceX1;
   int32_t                              SourceY1;
   uint32_t                             SourcePitch;
   bool                                 SourceEncryptEn;
   uint32_t                             SourceMOCSindex;
   uint32_t                             SourceTiling;
#define XY_TILE_LINEAR                           0
#define XY_TILE_X                                1
#define XY_TILE_4                                2
#define XY_TILE_64                               3
   __gen_address_type                   SourceBaseAddress;
   uint32_t                             SourceXOffset;
   uint32_t                             SourceYOffset;
   uint32_t                             SourceTargetMemory;
#define XY_MEM_LOCAL                             0
#define XY_MEM_SYSTEM                            1
   uint32_t                             SourceCompressionFormat;
   bool                                 SourceClearValueEnable;
   __gen_address_type                   SourceClearAddress;
   uint32_t                             DestinationCompressionFormat;
   bool                                 DestinationClearValueEnable;
   __gen_address_type                   DestinationClearAddress;
   uint32_t                             DestinationSurfaceHeight;
   uint32_t                             DestinationSurfaceWidth;
   uint32_t                             DestinationSurfaceType;
#define XY_SURFTYPE_1D                           0
#define XY_SURFTYPE_2D                           1
#define XY_SURFTYPE_3D                           2
#define XY_SURFTYPE_CUBE                         3
   uint32_t                             DestinationLOD;
   uint32_t                             DestinationSurfaceQPitch;
   uint32_t                             DestinationSurfaceDepth;
   uint32_t                             DestinationHorizontalAlign;
#define HALIGN_16                                0
#define HALIGN_32                                1
#define HALIGN_64                                2
#define HALIGN_128                               3
   uint32_t                             DestinationVerticalAlign;
#define VALIGN_4                                 1
#define VALIGN_8                                 2
#define VALIGN_16                                3
   uint32_t                             DestinationMipTailStartLOD;
   uint32_t                             DestinationArrayIndex;
   uint32_t                             SourceSurfaceHeight;
   uint32_t                             SourceSurfaceWidth;
   uint32_t                             SourceSurfaceType;
#define XY_SURFTYPE_1D                           0
#define XY_SURFTYPE_2D                           1
#define XY_SURFTYPE_3D                           2
#define XY_SURFTYPE_CUBE                         3
   uint32_t                             SourceLOD;
   uint32_t                             SourceSurfaceQPitch;
   uint32_t                             SourceSurfaceDepth;
   uint32_t                             SourceHorizontalAlign;
#define HALIGN_16                                0
#define HALIGN_32                                1
#define HALIGN_64                                2
#define HALIGN_128                               3
   uint32_t                             SourceVerticalAlign;
#define VALIGN_4                                 1
#define VALIGN_8                                 2
#define VALIGN_16                                3
   uint32_t                             SourceMipTailStartLOD;
   uint32_t                             SourceArrayIndex;
};

static inline __attribute__((always_inline)) void
GFX30_XY_BLOCK_COPY_BLT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_XY_BLOCK_COPY_BLT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->SpecialModeofOperation, 12, 13) |
      util_bitpack_uint(values->ColorDepth, 19, 21) |
      util_bitpack_uint(values->Opcode, 22, 28) |
      util_bitpack_uint(values->Client, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DestinationPitch, 0, 17) |
      util_bitpack_uint(values->DestinationEncryptEn, 21, 21) |
      util_bitpack_uint_nonzero(values->DestinationMOCSindex, 24, 27) |
      util_bitpack_uint(values->DestinationTiling, 30, 31);

   dw[2] =
      util_bitpack_sint(values->DestinationX1, 0, 15) |
      util_bitpack_sint(values->DestinationY1, 16, 31);

   dw[3] =
      util_bitpack_sint(values->DestinationX2, 0, 15) |
      util_bitpack_sint(values->DestinationY2, 16, 31);

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->DestinationBaseAddress, 0, 0, 63);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;

   dw[6] =
      util_bitpack_uint(values->DestinationXOffset, 0, 13) |
      util_bitpack_uint(values->DestinationYOffset, 16, 29) |
      util_bitpack_uint(values->DestinationTargetMemory, 31, 31);

   dw[7] =
      util_bitpack_sint(values->SourceX1, 0, 15) |
      util_bitpack_sint(values->SourceY1, 16, 31);

   dw[8] =
      util_bitpack_uint(values->SourcePitch, 0, 17) |
      util_bitpack_uint(values->SourceEncryptEn, 21, 21) |
      util_bitpack_uint(values->SourceMOCSindex, 24, 27) |
      util_bitpack_uint(values->SourceTiling, 30, 31);

   const uint64_t v9_address =
      __gen_address(data, &dw[9], values->SourceBaseAddress, 0, 0, 63);
   dw[9] = v9_address;
   dw[10] = v9_address >> 32;

   dw[11] =
      util_bitpack_uint(values->SourceXOffset, 0, 13) |
      util_bitpack_uint(values->SourceYOffset, 16, 29) |
      util_bitpack_uint(values->SourceTargetMemory, 31, 31);

   const uint64_t v12 =
      util_bitpack_uint(values->SourceCompressionFormat, 0, 4) |
      util_bitpack_uint(values->SourceClearValueEnable, 5, 5);
   const uint64_t v12_address =
      __gen_address(data, &dw[12], values->SourceClearAddress, v12, 6, 47);
   dw[12] = v12_address;
   dw[13] = (v12_address >> 32) | (v12 >> 32);

   const uint64_t v14 =
      util_bitpack_uint(values->DestinationCompressionFormat, 0, 4) |
      util_bitpack_uint(values->DestinationClearValueEnable, 5, 5);
   const uint64_t v14_address =
      __gen_address(data, &dw[14], values->DestinationClearAddress, v14, 6, 47);
   dw[14] = v14_address;
   dw[15] = (v14_address >> 32) | (v14 >> 32);

   dw[16] =
      util_bitpack_uint(values->DestinationSurfaceHeight, 0, 13) |
      util_bitpack_uint(values->DestinationSurfaceWidth, 14, 27) |
      util_bitpack_uint(values->DestinationSurfaceType, 29, 31);

   dw[17] =
      util_bitpack_uint(values->DestinationLOD, 0, 3) |
      util_bitpack_uint(values->DestinationSurfaceQPitch, 4, 18) |
      util_bitpack_uint(values->DestinationSurfaceDepth, 21, 31);

   dw[18] =
      util_bitpack_uint(values->DestinationHorizontalAlign, 0, 1) |
      util_bitpack_uint(values->DestinationVerticalAlign, 3, 4) |
      util_bitpack_uint(values->DestinationMipTailStartLOD, 8, 11) |
      util_bitpack_uint(values->DestinationArrayIndex, 21, 31);

   dw[19] =
      util_bitpack_uint(values->SourceSurfaceHeight, 0, 13) |
      util_bitpack_uint(values->SourceSurfaceWidth, 14, 27) |
      util_bitpack_uint(values->SourceSurfaceType, 29, 31);

   dw[20] =
      util_bitpack_uint(values->SourceLOD, 0, 3) |
      util_bitpack_uint(values->SourceSurfaceQPitch, 4, 18) |
      util_bitpack_uint(values->SourceSurfaceDepth, 21, 31);

   dw[21] =
      util_bitpack_uint(values->SourceHorizontalAlign, 0, 1) |
      util_bitpack_uint(values->SourceVerticalAlign, 3, 4) |
      util_bitpack_uint(values->SourceMipTailStartLOD, 8, 11) |
      util_bitpack_uint(values->SourceArrayIndex, 21, 31);
}

#define GFX30_XY_FAST_COLOR_BLT_length        16
#define GFX30_XY_FAST_COLOR_BLT_length_bias      2
#define GFX30_XY_FAST_COLOR_BLT_header          \
   .DWordLength                         =     14,  \
   .InstructionTargetOpcode             =     68,  \
   .Client                              =      2

struct GFX30_XY_FAST_COLOR_BLT {
   uint32_t                             DWordLength;
   uint32_t                             NumberofMultisamples;
#define MULTISAMPLECOUNT_1                       0
#define MULTISAMPLECOUNT_2                       1
#define MULTISAMPLECOUNT_4                       2
#define MULTISAMPLECOUNT_8                       3
#define MULTISAMPLECOUNT_16                      4
   uint32_t                             SpecialModeofOperation;
#define NONE                                     0
#define USE_AUX_SPECIAL_OPERATIONS_MODE          1
   uint32_t                             ColorDepth;
#define XY_BPP_8_BIT                             0
#define XY_BPP_16_BIT                            1
#define XY_BPP_32_BIT                            2
#define XY_BPP_64_BIT                            3
#define XY_BPP_96_BIT                            4
#define XY_BPP_128_BIT                           5
   uint32_t                             InstructionTargetOpcode;
#define XY_FAST_COLOR_BLT                        68
   uint32_t                             Client;
#define _2DProcessor                             2
   uint32_t                             DestinationPitch;
   bool                                 DestinationEncryptEn;
   uint32_t                             DestinationMOCSindex;
   uint32_t                             DestinationTiling;
#define XY_TILE_LINEAR                           0
#define XY_TILE_X                                1
#define XY_TILE_4                                2
#define XY_TILE_64                               3
   int32_t                              DestinationX1;
   int32_t                              DestinationY1;
   int32_t                              DestinationX2;
   int32_t                              DestinationY2;
   __gen_address_type                   DestinationBaseAddress;
   uint32_t                             DestinationXOffset;
   uint32_t                             DestinationYOffset;
   uint32_t                             DestinationTargetMemory;
#define LOCAL_MEM                                0
#define SYSTEM_MEM                               1
   uint32_t                             FillColor[4];
   uint32_t                             DestinationCompressionFormat;
   uint32_t                             SurfaceFormat;
   uint32_t                             AUX_SPECIAL_OPERATIONS_MODE;
#define FAST_CLEAR_HW_FORMAT_CONVERSION          0
#define FAST_CLEAR_BYPASS_HW_FORMAT_CONVERSION   1
#define FORCE_UNCOMPRESS                         2
   uint32_t                             DestinationSurfaceHeight;
   uint32_t                             DestinationSurfaceWidth;
   uint32_t                             DestinationSurfaceType;
#define XY_SURFTYPE_1D                           0
#define XY_SURFTYPE_2D                           1
#define XY_SURFTYPE_3D                           2
#define XY_SURFTYPE_CUBE                         3
   uint32_t                             DestinationLOD;
   uint32_t                             DestinationSurfaceQPitch;
   uint32_t                             DestinationSurfaceDepth;
   uint32_t                             DestinationHorizontalAlign;
#define HALIGN_16                                0
#define HALIGN_32                                1
#define HALIGN_64                                2
#define HALIGN_128                               3
   uint32_t                             DestinationVerticalAlign;
#define VALIGN_4                                 1
#define VALIGN_8                                 2
#define VALIGN_16                                3
   uint32_t                             DestinationMipTailStartLOD;
   bool                                 DestinationDepthStencilResource;
   uint32_t                             DestinationArrayIndex;
};

static inline __attribute__((always_inline)) void
GFX30_XY_FAST_COLOR_BLT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_XY_FAST_COLOR_BLT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DWordLength, 0, 7) |
      util_bitpack_uint(values->NumberofMultisamples, 9, 11) |
      util_bitpack_uint(values->SpecialModeofOperation, 12, 13) |
      util_bitpack_uint(values->ColorDepth, 19, 21) |
      util_bitpack_uint(values->InstructionTargetOpcode, 22, 28) |
      util_bitpack_uint(values->Client, 29, 31);

   dw[1] =
      util_bitpack_uint(values->DestinationPitch, 0, 17) |
      util_bitpack_uint(values->DestinationEncryptEn, 21, 21) |
      util_bitpack_uint(values->DestinationMOCSindex, 24, 27) |
      util_bitpack_uint(values->DestinationTiling, 30, 31);

   dw[2] =
      util_bitpack_sint(values->DestinationX1, 0, 15) |
      util_bitpack_sint(values->DestinationY1, 16, 31);

   dw[3] =
      util_bitpack_sint(values->DestinationX2, 0, 15) |
      util_bitpack_sint(values->DestinationY2, 16, 31);

   const uint64_t v4_address =
      __gen_address(data, &dw[4], values->DestinationBaseAddress, 0, 0, 63);
   dw[4] = v4_address;
   dw[5] = v4_address >> 32;

   dw[6] =
      util_bitpack_uint(values->DestinationXOffset, 0, 13) |
      util_bitpack_uint(values->DestinationYOffset, 16, 29) |
      util_bitpack_uint(values->DestinationTargetMemory, 31, 31);

   dw[7] =
      util_bitpack_uint(values->FillColor[0], 0, 31);

   dw[8] =
      util_bitpack_uint(values->FillColor[1], 0, 31);

   dw[9] =
      util_bitpack_uint(values->FillColor[2], 0, 31);

   dw[10] =
      util_bitpack_uint(values->FillColor[3], 0, 31);

   dw[11] =
      util_bitpack_uint(values->DestinationCompressionFormat, 0, 3);

   dw[12] =
      util_bitpack_uint(values->SurfaceFormat, 0, 8) |
      util_bitpack_uint(values->AUX_SPECIAL_OPERATIONS_MODE, 29, 31);

   dw[13] =
      util_bitpack_uint(values->DestinationSurfaceHeight, 0, 13) |
      util_bitpack_uint(values->DestinationSurfaceWidth, 14, 27) |
      util_bitpack_uint(values->DestinationSurfaceType, 29, 31);

   dw[14] =
      util_bitpack_uint(values->DestinationLOD, 0, 3) |
      util_bitpack_uint(values->DestinationSurfaceQPitch, 4, 18) |
      util_bitpack_uint(values->DestinationSurfaceDepth, 21, 31);

   dw[15] =
      util_bitpack_uint(values->DestinationHorizontalAlign, 0, 1) |
      util_bitpack_uint(values->DestinationVerticalAlign, 3, 4) |
      util_bitpack_uint(values->DestinationMipTailStartLOD, 8, 11) |
      util_bitpack_uint(values->DestinationDepthStencilResource, 18, 18) |
      util_bitpack_uint(values->DestinationArrayIndex, 21, 31);
}

#define GFX30_BCS_AUX_TABLE_BASE_ADDR_num 0x4240
#define GFX30_BCS_AUX_TABLE_BASE_ADDR_length      2
struct GFX30_BCS_AUX_TABLE_BASE_ADDR {
   uint64_t                             Address;
};

static inline __attribute__((always_inline)) void
GFX30_BCS_AUX_TABLE_BASE_ADDR_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_BCS_AUX_TABLE_BASE_ADDR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->Address, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_BCS_CCS_AUX_INV_num         0x4248
#define GFX30_BCS_CCS_AUX_INV_length           1
struct GFX30_BCS_CCS_AUX_INV {
   bool                                 AuxInv;
};

static inline __attribute__((always_inline)) void
GFX30_BCS_CCS_AUX_INV_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_BCS_CCS_AUX_INV * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AuxInv, 0, 0);
}

#define GFX30_BCS_INSTDONE_num            0x2206c
#define GFX30_BCS_INSTDONE_length              1
struct GFX30_BCS_INSTDONE {
   bool                                 RingEnable;
   bool                                 BlitterIDLE;
   bool                                 GABIDLE;
   bool                                 BCSDone;
};

static inline __attribute__((always_inline)) void
GFX30_BCS_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_BCS_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RingEnable, 0, 0) |
      util_bitpack_uint(values->BlitterIDLE, 1, 1) |
      util_bitpack_uint(values->GABIDLE, 2, 2) |
      util_bitpack_uint(values->BCSDone, 3, 3);
}

#define GFX30_BLT_TRTT_CR_num             0x4480
#define GFX30_BLT_TRTT_CR_length               1
struct GFX30_BLT_TRTT_CR {
   bool                                 TRTTEnable;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_CR_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_BLT_TRTT_CR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRTTEnable, 0, 0);
}

#define GFX30_BLT_TRTT_INVAL_num          0x4494
#define GFX30_BLT_TRTT_INVAL_length            1
struct GFX30_BLT_TRTT_INVAL {
   uint32_t                             InvalidTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_INVAL_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_BLT_TRTT_INVAL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->InvalidTileDetectionValue, 0, 31);
}

#define GFX30_BLT_TRTT_L3_BASE_HIGH_num   0x448c
#define GFX30_BLT_TRTT_L3_BASE_HIGH_length      1
struct GFX30_BLT_TRTT_L3_BASE_HIGH {
   uint32_t                             TRVAL3PointerUpperAddress;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_L3_BASE_HIGH_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_BLT_TRTT_L3_BASE_HIGH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerUpperAddress, 0, 15);
}

#define GFX30_BLT_TRTT_L3_BASE_LOW_num    0x4488
#define GFX30_BLT_TRTT_L3_BASE_LOW_length      1
struct GFX30_BLT_TRTT_L3_BASE_LOW {
   uint32_t                             TRVAL3PointerLowerAddress;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_L3_BASE_LOW_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_BLT_TRTT_L3_BASE_LOW * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerLowerAddress, 12, 31);
}

#define GFX30_BLT_TRTT_NULL_num           0x4490
#define GFX30_BLT_TRTT_NULL_length             1
struct GFX30_BLT_TRTT_NULL {
   uint32_t                             NullTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_NULL_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_BLT_TRTT_NULL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->NullTileDetectionValue, 0, 31);
}

#define GFX30_BLT_TRTT_VA_RANGE_num       0x4484
#define GFX30_BLT_TRTT_VA_RANGE_length         1
struct GFX30_BLT_TRTT_VA_RANGE {
   uint32_t                             TRVABase;
};

static inline __attribute__((always_inline)) void
GFX30_BLT_TRTT_VA_RANGE_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_BLT_TRTT_VA_RANGE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVABase, 0, 12);
}

#define GFX30_CACHE_MODE_0_num            0x7000
#define GFX30_CACHE_MODE_0_length              1
struct GFX30_CACHE_MODE_0 {
   bool                                 HierarchicalZRAWStallOptimizationDisable;
   bool                                 HierarchicalZDisable;
   bool                                 LateZAllocDisable;
   bool                                 MsaaFastClearEnabled;
   bool                                 DepthRelatedCachePipelinedFlushDisable;
   bool                                 SamplerL2TLBPrefetchEnable;
   uint32_t                             RCZPMANotPromotedAllocationstalloptimizationDisableduetochangeindepthparameters;
   bool                                 DisableRepackingforCompression;
   bool                                 HierarchicalZRAWStallOptimizationDisableMask;
   bool                                 HierarchicalZDisableMask;
   bool                                 LateZAllocDisableMask;
   bool                                 MsaaFastClearEnabledMask;
   bool                                 DepthRelatedCachePipelinedFlushDisableMask;
   bool                                 SamplerL2TLBPrefetchEnableMask;
   bool                                 RCZPMANotPromotedAllocationstalloptimizationDisableduetochangeindepthparametersMask;
   bool                                 DisableRepackingforCompressionMask;
};

static inline __attribute__((always_inline)) void
GFX30_CACHE_MODE_0_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_CACHE_MODE_0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->HierarchicalZRAWStallOptimizationDisable, 2, 2) |
      util_bitpack_uint(values->HierarchicalZDisable, 3, 3) |
      util_bitpack_uint(values->LateZAllocDisable, 4, 4) |
      util_bitpack_uint(values->MsaaFastClearEnabled, 5, 5) |
      util_bitpack_uint(values->DepthRelatedCachePipelinedFlushDisable, 8, 8) |
      util_bitpack_uint(values->SamplerL2TLBPrefetchEnable, 9, 9) |
      util_bitpack_uint(values->RCZPMANotPromotedAllocationstalloptimizationDisableduetochangeindepthparameters, 10, 10) |
      util_bitpack_uint(values->DisableRepackingforCompression, 15, 15) |
      util_bitpack_uint(values->HierarchicalZRAWStallOptimizationDisableMask, 18, 18) |
      util_bitpack_uint(values->HierarchicalZDisableMask, 19, 19) |
      util_bitpack_uint(values->LateZAllocDisableMask, 20, 20) |
      util_bitpack_uint(values->MsaaFastClearEnabledMask, 21, 21) |
      util_bitpack_uint(values->DepthRelatedCachePipelinedFlushDisableMask, 24, 24) |
      util_bitpack_uint(values->SamplerL2TLBPrefetchEnableMask, 25, 25) |
      util_bitpack_uint(values->RCZPMANotPromotedAllocationstalloptimizationDisableduetochangeindepthparametersMask, 26, 26) |
      util_bitpack_uint(values->DisableRepackingforCompressionMask, 31, 31);
}

#define GFX30_CACHE_MODE_1_num            0x7004
#define GFX30_CACHE_MODE_1_length              1
struct GFX30_CACHE_MODE_1 {
   bool                                 PartialResolveDisableInVC;
   bool                                 RCZPMAPromoted2NotPromotedAllocationstalloptimizationDisable;
   bool                                 MCSCacheDisable;
   bool                                 MSCRAWHazardAvoidanceBit;
   uint32_t                             NPEarlyZFailsDisable;
   bool                                 BlendOptimizationFixDisable;
   bool                                 ColorCompressionDisable;
   bool                                 PartialResolveDisableInVCMask;
   bool                                 RCZPMAPromoted2NotPromotedAllocationstalloptimizationDisableMask;
   bool                                 MCSCacheDisableMask;
   bool                                 MSCRAWHazardAvoidanceBitMask;
   bool                                 NPEarlyZFailsDisableMask;
   bool                                 BlendOptimizationFixDisableMask;
   bool                                 ColorCompressionDisableMask;
};

static inline __attribute__((always_inline)) void
GFX30_CACHE_MODE_1_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_CACHE_MODE_1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PartialResolveDisableInVC, 1, 1) |
      util_bitpack_uint(values->RCZPMAPromoted2NotPromotedAllocationstalloptimizationDisable, 3, 3) |
      util_bitpack_uint(values->MCSCacheDisable, 5, 5) |
      util_bitpack_uint(values->MSCRAWHazardAvoidanceBit, 9, 9) |
      util_bitpack_uint(values->NPEarlyZFailsDisable, 13, 13) |
      util_bitpack_uint(values->BlendOptimizationFixDisable, 14, 14) |
      util_bitpack_uint(values->ColorCompressionDisable, 15, 15) |
      util_bitpack_uint(values->PartialResolveDisableInVCMask, 17, 17) |
      util_bitpack_uint(values->RCZPMAPromoted2NotPromotedAllocationstalloptimizationDisableMask, 19, 19) |
      util_bitpack_uint(values->MCSCacheDisableMask, 21, 21) |
      util_bitpack_uint(values->MSCRAWHazardAvoidanceBitMask, 25, 25) |
      util_bitpack_uint(values->NPEarlyZFailsDisableMask, 29, 29) |
      util_bitpack_uint(values->BlendOptimizationFixDisableMask, 30, 30) |
      util_bitpack_uint(values->ColorCompressionDisableMask, 31, 31);
}

#define GFX30_CCS_INSTDONE_num            0x1206c
#define GFX30_CCS_INSTDONE_length              1
struct GFX30_CCS_INSTDONE {
   bool                                 RingEnable;
   bool                                 VFEDone;
   bool                                 TSGDone;
   bool                                 CSDone;
};

static inline __attribute__((always_inline)) void
GFX30_CCS_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_CCS_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RingEnable, 0, 0) |
      util_bitpack_uint(values->VFEDone, 16, 16) |
      util_bitpack_uint(values->TSGDone, 17, 17) |
      util_bitpack_uint(values->CSDone, 21, 21);
}

#define GFX30_CHICKEN_RASTER_1_num        0x6204
#define GFX30_CHICKEN_RASTER_1_length          1
struct GFX30_CHICKEN_RASTER_1 {
   bool                                 AALineQualityFix;
   bool                                 AALineQualityFixMask;
};

static inline __attribute__((always_inline)) void
GFX30_CHICKEN_RASTER_1_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_CHICKEN_RASTER_1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AALineQualityFix, 5, 5) |
      util_bitpack_uint(values->AALineQualityFixMask, 21, 21);
}

#define GFX30_CHICKEN_RASTER_2_num        0x6208
#define GFX30_CHICKEN_RASTER_2_length          1
struct GFX30_CHICKEN_RASTER_2 {
   bool                                 DisableAnyMCTRresponsefix;
   bool                                 DisableAnyMCTRresponsefixMask;
};

static inline __attribute__((always_inline)) void
GFX30_CHICKEN_RASTER_2_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_CHICKEN_RASTER_2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DisableAnyMCTRresponsefix, 5, 5) |
      util_bitpack_uint(values->DisableAnyMCTRresponsefixMask, 21, 21);
}

#define GFX30_CL_INVOCATION_COUNT_num     0x2338
#define GFX30_CL_INVOCATION_COUNT_length       2
struct GFX30_CL_INVOCATION_COUNT {
   uint64_t                             CLInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_CL_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_CL_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->CLInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_CL_PRIMITIVES_COUNT_num     0x2340
#define GFX30_CL_PRIMITIVES_COUNT_length       2
struct GFX30_CL_PRIMITIVES_COUNT {
   uint64_t                             CLPrimitivesCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_CL_PRIMITIVES_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_CL_PRIMITIVES_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->CLPrimitivesCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_COMMON_SLICE_CHICKEN1_num   0x7010
#define GFX30_COMMON_SLICE_CHICKEN1_length      1
struct GFX30_COMMON_SLICE_CHICKEN1 {
   bool                                 HIZPlaneOptimizationdisablebit;
   bool                                 HIZPlaneOptimizationdisablebitMask;
};

static inline __attribute__((always_inline)) void
GFX30_COMMON_SLICE_CHICKEN1_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_COMMON_SLICE_CHICKEN1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->HIZPlaneOptimizationdisablebit, 9, 9) |
      util_bitpack_uint(values->HIZPlaneOptimizationdisablebitMask, 25, 25);
}

#define GFX30_COMMON_SLICE_CHICKEN3_num   0x7304
#define GFX30_COMMON_SLICE_CHICKEN3_length      1
struct GFX30_COMMON_SLICE_CHICKEN3 {
   uint32_t                             PSThreadPanicDispatch;
   bool                                 StateCachePerfFixDisabled;
   uint32_t                             PSThreadPanicDispatchMask;
   bool                                 StateCachePerfFixDisabledMask;
};

static inline __attribute__((always_inline)) void
GFX30_COMMON_SLICE_CHICKEN3_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_COMMON_SLICE_CHICKEN3 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PSThreadPanicDispatch, 6, 7) |
      util_bitpack_uint(values->StateCachePerfFixDisabled, 13, 13) |
      util_bitpack_uint(values->PSThreadPanicDispatchMask, 22, 23) |
      util_bitpack_uint(values->StateCachePerfFixDisabledMask, 29, 29);
}

#define GFX30_COMMON_SLICE_CHICKEN4_num   0x7300
#define GFX30_COMMON_SLICE_CHICKEN4_length      1
struct GFX30_COMMON_SLICE_CHICKEN4 {
   bool                                 EnableHardwareFilteringinWM;
   bool                                 EnableHardwareFilteringinWMMask;
};

static inline __attribute__((always_inline)) void
GFX30_COMMON_SLICE_CHICKEN4_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_COMMON_SLICE_CHICKEN4 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->EnableHardwareFilteringinWM, 5, 5) |
      util_bitpack_uint(values->EnableHardwareFilteringinWMMask, 21, 21);
}

#define GFX30_COMPCS0_AUX_TABLE_BASE_ADDR_num 0x42c0
#define GFX30_COMPCS0_AUX_TABLE_BASE_ADDR_length      2
struct GFX30_COMPCS0_AUX_TABLE_BASE_ADDR {
   uint64_t                             Address;
};

static inline __attribute__((always_inline)) void
GFX30_COMPCS0_AUX_TABLE_BASE_ADDR_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_COMPCS0_AUX_TABLE_BASE_ADDR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->Address, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_COMPCS0_CCS_AUX_INV_num     0x42c8
#define GFX30_COMPCS0_CCS_AUX_INV_length       1
struct GFX30_COMPCS0_CCS_AUX_INV {
   bool                                 AuxInv;
};

static inline __attribute__((always_inline)) void
GFX30_COMPCS0_CCS_AUX_INV_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_COMPCS0_CCS_AUX_INV * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AuxInv, 0, 0);
}

#define GFX30_COMP_CTX0_TRTT_CR_num       0x4580
#define GFX30_COMP_CTX0_TRTT_CR_length         1
struct GFX30_COMP_CTX0_TRTT_CR {
   bool                                 TRTTEnable;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_CR_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_CR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRTTEnable, 0, 0);
}

#define GFX30_COMP_CTX0_TRTT_INVAL_num    0x4594
#define GFX30_COMP_CTX0_TRTT_INVAL_length      1
struct GFX30_COMP_CTX0_TRTT_INVAL {
   uint32_t                             InvalidTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_INVAL_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_INVAL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->InvalidTileDetectionValue, 0, 31);
}

#define GFX30_COMP_CTX0_TRTT_L3_BASE_HIGH_num 0x458c
#define GFX30_COMP_CTX0_TRTT_L3_BASE_HIGH_length      1
struct GFX30_COMP_CTX0_TRTT_L3_BASE_HIGH {
   uint32_t                             TRVAL3PointerUpperAddress;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_L3_BASE_HIGH_pack(__attribute__((unused)) __gen_user_data *data,
                                       __attribute__((unused)) void * restrict dst,
                                       __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_L3_BASE_HIGH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerUpperAddress, 0, 15);
}

#define GFX30_COMP_CTX0_TRTT_L3_BASE_LOW_num 0x4588
#define GFX30_COMP_CTX0_TRTT_L3_BASE_LOW_length      1
struct GFX30_COMP_CTX0_TRTT_L3_BASE_LOW {
   uint32_t                             TRVAL3PointerLowerAddress;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_L3_BASE_LOW_pack(__attribute__((unused)) __gen_user_data *data,
                                      __attribute__((unused)) void * restrict dst,
                                      __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_L3_BASE_LOW * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerLowerAddress, 12, 31);
}

#define GFX30_COMP_CTX0_TRTT_NULL_num     0x4590
#define GFX30_COMP_CTX0_TRTT_NULL_length       1
struct GFX30_COMP_CTX0_TRTT_NULL {
   uint32_t                             NullTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_NULL_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_NULL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->NullTileDetectionValue, 0, 31);
}

#define GFX30_COMP_CTX0_TRTT_VA_RANGE_num 0x4584
#define GFX30_COMP_CTX0_TRTT_VA_RANGE_length      1
struct GFX30_COMP_CTX0_TRTT_VA_RANGE {
   uint32_t                             TRVABase;
};

static inline __attribute__((always_inline)) void
GFX30_COMP_CTX0_TRTT_VA_RANGE_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_COMP_CTX0_TRTT_VA_RANGE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVABase, 0, 12);
}

#define GFX30_CS_CHICKEN1_num             0x2580
#define GFX30_CS_CHICKEN1_length               1
struct GFX30_CS_CHICKEN1 {
   uint32_t                             MediaAndGPGPUPreemptionControl;
#define MidthreadPreemption                      0
#define ThreadGroupPreemption                    1
#define CommandLevelPreemption                   2
   bool                                 DisablePreemptionandHighPriorityPausingdueto3DPRIMITIVECommand;
   uint32_t                             MediaAndGPGPUPreemptionControlMask;
   bool                                 DisablePreemptionandHighPriorityPausingdueto3DPRIMITIVECommandMask;
};

static inline __attribute__((always_inline)) void
GFX30_CS_CHICKEN1_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_CS_CHICKEN1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->MediaAndGPGPUPreemptionControl, 1, 2) |
      util_bitpack_uint(values->DisablePreemptionandHighPriorityPausingdueto3DPRIMITIVECommand, 10, 10) |
      util_bitpack_uint(values->MediaAndGPGPUPreemptionControlMask, 17, 18) |
      util_bitpack_uint(values->DisablePreemptionandHighPriorityPausingdueto3DPRIMITIVECommandMask, 26, 26);
}

#define GFX30_CS_DEBUG_MODE2_num          0x20d8
#define GFX30_CS_DEBUG_MODE2_length            1
struct GFX30_CS_DEBUG_MODE2 {
   bool                                 _3DRenderingInstructionDisable;
   bool                                 MediaInstructionDisable;
   bool                                 CONSTANT_BUFFERAddressOffsetDisable;
   bool                                 DisableGatheratSetShader;
   bool                                 _3DRenderingInstructionDisableMask;
   bool                                 MediaInstructionDisableMask;
   bool                                 CONSTANT_BUFFERAddressOffsetDisableMask;
   bool                                 DisableGatheratSetShaderMask;
};

static inline __attribute__((always_inline)) void
GFX30_CS_DEBUG_MODE2_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_CS_DEBUG_MODE2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->_3DRenderingInstructionDisable, 0, 0) |
      util_bitpack_uint(values->MediaInstructionDisable, 1, 1) |
      util_bitpack_uint(values->CONSTANT_BUFFERAddressOffsetDisable, 4, 4) |
      util_bitpack_uint(values->DisableGatheratSetShader, 7, 7) |
      util_bitpack_uint(values->_3DRenderingInstructionDisableMask, 16, 16) |
      util_bitpack_uint(values->MediaInstructionDisableMask, 17, 17) |
      util_bitpack_uint(values->CONSTANT_BUFFERAddressOffsetDisableMask, 20, 20) |
      util_bitpack_uint(values->DisableGatheratSetShaderMask, 23, 23);
}

#define GFX30_CS_INVOCATION_COUNT_num     0x2290
#define GFX30_CS_INVOCATION_COUNT_length       2
struct GFX30_CS_INVOCATION_COUNT {
   uint64_t                             CSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_CS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_CS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->CSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_DS_INVOCATION_COUNT_num     0x2308
#define GFX30_DS_INVOCATION_COUNT_length       2
struct GFX30_DS_INVOCATION_COUNT {
   uint64_t                             DSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_DS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_DS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->DSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_FF_MODE_num                 0x6210
#define GFX30_FF_MODE_length                   1
struct GFX30_FF_MODE {
   bool                                 TEPatchcontrolbugfix;
   bool                                 MeshShaderAutostripDisable;
   bool                                 MeshShaderPartialAutostripDisable;
   bool                                 TEAutostripDisable;
};

static inline __attribute__((always_inline)) void
GFX30_FF_MODE_pack(__attribute__((unused)) __gen_user_data *data,
                   __attribute__((unused)) void * restrict dst,
                   __attribute__((unused)) const struct GFX30_FF_MODE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TEPatchcontrolbugfix, 4, 4) |
      util_bitpack_uint(values->MeshShaderAutostripDisable, 15, 15) |
      util_bitpack_uint(values->MeshShaderPartialAutostripDisable, 16, 16) |
      util_bitpack_uint(values->TEAutostripDisable, 31, 31);
}

#define GFX30_FF_MODE2_num                0x6604
#define GFX30_FF_MODE2_length                  1
struct GFX30_FF_MODE2 {
   uint32_t                             VSTimerValue;
   uint32_t                             HSTimerValue;
   uint32_t                             TDSTimerValue;
   uint32_t                             GSTimerValue;
};

static inline __attribute__((always_inline)) void
GFX30_FF_MODE2_pack(__attribute__((unused)) __gen_user_data *data,
                    __attribute__((unused)) void * restrict dst,
                    __attribute__((unused)) const struct GFX30_FF_MODE2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->VSTimerValue, 0, 7) |
      util_bitpack_uint(values->HSTimerValue, 8, 15) |
      util_bitpack_uint(values->TDSTimerValue, 16, 23) |
      util_bitpack_uint(values->GSTimerValue, 24, 31);
}

#define GFX30_GAM_DONE_num                0xcf68
#define GFX30_GAM_DONE_length                  1
struct GFX30_GAM_DONE {
   bool                                 GFXTLB0Done;
   bool                                 GFXTLB1Done;
   bool                                 COMPTLB0Done;
   bool                                 COMPTLB1Done;
   bool                                 GUCTLBDone;
   bool                                 KCRTLBDone;
   bool                                 OACTLBDone;
   bool                                 VDBOX0TLBDone;
   bool                                 VDBOX2TLBDone;
   bool                                 VEBOX0TLBDone;
   bool                                 CMINGRESSDone;
   bool                                 DataINGRESSDone;
   bool                                 REQSTRMDone;
   bool                                 STLBDone;
   bool                                 BLTTLBDone;
   bool                                 TLBHitARBCOMPDone;
   bool                                 TLBHitARBMISCDone;
   bool                                 TLBHitARBSTGDone;
   bool                                 TLBHitGFXSTGDone;
   bool                                 TLBMissARBGFXDone;
   bool                                 TLBMissARBCOMPDone;
   bool                                 TLBMissARBMISCDone;
   bool                                 TLBMissARBSTGDone;
   bool                                 TRTLBDone;
   bool                                 WKRSDone;
   bool                                 CCSDone;
   bool                                 CTRLCOMMDone;
   bool                                 FLWCTRLDone;
   bool                                 DRTNDone;
   bool                                 VEBOX1TBLDone;
   bool                                 XBLDone;
};

static inline __attribute__((always_inline)) void
GFX30_GAM_DONE_pack(__attribute__((unused)) __gen_user_data *data,
                    __attribute__((unused)) void * restrict dst,
                    __attribute__((unused)) const struct GFX30_GAM_DONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->GFXTLB0Done, 0, 0) |
      util_bitpack_uint(values->GFXTLB1Done, 1, 1) |
      util_bitpack_uint(values->COMPTLB0Done, 2, 2) |
      util_bitpack_uint(values->COMPTLB1Done, 3, 3) |
      util_bitpack_uint(values->GUCTLBDone, 4, 4) |
      util_bitpack_uint(values->KCRTLBDone, 5, 5) |
      util_bitpack_uint(values->OACTLBDone, 6, 6) |
      util_bitpack_uint(values->VDBOX0TLBDone, 7, 7) |
      util_bitpack_uint(values->VDBOX2TLBDone, 8, 8) |
      util_bitpack_uint(values->VEBOX0TLBDone, 9, 9) |
      util_bitpack_uint(values->CMINGRESSDone, 10, 10) |
      util_bitpack_uint(values->DataINGRESSDone, 11, 11) |
      util_bitpack_uint(values->REQSTRMDone, 12, 12) |
      util_bitpack_uint(values->STLBDone, 13, 13) |
      util_bitpack_uint(values->BLTTLBDone, 14, 14) |
      util_bitpack_uint(values->TLBHitARBCOMPDone, 15, 15) |
      util_bitpack_uint(values->TLBHitARBMISCDone, 16, 16) |
      util_bitpack_uint(values->TLBHitARBSTGDone, 17, 17) |
      util_bitpack_uint(values->TLBHitGFXSTGDone, 18, 18) |
      util_bitpack_uint(values->TLBMissARBGFXDone, 19, 19) |
      util_bitpack_uint(values->TLBMissARBCOMPDone, 20, 20) |
      util_bitpack_uint(values->TLBMissARBMISCDone, 21, 21) |
      util_bitpack_uint(values->TLBMissARBSTGDone, 22, 22) |
      util_bitpack_uint(values->TRTLBDone, 23, 23) |
      util_bitpack_uint(values->WKRSDone, 24, 24) |
      util_bitpack_uint(values->CCSDone, 25, 25) |
      util_bitpack_uint(values->CTRLCOMMDone, 26, 26) |
      util_bitpack_uint(values->FLWCTRLDone, 27, 27) |
      util_bitpack_uint(values->DRTNDone, 28, 28) |
      util_bitpack_uint(values->VEBOX1TBLDone, 29, 29) |
      util_bitpack_uint(values->XBLDone, 30, 30);
}

#define GFX30_GFX_AUX_TABLE_BASE_ADDR_num 0x4200
#define GFX30_GFX_AUX_TABLE_BASE_ADDR_length      2
struct GFX30_GFX_AUX_TABLE_BASE_ADDR {
   uint64_t                             Address;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_AUX_TABLE_BASE_ADDR_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_GFX_AUX_TABLE_BASE_ADDR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->Address, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_GFX_CCS_AUX_INV_num         0x4208
#define GFX30_GFX_CCS_AUX_INV_length           1
struct GFX30_GFX_CCS_AUX_INV {
   bool                                 AuxInv;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_CCS_AUX_INV_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_GFX_CCS_AUX_INV * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AuxInv, 0, 0);
}

#define GFX30_GFX_TRTT_CR_num             0x4400
#define GFX30_GFX_TRTT_CR_length               1
struct GFX30_GFX_TRTT_CR {
   bool                                 TRTTEnable;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_CR_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_GFX_TRTT_CR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRTTEnable, 0, 0);
}

#define GFX30_GFX_TRTT_INVAL_num          0x4414
#define GFX30_GFX_TRTT_INVAL_length            1
struct GFX30_GFX_TRTT_INVAL {
   uint32_t                             InvalidTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_INVAL_pack(__attribute__((unused)) __gen_user_data *data,
                          __attribute__((unused)) void * restrict dst,
                          __attribute__((unused)) const struct GFX30_GFX_TRTT_INVAL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->InvalidTileDetectionValue, 0, 31);
}

#define GFX30_GFX_TRTT_L3_BASE_HIGH_num   0x440c
#define GFX30_GFX_TRTT_L3_BASE_HIGH_length      1
struct GFX30_GFX_TRTT_L3_BASE_HIGH {
   uint32_t                             TRVAL3PointerUpperAddress;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_L3_BASE_HIGH_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_GFX_TRTT_L3_BASE_HIGH * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerUpperAddress, 0, 15);
}

#define GFX30_GFX_TRTT_L3_BASE_LOW_num    0x4408
#define GFX30_GFX_TRTT_L3_BASE_LOW_length      1
struct GFX30_GFX_TRTT_L3_BASE_LOW {
   uint32_t                             TRVAL3PointerLowerAddress;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_L3_BASE_LOW_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_GFX_TRTT_L3_BASE_LOW * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVAL3PointerLowerAddress, 12, 31);
}

#define GFX30_GFX_TRTT_NULL_num           0x4410
#define GFX30_GFX_TRTT_NULL_length             1
struct GFX30_GFX_TRTT_NULL {
   uint32_t                             NullTileDetectionValue;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_NULL_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_GFX_TRTT_NULL * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->NullTileDetectionValue, 0, 31);
}

#define GFX30_GFX_TRTT_VA_RANGE_num       0x4404
#define GFX30_GFX_TRTT_VA_RANGE_length         1
struct GFX30_GFX_TRTT_VA_RANGE {
   uint32_t                             TRVABase;
};

static inline __attribute__((always_inline)) void
GFX30_GFX_TRTT_VA_RANGE_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_GFX_TRTT_VA_RANGE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->TRVABase, 0, 12);
}

#define GFX30_GPGPU_DISPATCHDIMX_num      0x2500
#define GFX30_GPGPU_DISPATCHDIMX_length        1
struct GFX30_GPGPU_DISPATCHDIMX {
   uint32_t                             DispatchDimensionX;
};

static inline __attribute__((always_inline)) void
GFX30_GPGPU_DISPATCHDIMX_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_GPGPU_DISPATCHDIMX * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DispatchDimensionX, 0, 31);
}

#define GFX30_GPGPU_DISPATCHDIMY_num      0x2504
#define GFX30_GPGPU_DISPATCHDIMY_length        1
struct GFX30_GPGPU_DISPATCHDIMY {
   uint32_t                             DispatchDimensionY;
};

static inline __attribute__((always_inline)) void
GFX30_GPGPU_DISPATCHDIMY_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_GPGPU_DISPATCHDIMY * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DispatchDimensionY, 0, 31);
}

#define GFX30_GPGPU_DISPATCHDIMZ_num      0x2508
#define GFX30_GPGPU_DISPATCHDIMZ_length        1
struct GFX30_GPGPU_DISPATCHDIMZ {
   uint32_t                             DispatchDimensionZ;
};

static inline __attribute__((always_inline)) void
GFX30_GPGPU_DISPATCHDIMZ_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_GPGPU_DISPATCHDIMZ * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->DispatchDimensionZ, 0, 31);
}

#define GFX30_GS_INVOCATION_COUNT_num     0x2328
#define GFX30_GS_INVOCATION_COUNT_length       2
struct GFX30_GS_INVOCATION_COUNT {
   uint64_t                             GSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_GS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_GS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->GSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_GS_PRIMITIVES_COUNT_num     0x2330
#define GFX30_GS_PRIMITIVES_COUNT_length       2
struct GFX30_GS_PRIMITIVES_COUNT {
   uint64_t                             GSPrimitivesCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_GS_PRIMITIVES_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_GS_PRIMITIVES_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->GSPrimitivesCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_HALF_SLICE_CHICKEN7_num     0xe194
#define GFX30_HALF_SLICE_CHICKEN7_length       1
struct GFX30_HALF_SLICE_CHICKEN7 {
   bool                                 EnabledTexelOffsetPrecisionFix;
   bool                                 EnabledTexelOffsetPrecisionFixMask;
};

static inline __attribute__((always_inline)) void
GFX30_HALF_SLICE_CHICKEN7_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_HALF_SLICE_CHICKEN7 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->EnabledTexelOffsetPrecisionFix, 1, 1) |
      util_bitpack_uint(values->EnabledTexelOffsetPrecisionFixMask, 17, 17);
}

#define GFX30_HIZ_CHICKEN_num             0x7018
#define GFX30_HIZ_CHICKEN_length               1
struct GFX30_HIZ_CHICKEN {
   bool                                 HZDepthTestLEGEOptimizationDisable;
   bool                                 HZDepthTestLEGEOptimizationDisableMask;
};

static inline __attribute__((always_inline)) void
GFX30_HIZ_CHICKEN_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_HIZ_CHICKEN * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->HZDepthTestLEGEOptimizationDisable, 13, 13) |
      util_bitpack_uint(values->HZDepthTestLEGEOptimizationDisableMask, 29, 29);
}

#define GFX30_HS_INVOCATION_COUNT_num     0x2300
#define GFX30_HS_INVOCATION_COUNT_length       2
struct GFX30_HS_INVOCATION_COUNT {
   uint64_t                             HSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_HS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_HS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->HSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_IA_PRIMITIVES_COUNT_num     0x2318
#define GFX30_IA_PRIMITIVES_COUNT_length       2
struct GFX30_IA_PRIMITIVES_COUNT {
   uint64_t                             IAPrimitivesCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_IA_PRIMITIVES_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_IA_PRIMITIVES_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->IAPrimitivesCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_IA_VERTICES_COUNT_num       0x2310
#define GFX30_IA_VERTICES_COUNT_length         2
struct GFX30_IA_VERTICES_COUNT {
   uint64_t                             IAVerticesCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_IA_VERTICES_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_IA_VERTICES_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->IAVerticesCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_INSTDONE_1_num              0x206c
#define GFX30_INSTDONE_1_length                1
struct GFX30_INSTDONE_1 {
   bool                                 PRB0RingEnable;
   bool                                 VFGDone;
   bool                                 VSDone;
   bool                                 HSDone;
   bool                                 TEDone;
   bool                                 DSDone;
   bool                                 GSDone;
   bool                                 SOLDone;
   bool                                 CLDone;
   bool                                 SFDone;
   bool                                 TDG1Done;
   bool                                 TDG0Done;
   bool                                 URBMDone;
   bool                                 SVGDone;
   bool                                 GAFSDone;
   bool                                 VFEDone;
   bool                                 TSG0Done;
   bool                                 GAFMDone;
   bool                                 GAMDone;
   bool                                 RSDone;
   bool                                 CSDone;
   bool                                 SDEDone;
   bool                                 RCCFBCCSDone;
   bool                                 TSG1Done;
};

static inline __attribute__((always_inline)) void
GFX30_INSTDONE_1_pack(__attribute__((unused)) __gen_user_data *data,
                      __attribute__((unused)) void * restrict dst,
                      __attribute__((unused)) const struct GFX30_INSTDONE_1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PRB0RingEnable, 0, 0) |
      util_bitpack_uint(values->VFGDone, 1, 1) |
      util_bitpack_uint(values->VSDone, 2, 2) |
      util_bitpack_uint(values->HSDone, 3, 3) |
      util_bitpack_uint(values->TEDone, 4, 4) |
      util_bitpack_uint(values->DSDone, 5, 5) |
      util_bitpack_uint(values->GSDone, 6, 6) |
      util_bitpack_uint(values->SOLDone, 7, 7) |
      util_bitpack_uint(values->CLDone, 8, 8) |
      util_bitpack_uint(values->SFDone, 9, 9) |
      util_bitpack_uint(values->TDG1Done, 11, 11) |
      util_bitpack_uint(values->TDG0Done, 12, 12) |
      util_bitpack_uint(values->URBMDone, 13, 13) |
      util_bitpack_uint(values->SVGDone, 14, 14) |
      util_bitpack_uint(values->GAFSDone, 15, 15) |
      util_bitpack_uint(values->VFEDone, 16, 16) |
      util_bitpack_uint(values->TSG0Done, 17, 17) |
      util_bitpack_uint(values->GAFMDone, 18, 18) |
      util_bitpack_uint(values->GAMDone, 19, 19) |
      util_bitpack_uint(values->RSDone, 20, 20) |
      util_bitpack_uint(values->CSDone, 21, 21) |
      util_bitpack_uint(values->SDEDone, 22, 22) |
      util_bitpack_uint(values->RCCFBCCSDone, 23, 23) |
      util_bitpack_uint(values->TSG1Done, 24, 24);
}

#define GFX30_INSTDONE_GEOM_num           0x666c
#define GFX30_INSTDONE_GEOM_length             1
struct GFX30_INSTDONE_GEOM {
   bool                                 VFLDone;
   bool                                 VSDone;
   bool                                 HSDone;
   bool                                 TEDone;
   bool                                 DSDone;
   bool                                 GSDone;
   bool                                 SOLDone;
   bool                                 CLDone;
   bool                                 SFDone;
   bool                                 TDG1Done;
   bool                                 URBMDone;
   bool                                 SVGDone;
   bool                                 TSG0Done;
   bool                                 SDEDone;
};

static inline __attribute__((always_inline)) void
GFX30_INSTDONE_GEOM_pack(__attribute__((unused)) __gen_user_data *data,
                         __attribute__((unused)) void * restrict dst,
                         __attribute__((unused)) const struct GFX30_INSTDONE_GEOM * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->VFLDone, 1, 1) |
      util_bitpack_uint(values->VSDone, 2, 2) |
      util_bitpack_uint(values->HSDone, 3, 3) |
      util_bitpack_uint(values->TEDone, 4, 4) |
      util_bitpack_uint(values->DSDone, 5, 5) |
      util_bitpack_uint(values->GSDone, 6, 6) |
      util_bitpack_uint(values->SOLDone, 7, 7) |
      util_bitpack_uint(values->CLDone, 8, 8) |
      util_bitpack_uint(values->SFDone, 9, 9) |
      util_bitpack_uint(values->TDG1Done, 11, 11) |
      util_bitpack_uint(values->URBMDone, 13, 13) |
      util_bitpack_uint(values->SVGDone, 14, 14) |
      util_bitpack_uint(values->TSG0Done, 17, 17) |
      util_bitpack_uint(values->SDEDone, 22, 22);
}

#define GFX30_L3SQCREG5_num               0xb158
#define GFX30_L3SQCREG5_length                 1
struct GFX30_L3SQCREG5 {
   uint32_t                             L3CachePartialWriteMergeTimerInitialValue;
   uint32_t                             CompressiblePartialWriteMergeEnable;
   uint32_t                             CoherentPartialWriteMergeEnable;
   uint32_t                             CrossTilePartialWriteMergeEnable;
};

static inline __attribute__((always_inline)) void
GFX30_L3SQCREG5_pack(__attribute__((unused)) __gen_user_data *data,
                     __attribute__((unused)) void * restrict dst,
                     __attribute__((unused)) const struct GFX30_L3SQCREG5 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->L3CachePartialWriteMergeTimerInitialValue, 0, 9) |
      util_bitpack_uint(values->CompressiblePartialWriteMergeEnable, 21, 21) |
      util_bitpack_uint(values->CoherentPartialWriteMergeEnable, 22, 22) |
      util_bitpack_uint(values->CrossTilePartialWriteMergeEnable, 23, 23);
}

#define GFX30_MESH_INVOCATION_COUNT_num   0x26e0
#define GFX30_MESH_INVOCATION_COUNT_length      2
struct GFX30_MESH_INVOCATION_COUNT {
   uint64_t                             MESHInvocationCounter;
};

static inline __attribute__((always_inline)) void
GFX30_MESH_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_MESH_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->MESHInvocationCounter, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_MESH_PRIMITIVE_COUNT_num    0x26d8
#define GFX30_MESH_PRIMITIVE_COUNT_length      2
struct GFX30_MESH_PRIMITIVE_COUNT {
   uint64_t                             MESHPrimitiveCounter;
};

static inline __attribute__((always_inline)) void
GFX30_MESH_PRIMITIVE_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                                __attribute__((unused)) void * restrict dst,
                                __attribute__((unused)) const struct GFX30_MESH_PRIMITIVE_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->MESHPrimitiveCounter, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_PS_INVOCATION_COUNT_num     0x2348
#define GFX30_PS_INVOCATION_COUNT_length       2
struct GFX30_PS_INVOCATION_COUNT {
   uint64_t                             PSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_PS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_PS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->PSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_ROW_INSTDONE_num            0xe164
#define GFX30_ROW_INSTDONE_length              1
struct GFX30_ROW_INSTDONE {
   bool                                 BCDone;
   bool                                 PSDDone;
   bool                                 TDPDone;
   bool                                 DAPRDone;
   bool                                 CPSSDone;
   bool                                 RTDONERENDER;
   bool                                 TDLDone;
   bool                                 EU00doneSS1;
   bool                                 EU01doneSS1;
   bool                                 EU02doneSS1;
   bool                                 EU03doneSS1;
   bool                                 MA0doneSS1;
   bool                                 ICDone;
   bool                                 BTDDONERENDER;
   bool                                 RTDONECOMPUTE;
   bool                                 MA0DoneSS0;
   bool                                 EU00DoneSS0;
   bool                                 EU01DoneSS0;
   bool                                 EU02DoneSS0;
   bool                                 EU03DoneSS0;
   bool                                 BTDDONECOMPUTE;
   bool                                 EU10DoneSS0;
   bool                                 EU11DoneSS0;
   bool                                 EU12DoneSS0;
   bool                                 EU13DoneSS0;
   bool                                 TSLDone;
   bool                                 EU10DoneSS1;
   bool                                 EU11DoneSS1;
   bool                                 EU12DoneSS1;
   bool                                 EU13DoneSS1;
};

static inline __attribute__((always_inline)) void
GFX30_ROW_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_ROW_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->BCDone, 0, 0) |
      util_bitpack_uint(values->PSDDone, 1, 1) |
      util_bitpack_uint(values->TDPDone, 2, 2) |
      util_bitpack_uint(values->DAPRDone, 3, 3) |
      util_bitpack_uint(values->CPSSDone, 4, 4) |
      util_bitpack_uint(values->RTDONERENDER, 5, 5) |
      util_bitpack_uint(values->TDLDone, 6, 6) |
      util_bitpack_uint(values->EU00doneSS1, 7, 7) |
      util_bitpack_uint(values->EU01doneSS1, 8, 8) |
      util_bitpack_uint(values->EU02doneSS1, 9, 9) |
      util_bitpack_uint(values->EU03doneSS1, 10, 10) |
      util_bitpack_uint(values->MA0doneSS1, 11, 11) |
      util_bitpack_uint(values->ICDone, 12, 12) |
      util_bitpack_uint(values->BTDDONERENDER, 13, 13) |
      util_bitpack_uint(values->RTDONECOMPUTE, 14, 14) |
      util_bitpack_uint(values->MA0DoneSS0, 15, 15) |
      util_bitpack_uint(values->EU00DoneSS0, 16, 16) |
      util_bitpack_uint(values->EU01DoneSS0, 17, 17) |
      util_bitpack_uint(values->EU02DoneSS0, 18, 18) |
      util_bitpack_uint(values->EU03DoneSS0, 19, 19) |
      util_bitpack_uint(values->BTDDONECOMPUTE, 20, 20) |
      util_bitpack_uint(values->EU10DoneSS0, 21, 21) |
      util_bitpack_uint(values->EU11DoneSS0, 22, 22) |
      util_bitpack_uint(values->EU12DoneSS0, 23, 23) |
      util_bitpack_uint(values->EU13DoneSS0, 24, 24) |
      util_bitpack_uint(values->TSLDone, 26, 26) |
      util_bitpack_uint(values->EU10DoneSS1, 27, 27) |
      util_bitpack_uint(values->EU11DoneSS1, 28, 28) |
      util_bitpack_uint(values->EU12DoneSS1, 29, 29) |
      util_bitpack_uint(values->EU13DoneSS1, 30, 30);
}

#define GFX30_RPSTAT0_num                 0xa01c
#define GFX30_RPSTAT0_length                   1
struct GFX30_RPSTAT0 {
   uint32_t                             PreviousGTFrequency;
   uint32_t                             CurrentGTFrequency;
};

static inline __attribute__((always_inline)) void
GFX30_RPSTAT0_pack(__attribute__((unused)) __gen_user_data *data,
                   __attribute__((unused)) void * restrict dst,
                   __attribute__((unused)) const struct GFX30_RPSTAT0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->PreviousGTFrequency, 0, 8) |
      util_bitpack_uint(values->CurrentGTFrequency, 23, 31);
}

#define GFX30_SAMPLER_INSTDONE_num        0xe160
#define GFX30_SAMPLER_INSTDONE_length          1
struct GFX30_SAMPLER_INSTDONE {
   bool                                 SIDone;
   bool                                 SVSMADAPTERDone;
   bool                                 SVSMARBDone;
   bool                                 SVSMDone;
   bool                                 PLDone;
   bool                                 DGDone;
   bool                                 SSLADone;
   bool                                 STARBDone;
   bool                                 STDone;
   bool                                 FTDone;
   bool                                 MTDone;
   bool                                 DMDone;
   bool                                 BDMDone;
   bool                                 SCDone;
   bool                                 FLDone;
   bool                                 SODone;
   bool                                 LSCL1BANK0Idle;
   bool                                 LSCL1BANK1Idle;
   bool                                 LSCL1BANK2Idle;
   bool                                 LSCL1BANK3Idle;
   bool                                 LSCSEQ0Idle;
   bool                                 LSCSEQ1Idle;
   bool                                 LSCSEQ2Idle;
   bool                                 LSCSEQ3Idle;
   bool                                 LSCSEQINTFIdle;
   bool                                 LSCL3INTFIdle;
   bool                                 VMEMSOARB0Done;
   bool                                 VMEMSOARB1Done;
   bool                                 VMEML3REQARB0Done;
   bool                                 VMEML3REQARB1Done;
};

static inline __attribute__((always_inline)) void
GFX30_SAMPLER_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SAMPLER_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->SIDone, 0, 0) |
      util_bitpack_uint(values->SVSMADAPTERDone, 1, 1) |
      util_bitpack_uint(values->SVSMARBDone, 2, 2) |
      util_bitpack_uint(values->SVSMDone, 3, 3) |
      util_bitpack_uint(values->PLDone, 4, 4) |
      util_bitpack_uint(values->DGDone, 5, 5) |
      util_bitpack_uint(values->SSLADone, 6, 6) |
      util_bitpack_uint(values->STARBDone, 7, 7) |
      util_bitpack_uint(values->STDone, 8, 8) |
      util_bitpack_uint(values->FTDone, 9, 9) |
      util_bitpack_uint(values->MTDone, 11, 11) |
      util_bitpack_uint(values->DMDone, 12, 12) |
      util_bitpack_uint(values->BDMDone, 13, 13) |
      util_bitpack_uint(values->SCDone, 14, 14) |
      util_bitpack_uint(values->FLDone, 15, 15) |
      util_bitpack_uint(values->SODone, 16, 16) |
      util_bitpack_uint(values->LSCL1BANK0Idle, 18, 18) |
      util_bitpack_uint(values->LSCL1BANK1Idle, 19, 19) |
      util_bitpack_uint(values->LSCL1BANK2Idle, 20, 20) |
      util_bitpack_uint(values->LSCL1BANK3Idle, 21, 21) |
      util_bitpack_uint(values->LSCSEQ0Idle, 22, 22) |
      util_bitpack_uint(values->LSCSEQ1Idle, 23, 23) |
      util_bitpack_uint(values->LSCSEQ2Idle, 24, 24) |
      util_bitpack_uint(values->LSCSEQ3Idle, 25, 25) |
      util_bitpack_uint(values->LSCSEQINTFIdle, 26, 26) |
      util_bitpack_uint(values->LSCL3INTFIdle, 27, 27) |
      util_bitpack_uint(values->VMEMSOARB0Done, 28, 28) |
      util_bitpack_uint(values->VMEMSOARB1Done, 29, 29) |
      util_bitpack_uint(values->VMEML3REQARB0Done, 30, 30) |
      util_bitpack_uint(values->VMEML3REQARB1Done, 31, 31);
}

#define GFX30_SC_INSTDONE_num             0x7100
#define GFX30_SC_INSTDONE_length               1
struct GFX30_SC_INSTDONE {
   bool                                 SVLDone;
   bool                                 WMFEDone;
   bool                                 WMBEDone;
   bool                                 HIZDone;
   bool                                 IZFEDone;
   bool                                 SBEDone;
   bool                                 RCCDone;
   bool                                 RCPBEDone;
   bool                                 RCPFEDone;
   bool                                 DAPBDone;
   bool                                 DAPRBEDone;
   bool                                 SARBDone;
   bool                                 DC0Done;
   bool                                 DC1Done;
   bool                                 DC2Done;
   bool                                 DC3Done;
   bool                                 GW0Done;
   bool                                 GW1Done;
   bool                                 GW2Done;
   bool                                 GW3Done;
   bool                                 TDCDone;
   bool                                 SFBEDone;
   bool                                 PSSDone;
   bool                                 AMFSDone;
};

static inline __attribute__((always_inline)) void
GFX30_SC_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                       __attribute__((unused)) void * restrict dst,
                       __attribute__((unused)) const struct GFX30_SC_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->SVLDone, 0, 0) |
      util_bitpack_uint(values->WMFEDone, 1, 1) |
      util_bitpack_uint(values->WMBEDone, 2, 2) |
      util_bitpack_uint(values->HIZDone, 3, 3) |
      util_bitpack_uint(values->IZFEDone, 5, 5) |
      util_bitpack_uint(values->SBEDone, 6, 6) |
      util_bitpack_uint(values->RCCDone, 9, 9) |
      util_bitpack_uint(values->RCPBEDone, 10, 10) |
      util_bitpack_uint(values->RCPFEDone, 11, 11) |
      util_bitpack_uint(values->DAPBDone, 12, 12) |
      util_bitpack_uint(values->DAPRBEDone, 13, 13) |
      util_bitpack_uint(values->SARBDone, 15, 15) |
      util_bitpack_uint(values->DC0Done, 16, 16) |
      util_bitpack_uint(values->DC1Done, 17, 17) |
      util_bitpack_uint(values->DC2Done, 18, 18) |
      util_bitpack_uint(values->DC3Done, 19, 19) |
      util_bitpack_uint(values->GW0Done, 20, 20) |
      util_bitpack_uint(values->GW1Done, 21, 21) |
      util_bitpack_uint(values->GW2Done, 22, 22) |
      util_bitpack_uint(values->GW3Done, 23, 23) |
      util_bitpack_uint(values->TDCDone, 24, 24) |
      util_bitpack_uint(values->SFBEDone, 25, 25) |
      util_bitpack_uint(values->PSSDone, 26, 26) |
      util_bitpack_uint(values->AMFSDone, 27, 27);
}

#define GFX30_SC_INSTDONE_EXTRA_num       0x7104
#define GFX30_SC_INSTDONE_EXTRA_length         1
struct GFX30_SC_INSTDONE_EXTRA {
   bool                                 RCC1Done;
   bool                                 RCPBE1Done;
   bool                                 RCPFE1Done;
   bool                                 DAPB1Done;
   bool                                 DAPRBE1Done;
   bool                                 DC4Done;
   bool                                 DC5Done;
   bool                                 DC6Done;
   bool                                 DC7Done;
   bool                                 GW4Done;
   bool                                 GW5Done;
   bool                                 GW6Done;
   bool                                 GW7Done;
   bool                                 TDC1Done;
   bool                                 PSSDone;
};

static inline __attribute__((always_inline)) void
GFX30_SC_INSTDONE_EXTRA_pack(__attribute__((unused)) __gen_user_data *data,
                             __attribute__((unused)) void * restrict dst,
                             __attribute__((unused)) const struct GFX30_SC_INSTDONE_EXTRA * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RCC1Done, 9, 9) |
      util_bitpack_uint(values->RCPBE1Done, 10, 10) |
      util_bitpack_uint(values->RCPFE1Done, 11, 11) |
      util_bitpack_uint(values->DAPB1Done, 12, 12) |
      util_bitpack_uint(values->DAPRBE1Done, 13, 13) |
      util_bitpack_uint(values->DC4Done, 16, 16) |
      util_bitpack_uint(values->DC5Done, 17, 17) |
      util_bitpack_uint(values->DC6Done, 18, 18) |
      util_bitpack_uint(values->DC7Done, 19, 19) |
      util_bitpack_uint(values->GW4Done, 20, 20) |
      util_bitpack_uint(values->GW5Done, 21, 21) |
      util_bitpack_uint(values->GW6Done, 22, 22) |
      util_bitpack_uint(values->GW7Done, 23, 23) |
      util_bitpack_uint(values->TDC1Done, 24, 24) |
      util_bitpack_uint(values->PSSDone, 26, 26);
}

#define GFX30_SC_INSTDONE_EXTRA2_num      0x7108
#define GFX30_SC_INSTDONE_EXTRA2_length        1
struct GFX30_SC_INSTDONE_EXTRA2 {
   bool                                 RCC2Done;
   bool                                 RCPBE2Done;
   bool                                 RCPFE2Done;
   bool                                 DAPB2Done;
   bool                                 DAPRBE2Done;
};

static inline __attribute__((always_inline)) void
GFX30_SC_INSTDONE_EXTRA2_pack(__attribute__((unused)) __gen_user_data *data,
                              __attribute__((unused)) void * restrict dst,
                              __attribute__((unused)) const struct GFX30_SC_INSTDONE_EXTRA2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RCC2Done, 9, 9) |
      util_bitpack_uint(values->RCPBE2Done, 10, 10) |
      util_bitpack_uint(values->RCPFE2Done, 11, 11) |
      util_bitpack_uint(values->DAPB2Done, 12, 12) |
      util_bitpack_uint(values->DAPRBE2Done, 13, 13);
}

#define GFX30_SLICE_COMMON_ECO_CHICKEN1_num 0x731c
#define GFX30_SLICE_COMMON_ECO_CHICKEN1_length      1
struct GFX30_SLICE_COMMON_ECO_CHICKEN1 {
   bool                                 StateCacheRedirectToCSSectionEnable;
   bool                                 StateCacheRedirectToCSSectionEnableMask;
};

static inline __attribute__((always_inline)) void
GFX30_SLICE_COMMON_ECO_CHICKEN1_pack(__attribute__((unused)) __gen_user_data *data,
                                     __attribute__((unused)) void * restrict dst,
                                     __attribute__((unused)) const struct GFX30_SLICE_COMMON_ECO_CHICKEN1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->StateCacheRedirectToCSSectionEnable, 11, 11) |
      util_bitpack_uint(values->StateCacheRedirectToCSSectionEnableMask, 27, 27);
}

#define GFX30_SO_NUM_PRIMS_WRITTEN0_num   0x5200
#define GFX30_SO_NUM_PRIMS_WRITTEN0_length      2
struct GFX30_SO_NUM_PRIMS_WRITTEN0 {
   uint64_t                             NumPrimsWrittenCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_NUM_PRIMS_WRITTEN0_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_SO_NUM_PRIMS_WRITTEN0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->NumPrimsWrittenCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_NUM_PRIMS_WRITTEN1_num   0x5208
#define GFX30_SO_NUM_PRIMS_WRITTEN1_length      2
struct GFX30_SO_NUM_PRIMS_WRITTEN1 {
   uint64_t                             NumPrimsWrittenCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_NUM_PRIMS_WRITTEN1_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_SO_NUM_PRIMS_WRITTEN1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->NumPrimsWrittenCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_NUM_PRIMS_WRITTEN2_num   0x5210
#define GFX30_SO_NUM_PRIMS_WRITTEN2_length      2
struct GFX30_SO_NUM_PRIMS_WRITTEN2 {
   uint64_t                             NumPrimsWrittenCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_NUM_PRIMS_WRITTEN2_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_SO_NUM_PRIMS_WRITTEN2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->NumPrimsWrittenCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_NUM_PRIMS_WRITTEN3_num   0x5218
#define GFX30_SO_NUM_PRIMS_WRITTEN3_length      2
struct GFX30_SO_NUM_PRIMS_WRITTEN3 {
   uint64_t                             NumPrimsWrittenCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_NUM_PRIMS_WRITTEN3_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_SO_NUM_PRIMS_WRITTEN3 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->NumPrimsWrittenCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_PRIM_STORAGE_NEEDED0_num 0x5240
#define GFX30_SO_PRIM_STORAGE_NEEDED0_length      2
struct GFX30_SO_PRIM_STORAGE_NEEDED0 {
   uint64_t                             PrimStorageNeededCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_PRIM_STORAGE_NEEDED0_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_SO_PRIM_STORAGE_NEEDED0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->PrimStorageNeededCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_PRIM_STORAGE_NEEDED1_num 0x5248
#define GFX30_SO_PRIM_STORAGE_NEEDED1_length      2
struct GFX30_SO_PRIM_STORAGE_NEEDED1 {
   uint64_t                             PrimStorageNeededCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_PRIM_STORAGE_NEEDED1_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_SO_PRIM_STORAGE_NEEDED1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->PrimStorageNeededCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_PRIM_STORAGE_NEEDED2_num 0x5250
#define GFX30_SO_PRIM_STORAGE_NEEDED2_length      2
struct GFX30_SO_PRIM_STORAGE_NEEDED2 {
   uint64_t                             PrimStorageNeededCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_PRIM_STORAGE_NEEDED2_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_SO_PRIM_STORAGE_NEEDED2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->PrimStorageNeededCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_PRIM_STORAGE_NEEDED3_num 0x5258
#define GFX30_SO_PRIM_STORAGE_NEEDED3_length      2
struct GFX30_SO_PRIM_STORAGE_NEEDED3 {
   uint64_t                             PrimStorageNeededCount;
};

static inline __attribute__((always_inline)) void
GFX30_SO_PRIM_STORAGE_NEEDED3_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_SO_PRIM_STORAGE_NEEDED3 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->PrimStorageNeededCount, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_SO_WRITE_OFFSET0_num        0x5280
#define GFX30_SO_WRITE_OFFSET0_length          1
struct GFX30_SO_WRITE_OFFSET0 {
   uint64_t                             WriteOffset;
};

static inline __attribute__((always_inline)) void
GFX30_SO_WRITE_OFFSET0_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SO_WRITE_OFFSET0 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->WriteOffset, 2, 31);
}

#define GFX30_SO_WRITE_OFFSET1_num        0x5284
#define GFX30_SO_WRITE_OFFSET1_length          1
struct GFX30_SO_WRITE_OFFSET1 {
   uint64_t                             WriteOffset;
};

static inline __attribute__((always_inline)) void
GFX30_SO_WRITE_OFFSET1_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SO_WRITE_OFFSET1 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->WriteOffset, 2, 31);
}

#define GFX30_SO_WRITE_OFFSET2_num        0x5288
#define GFX30_SO_WRITE_OFFSET2_length          1
struct GFX30_SO_WRITE_OFFSET2 {
   uint64_t                             WriteOffset;
};

static inline __attribute__((always_inline)) void
GFX30_SO_WRITE_OFFSET2_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SO_WRITE_OFFSET2 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->WriteOffset, 2, 31);
}

#define GFX30_SO_WRITE_OFFSET3_num        0x528c
#define GFX30_SO_WRITE_OFFSET3_length          1
struct GFX30_SO_WRITE_OFFSET3 {
   uint64_t                             WriteOffset;
};

static inline __attribute__((always_inline)) void
GFX30_SO_WRITE_OFFSET3_pack(__attribute__((unused)) __gen_user_data *data,
                            __attribute__((unused)) void * restrict dst,
                            __attribute__((unused)) const struct GFX30_SO_WRITE_OFFSET3 * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      __gen_offset(values->WriteOffset, 2, 31);
}

#define GFX30_TASK_INVOCATION_COUNT_num   0x26e8
#define GFX30_TASK_INVOCATION_COUNT_length      2
struct GFX30_TASK_INVOCATION_COUNT {
   uint64_t                             TASKInvocationCounter;
};

static inline __attribute__((always_inline)) void
GFX30_TASK_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                                 __attribute__((unused)) void * restrict dst,
                                 __attribute__((unused)) const struct GFX30_TASK_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->TASKInvocationCounter, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_VCS_INSTDONE_num            0x1206c
#define GFX30_VCS_INSTDONE_length              1
struct GFX30_VCS_INSTDONE {
   bool                                 RingEnable;
   bool                                 USBDone;
   bool                                 QRCDone;
   bool                                 SECDone;
   bool                                 MPCDone;
   bool                                 VFTDone;
   bool                                 BSPDone;
   bool                                 VLFDone;
   bool                                 VOPDone;
   bool                                 VMCDone;
   bool                                 VIPDone;
   bool                                 VITDone;
   bool                                 VDSDone;
   bool                                 VMXDone;
   bool                                 VCPDone;
   bool                                 VCDDone;
   bool                                 VADDone;
   bool                                 VMDDone;
   bool                                 VISDone;
   bool                                 VACDone;
   bool                                 VAMDone;
   bool                                 JPGDone;
   bool                                 VBPDone;
   bool                                 VHRDone;
   bool                                 VCIDone;
   bool                                 VINDone;
   bool                                 VPRDone;
   bool                                 VTQDone;
   bool                                 Reserved;
   bool                                 VCSDone;
   bool                                 GACDone;
};

static inline __attribute__((always_inline)) void
GFX30_VCS_INSTDONE_pack(__attribute__((unused)) __gen_user_data *data,
                        __attribute__((unused)) void * restrict dst,
                        __attribute__((unused)) const struct GFX30_VCS_INSTDONE * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->RingEnable, 0, 0) |
      util_bitpack_uint(values->USBDone, 1, 1) |
      util_bitpack_uint(values->QRCDone, 2, 2) |
      util_bitpack_uint(values->SECDone, 3, 3) |
      util_bitpack_uint(values->MPCDone, 4, 4) |
      util_bitpack_uint(values->VFTDone, 5, 5) |
      util_bitpack_uint(values->BSPDone, 6, 6) |
      util_bitpack_uint(values->VLFDone, 7, 7) |
      util_bitpack_uint(values->VOPDone, 8, 8) |
      util_bitpack_uint(values->VMCDone, 9, 9) |
      util_bitpack_uint(values->VIPDone, 10, 10) |
      util_bitpack_uint(values->VITDone, 11, 11) |
      util_bitpack_uint(values->VDSDone, 12, 12) |
      util_bitpack_uint(values->VMXDone, 13, 13) |
      util_bitpack_uint(values->VCPDone, 14, 14) |
      util_bitpack_uint(values->VCDDone, 15, 15) |
      util_bitpack_uint(values->VADDone, 16, 16) |
      util_bitpack_uint(values->VMDDone, 17, 17) |
      util_bitpack_uint(values->VISDone, 18, 18) |
      util_bitpack_uint(values->VACDone, 19, 19) |
      util_bitpack_uint(values->VAMDone, 20, 20) |
      util_bitpack_uint(values->JPGDone, 21, 21) |
      util_bitpack_uint(values->VBPDone, 22, 22) |
      util_bitpack_uint(values->VHRDone, 23, 23) |
      util_bitpack_uint(values->VCIDone, 24, 24) |
      util_bitpack_uint(values->VINDone, 26, 26) |
      util_bitpack_uint(values->VPRDone, 27, 27) |
      util_bitpack_uint(values->VTQDone, 28, 28) |
      util_bitpack_uint(values->Reserved, 29, 29) |
      util_bitpack_uint(values->VCSDone, 30, 30) |
      util_bitpack_uint(values->GACDone, 31, 31);
}

#define GFX30_VD0_AUX_TABLE_BASE_ADDR_num 0x4210
#define GFX30_VD0_AUX_TABLE_BASE_ADDR_length      2
struct GFX30_VD0_AUX_TABLE_BASE_ADDR {
   uint64_t                             Address;
};

static inline __attribute__((always_inline)) void
GFX30_VD0_AUX_TABLE_BASE_ADDR_pack(__attribute__((unused)) __gen_user_data *data,
                                   __attribute__((unused)) void * restrict dst,
                                   __attribute__((unused)) const struct GFX30_VD0_AUX_TABLE_BASE_ADDR * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->Address, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#define GFX30_VD0_CCS_AUX_INV_num         0x4218
#define GFX30_VD0_CCS_AUX_INV_length           1
struct GFX30_VD0_CCS_AUX_INV {
   bool                                 AuxInv;
};

static inline __attribute__((always_inline)) void
GFX30_VD0_CCS_AUX_INV_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_VD0_CCS_AUX_INV * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AuxInv, 0, 0);
}

#define GFX30_VFL_SCRATCH_PAD_num         0x62a8
#define GFX30_VFL_SCRATCH_PAD_length           1
struct GFX30_VFL_SCRATCH_PAD {
   uint32_t                             AutostripDisable;
   uint32_t                             PartialAutostripDisable;
   bool                                 AutostripDisableMask;
   bool                                 PartialAutostripDisableMask;
};

static inline __attribute__((always_inline)) void
GFX30_VFL_SCRATCH_PAD_pack(__attribute__((unused)) __gen_user_data *data,
                           __attribute__((unused)) void * restrict dst,
                           __attribute__((unused)) const struct GFX30_VFL_SCRATCH_PAD * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   dw[0] =
      util_bitpack_uint(values->AutostripDisable, 6, 6) |
      util_bitpack_uint(values->PartialAutostripDisable, 9, 9) |
      util_bitpack_uint(values->AutostripDisableMask, 22, 22) |
      util_bitpack_uint(values->PartialAutostripDisableMask, 25, 25);
}

#define GFX30_VS_INVOCATION_COUNT_num     0x2320
#define GFX30_VS_INVOCATION_COUNT_length       2
struct GFX30_VS_INVOCATION_COUNT {
   uint64_t                             VSInvocationCountReport;
};

static inline __attribute__((always_inline)) void
GFX30_VS_INVOCATION_COUNT_pack(__attribute__((unused)) __gen_user_data *data,
                               __attribute__((unused)) void * restrict dst,
                               __attribute__((unused)) const struct GFX30_VS_INVOCATION_COUNT * restrict values)
{
   uint32_t * restrict dw = (uint32_t * restrict) dst;

   const uint64_t v0 =
      util_bitpack_uint(values->VSInvocationCountReport, 0, 63);
   dw[0] = v0;
   dw[1] = v0 >> 32;
}

#endif /* GFX30_XE3_PACK_H */
