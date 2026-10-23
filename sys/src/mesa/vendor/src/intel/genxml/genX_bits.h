/*
 * Copyright © 2017 Intel Corporation
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

/* THIS FILE HAS BEEN GENERATED, DO NOT HAND EDIT.
 *
 * Sizes of bitfields in genxml instructions, structures, and registers.
 */

#ifndef GENX_BITS_H
#define GENX_BITS_H

#include <stdint.h>

#include "dev/intel_device_info.h"
#include "util/macros.h"



#ifdef __cplusplus
extern "C" {
#endif

/* 3DMESH_3D */


#ifndef GFX35_3DMESH_3D_length
#define GFX35_3DMESH_3D_length  4
#endif
#ifndef GFX30_3DMESH_3D_length
#define GFX30_3DMESH_3D_length  4
#endif
#ifndef GFX20_3DMESH_3D_length
#define GFX20_3DMESH_3D_length  4
#endif
#ifndef GFX125_3DMESH_3D_length
#define GFX125_3DMESH_3D_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DMESH_3D_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DPRIMITIVE */


#ifndef GFX35_3DPRIMITIVE_length
#define GFX35_3DPRIMITIVE_length  7
#endif
#ifndef GFX30_3DPRIMITIVE_length
#define GFX30_3DPRIMITIVE_length  7
#endif
#ifndef GFX20_3DPRIMITIVE_length
#define GFX20_3DPRIMITIVE_length  7
#endif
#ifndef GFX125_3DPRIMITIVE_length
#define GFX125_3DPRIMITIVE_length  7
#endif
#ifndef GFX12_3DPRIMITIVE_length
#define GFX12_3DPRIMITIVE_length  7
#endif
#ifndef GFX11_3DPRIMITIVE_length
#define GFX11_3DPRIMITIVE_length  7
#endif
#ifndef GFX9_3DPRIMITIVE_length
#define GFX9_3DPRIMITIVE_length  7
#endif
#ifndef GFX8_3DPRIMITIVE_length
#define GFX8_3DPRIMITIVE_length  7
#endif
#ifndef GFX75_3DPRIMITIVE_length
#define GFX75_3DPRIMITIVE_length  7
#endif
#ifndef GFX7_3DPRIMITIVE_length
#define GFX7_3DPRIMITIVE_length  7
#endif
#ifndef GFX6_3DPRIMITIVE_length
#define GFX6_3DPRIMITIVE_length  6
#endif
#ifndef GFX5_3DPRIMITIVE_length
#define GFX5_3DPRIMITIVE_length  6
#endif
#ifndef GFX45_3DPRIMITIVE_length
#define GFX45_3DPRIMITIVE_length  6
#endif
#ifndef GFX4_3DPRIMITIVE_length
#define GFX4_3DPRIMITIVE_length  6
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DPRIMITIVE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 7;
   case 300: return 7;
   case 200: return 7;
   case 125: return 7;
   case 120: return 7;
   case 110: return 7;
   case 90: return 7;
   case 80: return 7;
   case 75: return 7;
   case 70: return 7;
   case 60: return 6;
   case 50: return 6;
   case 45: return 6;
   case 40: return 6;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DPRIMITIVE_EXTENDED */


#ifndef GFX35_3DPRIMITIVE_EXTENDED_length
#define GFX35_3DPRIMITIVE_EXTENDED_length  10
#endif
#ifndef GFX30_3DPRIMITIVE_EXTENDED_length
#define GFX30_3DPRIMITIVE_EXTENDED_length  10
#endif
#ifndef GFX20_3DPRIMITIVE_EXTENDED_length
#define GFX20_3DPRIMITIVE_EXTENDED_length  10
#endif
#ifndef GFX125_3DPRIMITIVE_EXTENDED_length
#define GFX125_3DPRIMITIVE_EXTENDED_length  10
#endif
#ifndef GFX12_3DPRIMITIVE_EXTENDED_length
#define GFX12_3DPRIMITIVE_EXTENDED_length  10
#endif
#ifndef GFX11_3DPRIMITIVE_EXTENDED_length
#define GFX11_3DPRIMITIVE_EXTENDED_length  10
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DPRIMITIVE_EXTENDED_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 10;
   case 300: return 10;
   case 200: return 10;
   case 125: return 10;
   case 120: return 10;
   case 110: return 10;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CLEAR_PARAMS */


#ifndef GFX125_3DSTATE_CLEAR_PARAMS_length
#define GFX125_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX12_3DSTATE_CLEAR_PARAMS_length
#define GFX12_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX11_3DSTATE_CLEAR_PARAMS_length
#define GFX11_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX9_3DSTATE_CLEAR_PARAMS_length
#define GFX9_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX8_3DSTATE_CLEAR_PARAMS_length
#define GFX8_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX75_3DSTATE_CLEAR_PARAMS_length
#define GFX75_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX7_3DSTATE_CLEAR_PARAMS_length
#define GFX7_3DSTATE_CLEAR_PARAMS_length  3
#endif
#ifndef GFX6_3DSTATE_CLEAR_PARAMS_length
#define GFX6_3DSTATE_CLEAR_PARAMS_length  2
#endif
#ifndef GFX5_3DSTATE_CLEAR_PARAMS_length
#define GFX5_3DSTATE_CLEAR_PARAMS_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CLEAR_PARAMS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 3;
   case 120: return 3;
   case 110: return 3;
   case 90: return 3;
   case 80: return 3;
   case 75: return 3;
   case 70: return 3;
   case 60: return 2;
   case 50: return 2;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CLIP */


#ifndef GFX35_3DSTATE_CLIP_length
#define GFX35_3DSTATE_CLIP_length  4
#endif
#ifndef GFX30_3DSTATE_CLIP_length
#define GFX30_3DSTATE_CLIP_length  4
#endif
#ifndef GFX20_3DSTATE_CLIP_length
#define GFX20_3DSTATE_CLIP_length  4
#endif
#ifndef GFX125_3DSTATE_CLIP_length
#define GFX125_3DSTATE_CLIP_length  4
#endif
#ifndef GFX12_3DSTATE_CLIP_length
#define GFX12_3DSTATE_CLIP_length  4
#endif
#ifndef GFX11_3DSTATE_CLIP_length
#define GFX11_3DSTATE_CLIP_length  4
#endif
#ifndef GFX9_3DSTATE_CLIP_length
#define GFX9_3DSTATE_CLIP_length  4
#endif
#ifndef GFX8_3DSTATE_CLIP_length
#define GFX8_3DSTATE_CLIP_length  4
#endif
#ifndef GFX75_3DSTATE_CLIP_length
#define GFX75_3DSTATE_CLIP_length  4
#endif
#ifndef GFX7_3DSTATE_CLIP_length
#define GFX7_3DSTATE_CLIP_length  4
#endif
#ifndef GFX6_3DSTATE_CLIP_length
#define GFX6_3DSTATE_CLIP_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CLIP_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CLIP_MESH */


#ifndef GFX35_3DSTATE_CLIP_MESH_length
#define GFX35_3DSTATE_CLIP_MESH_length  2
#endif
#ifndef GFX30_3DSTATE_CLIP_MESH_length
#define GFX30_3DSTATE_CLIP_MESH_length  2
#endif
#ifndef GFX20_3DSTATE_CLIP_MESH_length
#define GFX20_3DSTATE_CLIP_MESH_length  2
#endif
#ifndef GFX125_3DSTATE_CLIP_MESH_length
#define GFX125_3DSTATE_CLIP_MESH_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CLIP_MESH_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CONSTANT_ALL */


#ifndef GFX35_3DSTATE_CONSTANT_ALL_length
#define GFX35_3DSTATE_CONSTANT_ALL_length  2
#endif
#ifndef GFX30_3DSTATE_CONSTANT_ALL_length
#define GFX30_3DSTATE_CONSTANT_ALL_length  2
#endif
#ifndef GFX20_3DSTATE_CONSTANT_ALL_length
#define GFX20_3DSTATE_CONSTANT_ALL_length  2
#endif
#ifndef GFX125_3DSTATE_CONSTANT_ALL_length
#define GFX125_3DSTATE_CONSTANT_ALL_length  2
#endif
#ifndef GFX12_3DSTATE_CONSTANT_ALL_length
#define GFX12_3DSTATE_CONSTANT_ALL_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CONSTANT_ALL_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CONSTANT_ALL_DATA */


#ifndef GFX35_3DSTATE_CONSTANT_ALL_DATA_length
#define GFX35_3DSTATE_CONSTANT_ALL_DATA_length  2
#endif
#ifndef GFX30_3DSTATE_CONSTANT_ALL_DATA_length
#define GFX30_3DSTATE_CONSTANT_ALL_DATA_length  2
#endif
#ifndef GFX20_3DSTATE_CONSTANT_ALL_DATA_length
#define GFX20_3DSTATE_CONSTANT_ALL_DATA_length  2
#endif
#ifndef GFX125_3DSTATE_CONSTANT_ALL_DATA_length
#define GFX125_3DSTATE_CONSTANT_ALL_DATA_length  2
#endif
#ifndef GFX12_3DSTATE_CONSTANT_ALL_DATA_length
#define GFX12_3DSTATE_CONSTANT_ALL_DATA_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CONSTANT_ALL_DATA_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CONSTANT_VS */


#ifndef GFX35_3DSTATE_CONSTANT_VS_length
#define GFX35_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX30_3DSTATE_CONSTANT_VS_length
#define GFX30_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX20_3DSTATE_CONSTANT_VS_length
#define GFX20_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX125_3DSTATE_CONSTANT_VS_length
#define GFX125_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX12_3DSTATE_CONSTANT_VS_length
#define GFX12_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX11_3DSTATE_CONSTANT_VS_length
#define GFX11_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX9_3DSTATE_CONSTANT_VS_length
#define GFX9_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX8_3DSTATE_CONSTANT_VS_length
#define GFX8_3DSTATE_CONSTANT_VS_length  11
#endif
#ifndef GFX75_3DSTATE_CONSTANT_VS_length
#define GFX75_3DSTATE_CONSTANT_VS_length  7
#endif
#ifndef GFX7_3DSTATE_CONSTANT_VS_length
#define GFX7_3DSTATE_CONSTANT_VS_length  7
#endif
#ifndef GFX6_3DSTATE_CONSTANT_VS_length
#define GFX6_3DSTATE_CONSTANT_VS_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CONSTANT_VS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 11;
   case 75: return 7;
   case 70: return 7;
   case 60: return 5;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CPSIZE_CONTROL_BUFFER */


#ifndef GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_length
#define GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_length  8
#endif
#ifndef GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_length
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_length  8
#endif
#ifndef GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_length
#define GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_length  8
#endif
#ifndef GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_length
#define GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CPSIZE_CONTROL_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 8;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_CPSIZE_CONTROL_BUFFER::Surface Base Address */


#define GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_bits  64

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_start  64
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_start  64
#define GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_start  64
#define GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_start  64

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_CPSIZE_CONTROL_BUFFER::Surface Pitch */


#define GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_bits  17
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_bits  17
#define GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_bits  17
#define GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_bits  17

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 17;
   case 300: return 17;
   case 200: return 17;
   case 125: return 17;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_start  32
#define GFX30_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_start  32
#define GFX20_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_start  32
#define GFX125_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_start  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_CPSIZE_CONTROL_BUFFER_SurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_DEPTH_BUFFER */


#ifndef GFX35_3DSTATE_DEPTH_BUFFER_length
#define GFX35_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX30_3DSTATE_DEPTH_BUFFER_length
#define GFX30_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX20_3DSTATE_DEPTH_BUFFER_length
#define GFX20_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX125_3DSTATE_DEPTH_BUFFER_length
#define GFX125_3DSTATE_DEPTH_BUFFER_length  10
#endif
#ifndef GFX12_3DSTATE_DEPTH_BUFFER_length
#define GFX12_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX11_3DSTATE_DEPTH_BUFFER_length
#define GFX11_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX9_3DSTATE_DEPTH_BUFFER_length
#define GFX9_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX8_3DSTATE_DEPTH_BUFFER_length
#define GFX8_3DSTATE_DEPTH_BUFFER_length  8
#endif
#ifndef GFX75_3DSTATE_DEPTH_BUFFER_length
#define GFX75_3DSTATE_DEPTH_BUFFER_length  7
#endif
#ifndef GFX7_3DSTATE_DEPTH_BUFFER_length
#define GFX7_3DSTATE_DEPTH_BUFFER_length  7
#endif
#ifndef GFX6_3DSTATE_DEPTH_BUFFER_length
#define GFX6_3DSTATE_DEPTH_BUFFER_length  7
#endif
#ifndef GFX5_3DSTATE_DEPTH_BUFFER_length
#define GFX5_3DSTATE_DEPTH_BUFFER_length  6
#endif
#ifndef GFX45_3DSTATE_DEPTH_BUFFER_length
#define GFX45_3DSTATE_DEPTH_BUFFER_length  6
#endif
#ifndef GFX4_3DSTATE_DEPTH_BUFFER_length
#define GFX4_3DSTATE_DEPTH_BUFFER_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DEPTH_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 10;
   case 120: return 8;
   case 110: return 8;
   case 90: return 8;
   case 80: return 8;
   case 75: return 7;
   case 70: return 7;
   case 60: return 7;
   case 50: return 6;
   case 45: return 6;
   case 40: return 5;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_DEPTH_BUFFER::Surface Base Address */


#define GFX35_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX30_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX20_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX125_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX12_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX11_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX9_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX8_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX75_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX7_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX6_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX5_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX45_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX4_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX30_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX20_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX125_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX12_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX11_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX9_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX8_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX75_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX7_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX6_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX5_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX45_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX4_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start  64

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DEPTH_BUFFER_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 64;
   case 70: return 64;
   case 60: return 64;
   case 50: return 64;
   case 45: return 64;
   case 40: return 64;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_DEPTH_BUFFER::Surface Pitch */


#define GFX35_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX30_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX20_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX125_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX12_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX11_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX9_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX8_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX75_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX7_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  18
#define GFX6_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX5_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX45_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX4_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits  17

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DEPTH_BUFFER_SurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 18;
   case 300: return 18;
   case 200: return 18;
   case 125: return 18;
   case 120: return 18;
   case 110: return 18;
   case 90: return 18;
   case 80: return 18;
   case 75: return 18;
   case 70: return 18;
   case 60: return 17;
   case 50: return 17;
   case 45: return 17;
   case 40: return 17;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX30_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX20_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX125_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX12_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX11_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX9_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX8_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX75_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX7_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX6_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX5_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX45_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX4_3DSTATE_DEPTH_BUFFER_SurfacePitch_start  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DEPTH_BUFFER_SurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_DS */


#ifndef GFX35_3DSTATE_DS_length
#define GFX35_3DSTATE_DS_length  11
#endif
#ifndef GFX30_3DSTATE_DS_length
#define GFX30_3DSTATE_DS_length  11
#endif
#ifndef GFX20_3DSTATE_DS_length
#define GFX20_3DSTATE_DS_length  11
#endif
#ifndef GFX125_3DSTATE_DS_length
#define GFX125_3DSTATE_DS_length  11
#endif
#ifndef GFX12_3DSTATE_DS_length
#define GFX12_3DSTATE_DS_length  11
#endif
#ifndef GFX11_3DSTATE_DS_length
#define GFX11_3DSTATE_DS_length  11
#endif
#ifndef GFX9_3DSTATE_DS_length
#define GFX9_3DSTATE_DS_length  11
#endif
#ifndef GFX8_3DSTATE_DS_length
#define GFX8_3DSTATE_DS_length  9
#endif
#ifndef GFX75_3DSTATE_DS_length
#define GFX75_3DSTATE_DS_length  6
#endif
#ifndef GFX7_3DSTATE_DS_length
#define GFX7_3DSTATE_DS_length  6
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_DS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 9;
   case 75: return 6;
   case 70: return 6;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_GS */


#ifndef GFX35_3DSTATE_GS_length
#define GFX35_3DSTATE_GS_length  10
#endif
#ifndef GFX30_3DSTATE_GS_length
#define GFX30_3DSTATE_GS_length  10
#endif
#ifndef GFX20_3DSTATE_GS_length
#define GFX20_3DSTATE_GS_length  10
#endif
#ifndef GFX125_3DSTATE_GS_length
#define GFX125_3DSTATE_GS_length  10
#endif
#ifndef GFX12_3DSTATE_GS_length
#define GFX12_3DSTATE_GS_length  10
#endif
#ifndef GFX11_3DSTATE_GS_length
#define GFX11_3DSTATE_GS_length  10
#endif
#ifndef GFX9_3DSTATE_GS_length
#define GFX9_3DSTATE_GS_length  10
#endif
#ifndef GFX8_3DSTATE_GS_length
#define GFX8_3DSTATE_GS_length  10
#endif
#ifndef GFX75_3DSTATE_GS_length
#define GFX75_3DSTATE_GS_length  7
#endif
#ifndef GFX7_3DSTATE_GS_length
#define GFX7_3DSTATE_GS_length  7
#endif
#ifndef GFX6_3DSTATE_GS_length
#define GFX6_3DSTATE_GS_length  7
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_GS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 10;
   case 300: return 10;
   case 200: return 10;
   case 125: return 10;
   case 120: return 10;
   case 110: return 10;
   case 90: return 10;
   case 80: return 10;
   case 75: return 7;
   case 70: return 7;
   case 60: return 7;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_HIER_DEPTH_BUFFER */


#ifndef GFX35_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX35_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX30_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX20_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX20_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX125_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX125_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX12_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX12_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX11_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX11_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX9_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX9_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX8_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX8_3DSTATE_HIER_DEPTH_BUFFER_length  5
#endif
#ifndef GFX75_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX75_3DSTATE_HIER_DEPTH_BUFFER_length  3
#endif
#ifndef GFX7_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX7_3DSTATE_HIER_DEPTH_BUFFER_length  3
#endif
#ifndef GFX6_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX6_3DSTATE_HIER_DEPTH_BUFFER_length  3
#endif
#ifndef GFX5_3DSTATE_HIER_DEPTH_BUFFER_length
#define GFX5_3DSTATE_HIER_DEPTH_BUFFER_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HIER_DEPTH_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 5;
   case 120: return 5;
   case 110: return 5;
   case 90: return 5;
   case 80: return 5;
   case 75: return 3;
   case 70: return 3;
   case 60: return 3;
   case 50: return 3;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_HIER_DEPTH_BUFFER::Surface Base Address */


#define GFX35_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX20_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX125_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX12_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX11_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX9_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX8_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  64
#define GFX75_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX7_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX6_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  32
#define GFX5_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX20_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX125_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX12_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX11_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX9_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX8_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX75_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX7_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX6_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64
#define GFX5_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start  64

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HIER_DEPTH_BUFFER_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 64;
   case 70: return 64;
   case 60: return 64;
   case 50: return 64;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_HIER_DEPTH_BUFFER::Surface Pitch */


#define GFX35_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX20_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX125_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX12_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX11_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX9_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX8_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX75_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX7_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX6_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17
#define GFX5_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits  17

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 17;
   case 300: return 17;
   case 200: return 17;
   case 125: return 17;
   case 120: return 17;
   case 110: return 17;
   case 90: return 17;
   case 80: return 17;
   case 75: return 17;
   case 70: return 17;
   case 60: return 17;
   case 50: return 17;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX30_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX20_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX125_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX12_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX11_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX9_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX8_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX75_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX7_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX6_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32
#define GFX5_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HIER_DEPTH_BUFFER_SurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_HS */


#ifndef GFX35_3DSTATE_HS_length
#define GFX35_3DSTATE_HS_length  8
#endif
#ifndef GFX30_3DSTATE_HS_length
#define GFX30_3DSTATE_HS_length  8
#endif
#ifndef GFX20_3DSTATE_HS_length
#define GFX20_3DSTATE_HS_length  8
#endif
#ifndef GFX125_3DSTATE_HS_length
#define GFX125_3DSTATE_HS_length  9
#endif
#ifndef GFX12_3DSTATE_HS_length
#define GFX12_3DSTATE_HS_length  9
#endif
#ifndef GFX11_3DSTATE_HS_length
#define GFX11_3DSTATE_HS_length  9
#endif
#ifndef GFX9_3DSTATE_HS_length
#define GFX9_3DSTATE_HS_length  9
#endif
#ifndef GFX8_3DSTATE_HS_length
#define GFX8_3DSTATE_HS_length  9
#endif
#ifndef GFX75_3DSTATE_HS_length
#define GFX75_3DSTATE_HS_length  7
#endif
#ifndef GFX7_3DSTATE_HS_length
#define GFX7_3DSTATE_HS_length  7
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_HS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 9;
   case 120: return 9;
   case 110: return 9;
   case 90: return 9;
   case 80: return 9;
   case 75: return 7;
   case 70: return 7;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_INDEX_BUFFER */


#ifndef GFX35_3DSTATE_INDEX_BUFFER_length
#define GFX35_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX30_3DSTATE_INDEX_BUFFER_length
#define GFX30_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX20_3DSTATE_INDEX_BUFFER_length
#define GFX20_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX125_3DSTATE_INDEX_BUFFER_length
#define GFX125_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX12_3DSTATE_INDEX_BUFFER_length
#define GFX12_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX11_3DSTATE_INDEX_BUFFER_length
#define GFX11_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX9_3DSTATE_INDEX_BUFFER_length
#define GFX9_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX8_3DSTATE_INDEX_BUFFER_length
#define GFX8_3DSTATE_INDEX_BUFFER_length  5
#endif
#ifndef GFX75_3DSTATE_INDEX_BUFFER_length
#define GFX75_3DSTATE_INDEX_BUFFER_length  3
#endif
#ifndef GFX7_3DSTATE_INDEX_BUFFER_length
#define GFX7_3DSTATE_INDEX_BUFFER_length  3
#endif
#ifndef GFX6_3DSTATE_INDEX_BUFFER_length
#define GFX6_3DSTATE_INDEX_BUFFER_length  3
#endif
#ifndef GFX5_3DSTATE_INDEX_BUFFER_length
#define GFX5_3DSTATE_INDEX_BUFFER_length  3
#endif
#ifndef GFX45_3DSTATE_INDEX_BUFFER_length
#define GFX45_3DSTATE_INDEX_BUFFER_length  3
#endif
#ifndef GFX4_3DSTATE_INDEX_BUFFER_length
#define GFX4_3DSTATE_INDEX_BUFFER_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_INDEX_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 5;
   case 120: return 5;
   case 110: return 5;
   case 90: return 5;
   case 80: return 5;
   case 75: return 3;
   case 70: return 3;
   case 60: return 3;
   case 50: return 3;
   case 45: return 3;
   case 40: return 3;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_MESH_CONTROL */


#ifndef GFX35_3DSTATE_MESH_CONTROL_length
#define GFX35_3DSTATE_MESH_CONTROL_length  3
#endif
#ifndef GFX30_3DSTATE_MESH_CONTROL_length
#define GFX30_3DSTATE_MESH_CONTROL_length  3
#endif
#ifndef GFX20_3DSTATE_MESH_CONTROL_length
#define GFX20_3DSTATE_MESH_CONTROL_length  3
#endif
#ifndef GFX125_3DSTATE_MESH_CONTROL_length
#define GFX125_3DSTATE_MESH_CONTROL_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_MESH_CONTROL_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_MESH_DISTRIB */


#ifndef GFX35_3DSTATE_MESH_DISTRIB_length
#define GFX35_3DSTATE_MESH_DISTRIB_length  2
#endif
#ifndef GFX30_3DSTATE_MESH_DISTRIB_length
#define GFX30_3DSTATE_MESH_DISTRIB_length  2
#endif
#ifndef GFX20_3DSTATE_MESH_DISTRIB_length
#define GFX20_3DSTATE_MESH_DISTRIB_length  2
#endif
#ifndef GFX125_3DSTATE_MESH_DISTRIB_length
#define GFX125_3DSTATE_MESH_DISTRIB_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_MESH_DISTRIB_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_MESH_SHADER */


#ifndef GFX35_3DSTATE_MESH_SHADER_length
#define GFX35_3DSTATE_MESH_SHADER_length  8
#endif
#ifndef GFX30_3DSTATE_MESH_SHADER_length
#define GFX30_3DSTATE_MESH_SHADER_length  8
#endif
#ifndef GFX20_3DSTATE_MESH_SHADER_length
#define GFX20_3DSTATE_MESH_SHADER_length  8
#endif
#ifndef GFX125_3DSTATE_MESH_SHADER_length
#define GFX125_3DSTATE_MESH_SHADER_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_MESH_SHADER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 8;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_MESH_SHADER_DATA */


#ifndef GFX35_3DSTATE_MESH_SHADER_DATA_length
#define GFX35_3DSTATE_MESH_SHADER_DATA_length  10
#endif
#ifndef GFX30_3DSTATE_MESH_SHADER_DATA_length
#define GFX30_3DSTATE_MESH_SHADER_DATA_length  10
#endif
#ifndef GFX20_3DSTATE_MESH_SHADER_DATA_length
#define GFX20_3DSTATE_MESH_SHADER_DATA_length  10
#endif
#ifndef GFX125_3DSTATE_MESH_SHADER_DATA_length
#define GFX125_3DSTATE_MESH_SHADER_DATA_length  10
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_MESH_SHADER_DATA_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 10;
   case 300: return 10;
   case 200: return 10;
   case 125: return 10;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_PRIMITIVE_REPLICATION */


#ifndef GFX35_3DSTATE_PRIMITIVE_REPLICATION_length
#define GFX35_3DSTATE_PRIMITIVE_REPLICATION_length  6
#endif
#ifndef GFX30_3DSTATE_PRIMITIVE_REPLICATION_length
#define GFX30_3DSTATE_PRIMITIVE_REPLICATION_length  6
#endif
#ifndef GFX20_3DSTATE_PRIMITIVE_REPLICATION_length
#define GFX20_3DSTATE_PRIMITIVE_REPLICATION_length  6
#endif
#ifndef GFX125_3DSTATE_PRIMITIVE_REPLICATION_length
#define GFX125_3DSTATE_PRIMITIVE_REPLICATION_length  6
#endif
#ifndef GFX12_3DSTATE_PRIMITIVE_REPLICATION_length
#define GFX12_3DSTATE_PRIMITIVE_REPLICATION_length  6
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_PRIMITIVE_REPLICATION_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 6;
   case 300: return 6;
   case 200: return 6;
   case 125: return 6;
   case 120: return 6;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_PS */


#ifndef GFX35_3DSTATE_PS_length
#define GFX35_3DSTATE_PS_length  12
#endif
#ifndef GFX30_3DSTATE_PS_length
#define GFX30_3DSTATE_PS_length  12
#endif
#ifndef GFX20_3DSTATE_PS_length
#define GFX20_3DSTATE_PS_length  12
#endif
#ifndef GFX125_3DSTATE_PS_length
#define GFX125_3DSTATE_PS_length  12
#endif
#ifndef GFX12_3DSTATE_PS_length
#define GFX12_3DSTATE_PS_length  12
#endif
#ifndef GFX11_3DSTATE_PS_length
#define GFX11_3DSTATE_PS_length  12
#endif
#ifndef GFX9_3DSTATE_PS_length
#define GFX9_3DSTATE_PS_length  12
#endif
#ifndef GFX8_3DSTATE_PS_length
#define GFX8_3DSTATE_PS_length  12
#endif
#ifndef GFX75_3DSTATE_PS_length
#define GFX75_3DSTATE_PS_length  8
#endif
#ifndef GFX7_3DSTATE_PS_length
#define GFX7_3DSTATE_PS_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_PS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 12;
   case 300: return 12;
   case 200: return 12;
   case 125: return 12;
   case 120: return 12;
   case 110: return 12;
   case 90: return 12;
   case 80: return 12;
   case 75: return 8;
   case 70: return 8;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_PS_BLEND */


#ifndef GFX35_3DSTATE_PS_BLEND_length
#define GFX35_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX30_3DSTATE_PS_BLEND_length
#define GFX30_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX20_3DSTATE_PS_BLEND_length
#define GFX20_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX125_3DSTATE_PS_BLEND_length
#define GFX125_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX12_3DSTATE_PS_BLEND_length
#define GFX12_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX11_3DSTATE_PS_BLEND_length
#define GFX11_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX9_3DSTATE_PS_BLEND_length
#define GFX9_3DSTATE_PS_BLEND_length  2
#endif
#ifndef GFX8_3DSTATE_PS_BLEND_length
#define GFX8_3DSTATE_PS_BLEND_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_PS_BLEND_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_PS_EXTRA */


#ifndef GFX35_3DSTATE_PS_EXTRA_length
#define GFX35_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX30_3DSTATE_PS_EXTRA_length
#define GFX30_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX20_3DSTATE_PS_EXTRA_length
#define GFX20_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX125_3DSTATE_PS_EXTRA_length
#define GFX125_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX12_3DSTATE_PS_EXTRA_length
#define GFX12_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX11_3DSTATE_PS_EXTRA_length
#define GFX11_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX9_3DSTATE_PS_EXTRA_length
#define GFX9_3DSTATE_PS_EXTRA_length  2
#endif
#ifndef GFX8_3DSTATE_PS_EXTRA_length
#define GFX8_3DSTATE_PS_EXTRA_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_PS_EXTRA_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_PUSH_CONSTANT_ALLOC_VS */


#ifndef GFX35_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX35_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX30_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX20_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX20_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX125_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX125_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX12_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX12_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX11_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX11_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX9_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX9_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX8_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX8_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX75_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX75_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif
#ifndef GFX7_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length
#define GFX7_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_PUSH_CONSTANT_ALLOC_VS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 2;
   case 70: return 2;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_RASTER */


#ifndef GFX35_3DSTATE_RASTER_length
#define GFX35_3DSTATE_RASTER_length  5
#endif
#ifndef GFX30_3DSTATE_RASTER_length
#define GFX30_3DSTATE_RASTER_length  5
#endif
#ifndef GFX20_3DSTATE_RASTER_length
#define GFX20_3DSTATE_RASTER_length  5
#endif
#ifndef GFX125_3DSTATE_RASTER_length
#define GFX125_3DSTATE_RASTER_length  5
#endif
#ifndef GFX12_3DSTATE_RASTER_length
#define GFX12_3DSTATE_RASTER_length  5
#endif
#ifndef GFX11_3DSTATE_RASTER_length
#define GFX11_3DSTATE_RASTER_length  5
#endif
#ifndef GFX9_3DSTATE_RASTER_length
#define GFX9_3DSTATE_RASTER_length  5
#endif
#ifndef GFX8_3DSTATE_RASTER_length
#define GFX8_3DSTATE_RASTER_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_RASTER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 5;
   case 120: return 5;
   case 110: return 5;
   case 90: return 5;
   case 80: return 5;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SBE */


#ifndef GFX35_3DSTATE_SBE_length
#define GFX35_3DSTATE_SBE_length  6
#endif
#ifndef GFX30_3DSTATE_SBE_length
#define GFX30_3DSTATE_SBE_length  6
#endif
#ifndef GFX20_3DSTATE_SBE_length
#define GFX20_3DSTATE_SBE_length  6
#endif
#ifndef GFX125_3DSTATE_SBE_length
#define GFX125_3DSTATE_SBE_length  6
#endif
#ifndef GFX12_3DSTATE_SBE_length
#define GFX12_3DSTATE_SBE_length  6
#endif
#ifndef GFX11_3DSTATE_SBE_length
#define GFX11_3DSTATE_SBE_length  6
#endif
#ifndef GFX9_3DSTATE_SBE_length
#define GFX9_3DSTATE_SBE_length  6
#endif
#ifndef GFX8_3DSTATE_SBE_length
#define GFX8_3DSTATE_SBE_length  4
#endif
#ifndef GFX75_3DSTATE_SBE_length
#define GFX75_3DSTATE_SBE_length  14
#endif
#ifndef GFX7_3DSTATE_SBE_length
#define GFX7_3DSTATE_SBE_length  14
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SBE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 6;
   case 300: return 6;
   case 200: return 6;
   case 125: return 6;
   case 120: return 6;
   case 110: return 6;
   case 90: return 6;
   case 80: return 4;
   case 75: return 14;
   case 70: return 14;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SBE_MESH */


#ifndef GFX35_3DSTATE_SBE_MESH_length
#define GFX35_3DSTATE_SBE_MESH_length  2
#endif
#ifndef GFX30_3DSTATE_SBE_MESH_length
#define GFX30_3DSTATE_SBE_MESH_length  2
#endif
#ifndef GFX20_3DSTATE_SBE_MESH_length
#define GFX20_3DSTATE_SBE_MESH_length  2
#endif
#ifndef GFX125_3DSTATE_SBE_MESH_length
#define GFX125_3DSTATE_SBE_MESH_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SBE_MESH_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SBE_SWIZ */


#ifndef GFX35_3DSTATE_SBE_SWIZ_length
#define GFX35_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX30_3DSTATE_SBE_SWIZ_length
#define GFX30_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX20_3DSTATE_SBE_SWIZ_length
#define GFX20_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX125_3DSTATE_SBE_SWIZ_length
#define GFX125_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX12_3DSTATE_SBE_SWIZ_length
#define GFX12_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX11_3DSTATE_SBE_SWIZ_length
#define GFX11_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX9_3DSTATE_SBE_SWIZ_length
#define GFX9_3DSTATE_SBE_SWIZ_length  11
#endif
#ifndef GFX8_3DSTATE_SBE_SWIZ_length
#define GFX8_3DSTATE_SBE_SWIZ_length  11
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SBE_SWIZ_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 11;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SF */


#ifndef GFX35_3DSTATE_SF_length
#define GFX35_3DSTATE_SF_length  4
#endif
#ifndef GFX30_3DSTATE_SF_length
#define GFX30_3DSTATE_SF_length  4
#endif
#ifndef GFX20_3DSTATE_SF_length
#define GFX20_3DSTATE_SF_length  4
#endif
#ifndef GFX125_3DSTATE_SF_length
#define GFX125_3DSTATE_SF_length  4
#endif
#ifndef GFX12_3DSTATE_SF_length
#define GFX12_3DSTATE_SF_length  4
#endif
#ifndef GFX11_3DSTATE_SF_length
#define GFX11_3DSTATE_SF_length  4
#endif
#ifndef GFX9_3DSTATE_SF_length
#define GFX9_3DSTATE_SF_length  4
#endif
#ifndef GFX8_3DSTATE_SF_length
#define GFX8_3DSTATE_SF_length  4
#endif
#ifndef GFX75_3DSTATE_SF_length
#define GFX75_3DSTATE_SF_length  7
#endif
#ifndef GFX7_3DSTATE_SF_length
#define GFX7_3DSTATE_SF_length  7
#endif
#ifndef GFX6_3DSTATE_SF_length
#define GFX6_3DSTATE_SF_length  20
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SF_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 7;
   case 70: return 7;
   case 60: return 20;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SO_BUFFER */


#ifndef GFX35_3DSTATE_SO_BUFFER_length
#define GFX35_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX30_3DSTATE_SO_BUFFER_length
#define GFX30_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX20_3DSTATE_SO_BUFFER_length
#define GFX20_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX125_3DSTATE_SO_BUFFER_length
#define GFX125_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX12_3DSTATE_SO_BUFFER_length
#define GFX12_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX11_3DSTATE_SO_BUFFER_length
#define GFX11_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX9_3DSTATE_SO_BUFFER_length
#define GFX9_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX8_3DSTATE_SO_BUFFER_length
#define GFX8_3DSTATE_SO_BUFFER_length  8
#endif
#ifndef GFX75_3DSTATE_SO_BUFFER_length
#define GFX75_3DSTATE_SO_BUFFER_length  4
#endif
#ifndef GFX7_3DSTATE_SO_BUFFER_length
#define GFX7_3DSTATE_SO_BUFFER_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SO_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 8;
   case 120: return 8;
   case 110: return 8;
   case 90: return 8;
   case 80: return 8;
   case 75: return 4;
   case 70: return 4;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_SO_BUFFER::Stream Offset */


#define GFX35_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX30_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX20_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX125_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX12_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX11_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX9_3DSTATE_SO_BUFFER_StreamOffset_bits  32
#define GFX8_3DSTATE_SO_BUFFER_StreamOffset_bits  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SO_BUFFER_StreamOffset_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX30_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX20_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX125_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX12_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX11_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX9_3DSTATE_SO_BUFFER_StreamOffset_start  224
#define GFX8_3DSTATE_SO_BUFFER_StreamOffset_start  224

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SO_BUFFER_StreamOffset_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 224;
   case 300: return 224;
   case 200: return 224;
   case 125: return 224;
   case 120: return 224;
   case 110: return 224;
   case 90: return 224;
   case 80: return 224;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_SO_BUFFER::Surface Base Address */


#define GFX35_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX30_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX20_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX125_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX12_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX11_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX9_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX8_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  46
#define GFX75_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  30
#define GFX7_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits  30

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SO_BUFFER_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 46;
   case 300: return 46;
   case 200: return 46;
   case 125: return 46;
   case 120: return 46;
   case 110: return 46;
   case 90: return 46;
   case 80: return 46;
   case 75: return 30;
   case 70: return 30;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX30_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX20_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX125_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX12_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX11_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX9_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX8_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX75_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66
#define GFX7_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start  66

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_SO_BUFFER_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 66;
   case 300: return 66;
   case 200: return 66;
   case 125: return 66;
   case 120: return 66;
   case 110: return 66;
   case 90: return 66;
   case 80: return 66;
   case 75: return 66;
   case 70: return 66;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_STENCIL_BUFFER */


#ifndef GFX35_3DSTATE_STENCIL_BUFFER_length
#define GFX35_3DSTATE_STENCIL_BUFFER_length  8
#endif
#ifndef GFX30_3DSTATE_STENCIL_BUFFER_length
#define GFX30_3DSTATE_STENCIL_BUFFER_length  8
#endif
#ifndef GFX20_3DSTATE_STENCIL_BUFFER_length
#define GFX20_3DSTATE_STENCIL_BUFFER_length  8
#endif
#ifndef GFX125_3DSTATE_STENCIL_BUFFER_length
#define GFX125_3DSTATE_STENCIL_BUFFER_length  8
#endif
#ifndef GFX12_3DSTATE_STENCIL_BUFFER_length
#define GFX12_3DSTATE_STENCIL_BUFFER_length  8
#endif
#ifndef GFX11_3DSTATE_STENCIL_BUFFER_length
#define GFX11_3DSTATE_STENCIL_BUFFER_length  5
#endif
#ifndef GFX9_3DSTATE_STENCIL_BUFFER_length
#define GFX9_3DSTATE_STENCIL_BUFFER_length  5
#endif
#ifndef GFX8_3DSTATE_STENCIL_BUFFER_length
#define GFX8_3DSTATE_STENCIL_BUFFER_length  5
#endif
#ifndef GFX75_3DSTATE_STENCIL_BUFFER_length
#define GFX75_3DSTATE_STENCIL_BUFFER_length  3
#endif
#ifndef GFX7_3DSTATE_STENCIL_BUFFER_length
#define GFX7_3DSTATE_STENCIL_BUFFER_length  3
#endif
#ifndef GFX6_3DSTATE_STENCIL_BUFFER_length
#define GFX6_3DSTATE_STENCIL_BUFFER_length  3
#endif
#ifndef GFX5_3DSTATE_STENCIL_BUFFER_length
#define GFX5_3DSTATE_STENCIL_BUFFER_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STENCIL_BUFFER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 8;
   case 120: return 8;
   case 110: return 5;
   case 90: return 5;
   case 80: return 5;
   case 75: return 3;
   case 70: return 3;
   case 60: return 3;
   case 50: return 3;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_STENCIL_BUFFER::Surface Base Address */


#define GFX35_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX30_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX20_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX125_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX12_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX11_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX9_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX8_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  64
#define GFX75_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  32
#define GFX7_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  32
#define GFX6_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  32
#define GFX5_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX30_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX20_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX125_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX12_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX11_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX9_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX8_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX75_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX7_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX6_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64
#define GFX5_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start  64

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STENCIL_BUFFER_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 64;
   case 70: return 64;
   case 60: return 64;
   case 50: return 64;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_STENCIL_BUFFER::Surface Pitch */


#define GFX35_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX30_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX20_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX125_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX12_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX11_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX9_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX8_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX75_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX7_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX6_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17
#define GFX5_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits  17

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STENCIL_BUFFER_SurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 17;
   case 300: return 17;
   case 200: return 17;
   case 125: return 17;
   case 120: return 17;
   case 110: return 17;
   case 90: return 17;
   case 80: return 17;
   case 75: return 17;
   case 70: return 17;
   case 60: return 17;
   case 50: return 17;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX30_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX20_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX125_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX12_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX11_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX9_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX8_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX75_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX7_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX6_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32
#define GFX5_3DSTATE_STENCIL_BUFFER_SurfacePitch_start  32

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STENCIL_BUFFER_SurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* 3DSTATE_STREAMOUT */


#ifndef GFX35_3DSTATE_STREAMOUT_length
#define GFX35_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX30_3DSTATE_STREAMOUT_length
#define GFX30_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX20_3DSTATE_STREAMOUT_length
#define GFX20_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX125_3DSTATE_STREAMOUT_length
#define GFX125_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX12_3DSTATE_STREAMOUT_length
#define GFX12_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX11_3DSTATE_STREAMOUT_length
#define GFX11_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX9_3DSTATE_STREAMOUT_length
#define GFX9_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX8_3DSTATE_STREAMOUT_length
#define GFX8_3DSTATE_STREAMOUT_length  5
#endif
#ifndef GFX75_3DSTATE_STREAMOUT_length
#define GFX75_3DSTATE_STREAMOUT_length  3
#endif
#ifndef GFX7_3DSTATE_STREAMOUT_length
#define GFX7_3DSTATE_STREAMOUT_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_STREAMOUT_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 5;
   case 120: return 5;
   case 110: return 5;
   case 90: return 5;
   case 80: return 5;
   case 75: return 3;
   case 70: return 3;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_TASK_CONTROL */


#ifndef GFX35_3DSTATE_TASK_CONTROL_length
#define GFX35_3DSTATE_TASK_CONTROL_length  3
#endif
#ifndef GFX30_3DSTATE_TASK_CONTROL_length
#define GFX30_3DSTATE_TASK_CONTROL_length  3
#endif
#ifndef GFX20_3DSTATE_TASK_CONTROL_length
#define GFX20_3DSTATE_TASK_CONTROL_length  3
#endif
#ifndef GFX125_3DSTATE_TASK_CONTROL_length
#define GFX125_3DSTATE_TASK_CONTROL_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_TASK_CONTROL_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_TASK_REDISTRIB */


#ifndef GFX35_3DSTATE_TASK_REDISTRIB_length
#define GFX35_3DSTATE_TASK_REDISTRIB_length  2
#endif
#ifndef GFX30_3DSTATE_TASK_REDISTRIB_length
#define GFX30_3DSTATE_TASK_REDISTRIB_length  2
#endif
#ifndef GFX20_3DSTATE_TASK_REDISTRIB_length
#define GFX20_3DSTATE_TASK_REDISTRIB_length  2
#endif
#ifndef GFX125_3DSTATE_TASK_REDISTRIB_length
#define GFX125_3DSTATE_TASK_REDISTRIB_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_TASK_REDISTRIB_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_TASK_SHADER */


#ifndef GFX35_3DSTATE_TASK_SHADER_length
#define GFX35_3DSTATE_TASK_SHADER_length  7
#endif
#ifndef GFX30_3DSTATE_TASK_SHADER_length
#define GFX30_3DSTATE_TASK_SHADER_length  7
#endif
#ifndef GFX20_3DSTATE_TASK_SHADER_length
#define GFX20_3DSTATE_TASK_SHADER_length  7
#endif
#ifndef GFX125_3DSTATE_TASK_SHADER_length
#define GFX125_3DSTATE_TASK_SHADER_length  7
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_TASK_SHADER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 7;
   case 300: return 7;
   case 200: return 7;
   case 125: return 7;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_TASK_SHADER_DATA */


#ifndef GFX35_3DSTATE_TASK_SHADER_DATA_length
#define GFX35_3DSTATE_TASK_SHADER_DATA_length  10
#endif
#ifndef GFX30_3DSTATE_TASK_SHADER_DATA_length
#define GFX30_3DSTATE_TASK_SHADER_DATA_length  10
#endif
#ifndef GFX20_3DSTATE_TASK_SHADER_DATA_length
#define GFX20_3DSTATE_TASK_SHADER_DATA_length  10
#endif
#ifndef GFX125_3DSTATE_TASK_SHADER_DATA_length
#define GFX125_3DSTATE_TASK_SHADER_DATA_length  10
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_TASK_SHADER_DATA_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 10;
   case 300: return 10;
   case 200: return 10;
   case 125: return 10;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_TE */


#ifndef GFX35_3DSTATE_TE_length
#define GFX35_3DSTATE_TE_length  5
#endif
#ifndef GFX30_3DSTATE_TE_length
#define GFX30_3DSTATE_TE_length  5
#endif
#ifndef GFX20_3DSTATE_TE_length
#define GFX20_3DSTATE_TE_length  5
#endif
#ifndef GFX125_3DSTATE_TE_length
#define GFX125_3DSTATE_TE_length  5
#endif
#ifndef GFX12_3DSTATE_TE_length
#define GFX12_3DSTATE_TE_length  5
#endif
#ifndef GFX11_3DSTATE_TE_length
#define GFX11_3DSTATE_TE_length  4
#endif
#ifndef GFX9_3DSTATE_TE_length
#define GFX9_3DSTATE_TE_length  4
#endif
#ifndef GFX8_3DSTATE_TE_length
#define GFX8_3DSTATE_TE_length  4
#endif
#ifndef GFX75_3DSTATE_TE_length
#define GFX75_3DSTATE_TE_length  4
#endif
#ifndef GFX7_3DSTATE_TE_length
#define GFX7_3DSTATE_TE_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_TE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 5;
   case 120: return 5;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_URB_ALLOC_MESH */


#ifndef GFX35_3DSTATE_URB_ALLOC_MESH_length
#define GFX35_3DSTATE_URB_ALLOC_MESH_length  3
#endif
#ifndef GFX30_3DSTATE_URB_ALLOC_MESH_length
#define GFX30_3DSTATE_URB_ALLOC_MESH_length  3
#endif
#ifndef GFX20_3DSTATE_URB_ALLOC_MESH_length
#define GFX20_3DSTATE_URB_ALLOC_MESH_length  3
#endif
#ifndef GFX125_3DSTATE_URB_ALLOC_MESH_length
#define GFX125_3DSTATE_URB_ALLOC_MESH_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_URB_ALLOC_MESH_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_URB_ALLOC_TASK */


#ifndef GFX35_3DSTATE_URB_ALLOC_TASK_length
#define GFX35_3DSTATE_URB_ALLOC_TASK_length  3
#endif
#ifndef GFX30_3DSTATE_URB_ALLOC_TASK_length
#define GFX30_3DSTATE_URB_ALLOC_TASK_length  3
#endif
#ifndef GFX20_3DSTATE_URB_ALLOC_TASK_length
#define GFX20_3DSTATE_URB_ALLOC_TASK_length  3
#endif
#ifndef GFX125_3DSTATE_URB_ALLOC_TASK_length
#define GFX125_3DSTATE_URB_ALLOC_TASK_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_URB_ALLOC_TASK_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_URB_ALLOC_VS */


#ifndef GFX35_3DSTATE_URB_ALLOC_VS_length
#define GFX35_3DSTATE_URB_ALLOC_VS_length  3
#endif
#ifndef GFX30_3DSTATE_URB_ALLOC_VS_length
#define GFX30_3DSTATE_URB_ALLOC_VS_length  3
#endif
#ifndef GFX20_3DSTATE_URB_ALLOC_VS_length
#define GFX20_3DSTATE_URB_ALLOC_VS_length  3
#endif
#ifndef GFX125_3DSTATE_URB_ALLOC_VS_length
#define GFX125_3DSTATE_URB_ALLOC_VS_length  3
#endif
#ifndef GFX12_3DSTATE_URB_ALLOC_VS_length
#define GFX12_3DSTATE_URB_ALLOC_VS_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_URB_ALLOC_VS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 3;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_URB_VS */


#ifndef GFX35_3DSTATE_URB_VS_length
#define GFX35_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX30_3DSTATE_URB_VS_length
#define GFX30_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX20_3DSTATE_URB_VS_length
#define GFX20_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX125_3DSTATE_URB_VS_length
#define GFX125_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX12_3DSTATE_URB_VS_length
#define GFX12_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX11_3DSTATE_URB_VS_length
#define GFX11_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX9_3DSTATE_URB_VS_length
#define GFX9_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX8_3DSTATE_URB_VS_length
#define GFX8_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX75_3DSTATE_URB_VS_length
#define GFX75_3DSTATE_URB_VS_length  2
#endif
#ifndef GFX7_3DSTATE_URB_VS_length
#define GFX7_3DSTATE_URB_VS_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_URB_VS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 2;
   case 70: return 2;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VFG */


#ifndef GFX35_3DSTATE_VFG_length
#define GFX35_3DSTATE_VFG_length  4
#endif
#ifndef GFX30_3DSTATE_VFG_length
#define GFX30_3DSTATE_VFG_length  4
#endif
#ifndef GFX20_3DSTATE_VFG_length
#define GFX20_3DSTATE_VFG_length  4
#endif
#ifndef GFX125_3DSTATE_VFG_length
#define GFX125_3DSTATE_VFG_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VFG_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VF_INSTANCING */


#ifndef GFX35_3DSTATE_VF_INSTANCING_length
#define GFX35_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX30_3DSTATE_VF_INSTANCING_length
#define GFX30_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX20_3DSTATE_VF_INSTANCING_length
#define GFX20_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX125_3DSTATE_VF_INSTANCING_length
#define GFX125_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX12_3DSTATE_VF_INSTANCING_length
#define GFX12_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX11_3DSTATE_VF_INSTANCING_length
#define GFX11_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX9_3DSTATE_VF_INSTANCING_length
#define GFX9_3DSTATE_VF_INSTANCING_length  3
#endif
#ifndef GFX8_3DSTATE_VF_INSTANCING_length
#define GFX8_3DSTATE_VF_INSTANCING_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VF_INSTANCING_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 3;
   case 110: return 3;
   case 90: return 3;
   case 80: return 3;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VF_SGVS */


#ifndef GFX35_3DSTATE_VF_SGVS_length
#define GFX35_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX30_3DSTATE_VF_SGVS_length
#define GFX30_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX20_3DSTATE_VF_SGVS_length
#define GFX20_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX125_3DSTATE_VF_SGVS_length
#define GFX125_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX12_3DSTATE_VF_SGVS_length
#define GFX12_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX11_3DSTATE_VF_SGVS_length
#define GFX11_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX9_3DSTATE_VF_SGVS_length
#define GFX9_3DSTATE_VF_SGVS_length  2
#endif
#ifndef GFX8_3DSTATE_VF_SGVS_length
#define GFX8_3DSTATE_VF_SGVS_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VF_SGVS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VF_SGVS_2 */


#ifndef GFX35_3DSTATE_VF_SGVS_2_length
#define GFX35_3DSTATE_VF_SGVS_2_length  3
#endif
#ifndef GFX30_3DSTATE_VF_SGVS_2_length
#define GFX30_3DSTATE_VF_SGVS_2_length  3
#endif
#ifndef GFX20_3DSTATE_VF_SGVS_2_length
#define GFX20_3DSTATE_VF_SGVS_2_length  3
#endif
#ifndef GFX125_3DSTATE_VF_SGVS_2_length
#define GFX125_3DSTATE_VF_SGVS_2_length  3
#endif
#ifndef GFX12_3DSTATE_VF_SGVS_2_length
#define GFX12_3DSTATE_VF_SGVS_2_length  3
#endif
#ifndef GFX11_3DSTATE_VF_SGVS_2_length
#define GFX11_3DSTATE_VF_SGVS_2_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VF_SGVS_2_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 3;
   case 110: return 3;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VF_TOPOLOGY */


#ifndef GFX35_3DSTATE_VF_TOPOLOGY_length
#define GFX35_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX30_3DSTATE_VF_TOPOLOGY_length
#define GFX30_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX20_3DSTATE_VF_TOPOLOGY_length
#define GFX20_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX125_3DSTATE_VF_TOPOLOGY_length
#define GFX125_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX12_3DSTATE_VF_TOPOLOGY_length
#define GFX12_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX11_3DSTATE_VF_TOPOLOGY_length
#define GFX11_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX9_3DSTATE_VF_TOPOLOGY_length
#define GFX9_3DSTATE_VF_TOPOLOGY_length  2
#endif
#ifndef GFX8_3DSTATE_VF_TOPOLOGY_length
#define GFX8_3DSTATE_VF_TOPOLOGY_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VF_TOPOLOGY_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_VS */


#ifndef GFX35_3DSTATE_VS_length
#define GFX35_3DSTATE_VS_length  9
#endif
#ifndef GFX30_3DSTATE_VS_length
#define GFX30_3DSTATE_VS_length  9
#endif
#ifndef GFX20_3DSTATE_VS_length
#define GFX20_3DSTATE_VS_length  9
#endif
#ifndef GFX125_3DSTATE_VS_length
#define GFX125_3DSTATE_VS_length  9
#endif
#ifndef GFX12_3DSTATE_VS_length
#define GFX12_3DSTATE_VS_length  9
#endif
#ifndef GFX11_3DSTATE_VS_length
#define GFX11_3DSTATE_VS_length  9
#endif
#ifndef GFX9_3DSTATE_VS_length
#define GFX9_3DSTATE_VS_length  9
#endif
#ifndef GFX8_3DSTATE_VS_length
#define GFX8_3DSTATE_VS_length  9
#endif
#ifndef GFX75_3DSTATE_VS_length
#define GFX75_3DSTATE_VS_length  6
#endif
#ifndef GFX7_3DSTATE_VS_length
#define GFX7_3DSTATE_VS_length  6
#endif
#ifndef GFX6_3DSTATE_VS_length
#define GFX6_3DSTATE_VS_length  6
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_VS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 9;
   case 300: return 9;
   case 200: return 9;
   case 125: return 9;
   case 120: return 9;
   case 110: return 9;
   case 90: return 9;
   case 80: return 9;
   case 75: return 6;
   case 70: return 6;
   case 60: return 6;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* 3DSTATE_WM */


#ifndef GFX35_3DSTATE_WM_length
#define GFX35_3DSTATE_WM_length  2
#endif
#ifndef GFX30_3DSTATE_WM_length
#define GFX30_3DSTATE_WM_length  2
#endif
#ifndef GFX20_3DSTATE_WM_length
#define GFX20_3DSTATE_WM_length  2
#endif
#ifndef GFX125_3DSTATE_WM_length
#define GFX125_3DSTATE_WM_length  2
#endif
#ifndef GFX12_3DSTATE_WM_length
#define GFX12_3DSTATE_WM_length  2
#endif
#ifndef GFX11_3DSTATE_WM_length
#define GFX11_3DSTATE_WM_length  2
#endif
#ifndef GFX9_3DSTATE_WM_length
#define GFX9_3DSTATE_WM_length  2
#endif
#ifndef GFX8_3DSTATE_WM_length
#define GFX8_3DSTATE_WM_length  2
#endif
#ifndef GFX75_3DSTATE_WM_length
#define GFX75_3DSTATE_WM_length  3
#endif
#ifndef GFX7_3DSTATE_WM_length
#define GFX7_3DSTATE_WM_length  3
#endif
#ifndef GFX6_3DSTATE_WM_length
#define GFX6_3DSTATE_WM_length  9
#endif

static inline uint32_t ATTRIBUTE_PURE
_3DSTATE_WM_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 3;
   case 70: return 3;
   case 60: return 9;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* CLEAR_COLOR */


#ifndef GFX125_CLEAR_COLOR_length
#define GFX125_CLEAR_COLOR_length  8
#endif
#ifndef GFX12_CLEAR_COLOR_length
#define GFX12_CLEAR_COLOR_length  8
#endif
#ifndef GFX11_CLEAR_COLOR_length
#define GFX11_CLEAR_COLOR_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
CLEAR_COLOR_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 8;
   case 120: return 8;
   case 110: return 8;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* COMPUTE_WALKER */


#ifndef GFX35_COMPUTE_WALKER_length
#define GFX35_COMPUTE_WALKER_length  40
#endif
#ifndef GFX30_COMPUTE_WALKER_length
#define GFX30_COMPUTE_WALKER_length  40
#endif
#ifndef GFX20_COMPUTE_WALKER_length
#define GFX20_COMPUTE_WALKER_length  40
#endif
#ifndef GFX125_COMPUTE_WALKER_length
#define GFX125_COMPUTE_WALKER_length  39
#endif

static inline uint32_t ATTRIBUTE_PURE
COMPUTE_WALKER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 40;
   case 300: return 40;
   case 200: return 40;
   case 125: return 39;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* COMPUTE_WALKER::body */


#define GFX35_COMPUTE_WALKER_body_bits  1249
#define GFX30_COMPUTE_WALKER_body_bits  1249
#define GFX20_COMPUTE_WALKER_body_bits  1249
#define GFX125_COMPUTE_WALKER_body_bits  1217

static inline uint32_t ATTRIBUTE_PURE
COMPUTE_WALKER_body_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 1249;
   case 300: return 1249;
   case 200: return 1249;
   case 125: return 1217;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_COMPUTE_WALKER_body_start  32
#define GFX30_COMPUTE_WALKER_body_start  32
#define GFX20_COMPUTE_WALKER_body_start  32
#define GFX125_COMPUTE_WALKER_body_start  32

static inline uint32_t ATTRIBUTE_PURE
COMPUTE_WALKER_body_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* CPS_STATE */


#ifndef GFX20_CPS_STATE_length
#define GFX20_CPS_STATE_length  8
#endif
#ifndef GFX125_CPS_STATE_length
#define GFX125_CPS_STATE_length  8
#endif
#ifndef GFX12_CPS_STATE_length
#define GFX12_CPS_STATE_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
CPS_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 8;
   case 125: return 8;
   case 120: return 8;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* EXECUTE_INDIRECT_DISPATCH */


#ifndef GFX35_EXECUTE_INDIRECT_DISPATCH_length
#define GFX35_EXECUTE_INDIRECT_DISPATCH_length  45
#endif
#ifndef GFX30_EXECUTE_INDIRECT_DISPATCH_length
#define GFX30_EXECUTE_INDIRECT_DISPATCH_length  45
#endif
#ifndef GFX20_EXECUTE_INDIRECT_DISPATCH_length
#define GFX20_EXECUTE_INDIRECT_DISPATCH_length  45
#endif
#ifndef GFX125_EXECUTE_INDIRECT_DISPATCH_length
#define GFX125_EXECUTE_INDIRECT_DISPATCH_length  44
#endif

static inline uint32_t ATTRIBUTE_PURE
EXECUTE_INDIRECT_DISPATCH_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 45;
   case 300: return 45;
   case 200: return 45;
   case 125: return 44;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* EXECUTE_INDIRECT_DISPATCH::body */


#define GFX35_EXECUTE_INDIRECT_DISPATCH_body_bits  1248
#define GFX30_EXECUTE_INDIRECT_DISPATCH_body_bits  1248
#define GFX20_EXECUTE_INDIRECT_DISPATCH_body_bits  1248
#define GFX125_EXECUTE_INDIRECT_DISPATCH_body_bits  1216

static inline uint32_t ATTRIBUTE_PURE
EXECUTE_INDIRECT_DISPATCH_body_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 1248;
   case 300: return 1248;
   case 200: return 1248;
   case 125: return 1216;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_EXECUTE_INDIRECT_DISPATCH_body_start  192
#define GFX30_EXECUTE_INDIRECT_DISPATCH_body_start  192
#define GFX20_EXECUTE_INDIRECT_DISPATCH_body_start  192
#define GFX125_EXECUTE_INDIRECT_DISPATCH_body_start  192

static inline uint32_t ATTRIBUTE_PURE
EXECUTE_INDIRECT_DISPATCH_body_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 192;
   case 300: return 192;
   case 200: return 192;
   case 125: return 192;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* GPGPU_WALKER */


#ifndef GFX12_GPGPU_WALKER_length
#define GFX12_GPGPU_WALKER_length  15
#endif
#ifndef GFX11_GPGPU_WALKER_length
#define GFX11_GPGPU_WALKER_length  15
#endif
#ifndef GFX9_GPGPU_WALKER_length
#define GFX9_GPGPU_WALKER_length  15
#endif
#ifndef GFX8_GPGPU_WALKER_length
#define GFX8_GPGPU_WALKER_length  15
#endif
#ifndef GFX75_GPGPU_WALKER_length
#define GFX75_GPGPU_WALKER_length  11
#endif
#ifndef GFX7_GPGPU_WALKER_length
#define GFX7_GPGPU_WALKER_length  11
#endif

static inline uint32_t ATTRIBUTE_PURE
GPGPU_WALKER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 15;
   case 110: return 15;
   case 90: return 15;
   case 80: return 15;
   case 75: return 11;
   case 70: return 11;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* INTERFACE_DESCRIPTOR_DATA */


#ifndef GFX35_INTERFACE_DESCRIPTOR_DATA_length
#define GFX35_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX30_INTERFACE_DESCRIPTOR_DATA_length
#define GFX30_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX20_INTERFACE_DESCRIPTOR_DATA_length
#define GFX20_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX125_INTERFACE_DESCRIPTOR_DATA_length
#define GFX125_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX12_INTERFACE_DESCRIPTOR_DATA_length
#define GFX12_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX11_INTERFACE_DESCRIPTOR_DATA_length
#define GFX11_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX9_INTERFACE_DESCRIPTOR_DATA_length
#define GFX9_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX8_INTERFACE_DESCRIPTOR_DATA_length
#define GFX8_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX75_INTERFACE_DESCRIPTOR_DATA_length
#define GFX75_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX7_INTERFACE_DESCRIPTOR_DATA_length
#define GFX7_INTERFACE_DESCRIPTOR_DATA_length  8
#endif
#ifndef GFX6_INTERFACE_DESCRIPTOR_DATA_length
#define GFX6_INTERFACE_DESCRIPTOR_DATA_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
INTERFACE_DESCRIPTOR_DATA_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 8;
   case 300: return 8;
   case 200: return 8;
   case 125: return 8;
   case 120: return 8;
   case 110: return 8;
   case 90: return 8;
   case 80: return 8;
   case 75: return 8;
   case 70: return 8;
   case 60: return 8;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MEDIA_CURBE_LOAD */


#ifndef GFX12_MEDIA_CURBE_LOAD_length
#define GFX12_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX11_MEDIA_CURBE_LOAD_length
#define GFX11_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX9_MEDIA_CURBE_LOAD_length
#define GFX9_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX8_MEDIA_CURBE_LOAD_length
#define GFX8_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX75_MEDIA_CURBE_LOAD_length
#define GFX75_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX7_MEDIA_CURBE_LOAD_length
#define GFX7_MEDIA_CURBE_LOAD_length  4
#endif
#ifndef GFX6_MEDIA_CURBE_LOAD_length
#define GFX6_MEDIA_CURBE_LOAD_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
MEDIA_CURBE_LOAD_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MEDIA_INTERFACE_DESCRIPTOR_LOAD */


#ifndef GFX12_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX12_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX11_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX11_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX9_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX9_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX8_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX8_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX75_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX75_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX7_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX7_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif
#ifndef GFX6_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length
#define GFX6_MEDIA_INTERFACE_DESCRIPTOR_LOAD_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
MEDIA_INTERFACE_DESCRIPTOR_LOAD_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MEDIA_STATE_FLUSH */


#ifndef GFX12_MEDIA_STATE_FLUSH_length
#define GFX12_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX11_MEDIA_STATE_FLUSH_length
#define GFX11_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX9_MEDIA_STATE_FLUSH_length
#define GFX9_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX8_MEDIA_STATE_FLUSH_length
#define GFX8_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX75_MEDIA_STATE_FLUSH_length
#define GFX75_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX7_MEDIA_STATE_FLUSH_length
#define GFX7_MEDIA_STATE_FLUSH_length  2
#endif
#ifndef GFX6_MEDIA_STATE_FLUSH_length
#define GFX6_MEDIA_STATE_FLUSH_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
MEDIA_STATE_FLUSH_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 2;
   case 70: return 2;
   case 60: return 2;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MEDIA_VFE_STATE */


#ifndef GFX12_MEDIA_VFE_STATE_length
#define GFX12_MEDIA_VFE_STATE_length  9
#endif
#ifndef GFX11_MEDIA_VFE_STATE_length
#define GFX11_MEDIA_VFE_STATE_length  9
#endif
#ifndef GFX9_MEDIA_VFE_STATE_length
#define GFX9_MEDIA_VFE_STATE_length  9
#endif
#ifndef GFX8_MEDIA_VFE_STATE_length
#define GFX8_MEDIA_VFE_STATE_length  9
#endif
#ifndef GFX75_MEDIA_VFE_STATE_length
#define GFX75_MEDIA_VFE_STATE_length  8
#endif
#ifndef GFX7_MEDIA_VFE_STATE_length
#define GFX7_MEDIA_VFE_STATE_length  8
#endif
#ifndef GFX6_MEDIA_VFE_STATE_length
#define GFX6_MEDIA_VFE_STATE_length  8
#endif

static inline uint32_t ATTRIBUTE_PURE
MEDIA_VFE_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 9;
   case 110: return 9;
   case 90: return 9;
   case 80: return 9;
   case 75: return 8;
   case 70: return 8;
   case 60: return 8;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_ARB_CHECK */


#ifndef GFX35_MI_ARB_CHECK_length
#define GFX35_MI_ARB_CHECK_length  1
#endif
#ifndef GFX30_MI_ARB_CHECK_length
#define GFX30_MI_ARB_CHECK_length  1
#endif
#ifndef GFX20_MI_ARB_CHECK_length
#define GFX20_MI_ARB_CHECK_length  1
#endif
#ifndef GFX125_MI_ARB_CHECK_length
#define GFX125_MI_ARB_CHECK_length  1
#endif
#ifndef GFX12_MI_ARB_CHECK_length
#define GFX12_MI_ARB_CHECK_length  1
#endif
#ifndef GFX11_MI_ARB_CHECK_length
#define GFX11_MI_ARB_CHECK_length  1
#endif
#ifndef GFX9_MI_ARB_CHECK_length
#define GFX9_MI_ARB_CHECK_length  1
#endif
#ifndef GFX8_MI_ARB_CHECK_length
#define GFX8_MI_ARB_CHECK_length  1
#endif
#ifndef GFX75_MI_ARB_CHECK_length
#define GFX75_MI_ARB_CHECK_length  1
#endif
#ifndef GFX7_MI_ARB_CHECK_length
#define GFX7_MI_ARB_CHECK_length  1
#endif
#ifndef GFX6_MI_ARB_CHECK_length
#define GFX6_MI_ARB_CHECK_length  1
#endif

static inline uint32_t ATTRIBUTE_PURE
MI_ARB_CHECK_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 1;
   case 300: return 1;
   case 200: return 1;
   case 125: return 1;
   case 120: return 1;
   case 110: return 1;
   case 90: return 1;
   case 80: return 1;
   case 75: return 1;
   case 70: return 1;
   case 60: return 1;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_BATCH_BUFFER_START */


#ifndef GFX35_MI_BATCH_BUFFER_START_length
#define GFX35_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX30_MI_BATCH_BUFFER_START_length
#define GFX30_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX20_MI_BATCH_BUFFER_START_length
#define GFX20_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX125_MI_BATCH_BUFFER_START_length
#define GFX125_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX12_MI_BATCH_BUFFER_START_length
#define GFX12_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX11_MI_BATCH_BUFFER_START_length
#define GFX11_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX9_MI_BATCH_BUFFER_START_length
#define GFX9_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX8_MI_BATCH_BUFFER_START_length
#define GFX8_MI_BATCH_BUFFER_START_length  3
#endif
#ifndef GFX75_MI_BATCH_BUFFER_START_length
#define GFX75_MI_BATCH_BUFFER_START_length  2
#endif
#ifndef GFX7_MI_BATCH_BUFFER_START_length
#define GFX7_MI_BATCH_BUFFER_START_length  2
#endif
#ifndef GFX6_MI_BATCH_BUFFER_START_length
#define GFX6_MI_BATCH_BUFFER_START_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
MI_BATCH_BUFFER_START_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 3;
   case 110: return 3;
   case 90: return 3;
   case 80: return 3;
   case 75: return 2;
   case 70: return 2;
   case 60: return 2;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_BATCH_BUFFER_START::Batch Buffer Start Address */


#define GFX35_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX30_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX20_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX125_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX12_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX11_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX9_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  62
#define GFX8_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  46
#define GFX75_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  30
#define GFX7_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  30
#define GFX6_MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits  30

static inline uint32_t ATTRIBUTE_PURE
MI_BATCH_BUFFER_START_BatchBufferStartAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 62;
   case 300: return 62;
   case 200: return 62;
   case 125: return 62;
   case 120: return 62;
   case 110: return 62;
   case 90: return 62;
   case 80: return 46;
   case 75: return 30;
   case 70: return 30;
   case 60: return 30;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX30_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX20_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX125_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX12_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX11_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX9_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX8_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX75_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX7_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34
#define GFX6_MI_BATCH_BUFFER_START_BatchBufferStartAddress_start  34

static inline uint32_t ATTRIBUTE_PURE
MI_BATCH_BUFFER_START_BatchBufferStartAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 34;
   case 300: return 34;
   case 200: return 34;
   case 125: return 34;
   case 120: return 34;
   case 110: return 34;
   case 90: return 34;
   case 80: return 34;
   case 75: return 34;
   case 70: return 34;
   case 60: return 34;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* MI_REPORT_PERF_COUNT */


#ifndef GFX35_MI_REPORT_PERF_COUNT_length
#define GFX35_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX30_MI_REPORT_PERF_COUNT_length
#define GFX30_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX20_MI_REPORT_PERF_COUNT_length
#define GFX20_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX125_MI_REPORT_PERF_COUNT_length
#define GFX125_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX12_MI_REPORT_PERF_COUNT_length
#define GFX12_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX11_MI_REPORT_PERF_COUNT_length
#define GFX11_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX9_MI_REPORT_PERF_COUNT_length
#define GFX9_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX8_MI_REPORT_PERF_COUNT_length
#define GFX8_MI_REPORT_PERF_COUNT_length  4
#endif
#ifndef GFX75_MI_REPORT_PERF_COUNT_length
#define GFX75_MI_REPORT_PERF_COUNT_length  3
#endif
#ifndef GFX7_MI_REPORT_PERF_COUNT_length
#define GFX7_MI_REPORT_PERF_COUNT_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
MI_REPORT_PERF_COUNT_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 3;
   case 70: return 3;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_REPORT_PERF_COUNT::Memory Address */


#define GFX35_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX30_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX20_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX125_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX12_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX11_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX9_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX8_MI_REPORT_PERF_COUNT_MemoryAddress_bits  58
#define GFX75_MI_REPORT_PERF_COUNT_MemoryAddress_bits  26
#define GFX7_MI_REPORT_PERF_COUNT_MemoryAddress_bits  26

static inline uint32_t ATTRIBUTE_PURE
MI_REPORT_PERF_COUNT_MemoryAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 58;
   case 300: return 58;
   case 200: return 58;
   case 125: return 58;
   case 120: return 58;
   case 110: return 58;
   case 90: return 58;
   case 80: return 58;
   case 75: return 26;
   case 70: return 26;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX30_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX20_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX125_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX12_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX11_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX9_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX8_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX75_MI_REPORT_PERF_COUNT_MemoryAddress_start  38
#define GFX7_MI_REPORT_PERF_COUNT_MemoryAddress_start  38

static inline uint32_t ATTRIBUTE_PURE
MI_REPORT_PERF_COUNT_MemoryAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 38;
   case 300: return 38;
   case 200: return 38;
   case 125: return 38;
   case 120: return 38;
   case 110: return 38;
   case 90: return 38;
   case 80: return 38;
   case 75: return 38;
   case 70: return 38;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* MI_STORE_DATA_IMM */


#ifndef GFX35_MI_STORE_DATA_IMM_length
#define GFX35_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX30_MI_STORE_DATA_IMM_length
#define GFX30_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX20_MI_STORE_DATA_IMM_length
#define GFX20_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX125_MI_STORE_DATA_IMM_length
#define GFX125_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX12_MI_STORE_DATA_IMM_length
#define GFX12_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX11_MI_STORE_DATA_IMM_length
#define GFX11_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX9_MI_STORE_DATA_IMM_length
#define GFX9_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX8_MI_STORE_DATA_IMM_length
#define GFX8_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX75_MI_STORE_DATA_IMM_length
#define GFX75_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX7_MI_STORE_DATA_IMM_length
#define GFX7_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX6_MI_STORE_DATA_IMM_length
#define GFX6_MI_STORE_DATA_IMM_length  4
#endif
#ifndef GFX5_MI_STORE_DATA_IMM_length
#define GFX5_MI_STORE_DATA_IMM_length  5
#endif
#ifndef GFX45_MI_STORE_DATA_IMM_length
#define GFX45_MI_STORE_DATA_IMM_length  5
#endif
#ifndef GFX4_MI_STORE_DATA_IMM_length
#define GFX4_MI_STORE_DATA_IMM_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_DATA_IMM_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 5;
   case 45: return 5;
   case 40: return 5;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_STORE_DATA_IMM::Address */


#define GFX35_MI_STORE_DATA_IMM_Address_bits  46
#define GFX30_MI_STORE_DATA_IMM_Address_bits  46
#define GFX20_MI_STORE_DATA_IMM_Address_bits  46
#define GFX125_MI_STORE_DATA_IMM_Address_bits  46
#define GFX12_MI_STORE_DATA_IMM_Address_bits  46
#define GFX11_MI_STORE_DATA_IMM_Address_bits  46
#define GFX9_MI_STORE_DATA_IMM_Address_bits  46
#define GFX8_MI_STORE_DATA_IMM_Address_bits  46
#define GFX75_MI_STORE_DATA_IMM_Address_bits  30
#define GFX7_MI_STORE_DATA_IMM_Address_bits  30
#define GFX6_MI_STORE_DATA_IMM_Address_bits  30
#define GFX5_MI_STORE_DATA_IMM_Address_bits  30
#define GFX45_MI_STORE_DATA_IMM_Address_bits  30
#define GFX4_MI_STORE_DATA_IMM_Address_bits  30

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_DATA_IMM_Address_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 46;
   case 300: return 46;
   case 200: return 46;
   case 125: return 46;
   case 120: return 46;
   case 110: return 46;
   case 90: return 46;
   case 80: return 46;
   case 75: return 30;
   case 70: return 30;
   case 60: return 30;
   case 50: return 30;
   case 45: return 30;
   case 40: return 30;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_MI_STORE_DATA_IMM_Address_start  34
#define GFX30_MI_STORE_DATA_IMM_Address_start  34
#define GFX20_MI_STORE_DATA_IMM_Address_start  34
#define GFX125_MI_STORE_DATA_IMM_Address_start  34
#define GFX12_MI_STORE_DATA_IMM_Address_start  34
#define GFX11_MI_STORE_DATA_IMM_Address_start  34
#define GFX9_MI_STORE_DATA_IMM_Address_start  34
#define GFX8_MI_STORE_DATA_IMM_Address_start  34
#define GFX75_MI_STORE_DATA_IMM_Address_start  66
#define GFX7_MI_STORE_DATA_IMM_Address_start  66
#define GFX6_MI_STORE_DATA_IMM_Address_start  66
#define GFX5_MI_STORE_DATA_IMM_Address_start  66
#define GFX45_MI_STORE_DATA_IMM_Address_start  66
#define GFX4_MI_STORE_DATA_IMM_Address_start  66

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_DATA_IMM_Address_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 34;
   case 300: return 34;
   case 200: return 34;
   case 125: return 34;
   case 120: return 34;
   case 110: return 34;
   case 90: return 34;
   case 80: return 34;
   case 75: return 66;
   case 70: return 66;
   case 60: return 66;
   case 50: return 66;
   case 45: return 66;
   case 40: return 66;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* MI_STORE_DATA_IMM::Immediate Data */


#define GFX35_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX30_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX20_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX125_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX12_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX11_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX9_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX8_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX75_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX7_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX6_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX5_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX45_MI_STORE_DATA_IMM_ImmediateData_bits  64
#define GFX4_MI_STORE_DATA_IMM_ImmediateData_bits  64

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_DATA_IMM_ImmediateData_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 64;
   case 70: return 64;
   case 60: return 64;
   case 50: return 64;
   case 45: return 64;
   case 40: return 64;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX30_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX20_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX125_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX12_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX11_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX9_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX8_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX75_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX7_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX6_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX5_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX45_MI_STORE_DATA_IMM_ImmediateData_start  96
#define GFX4_MI_STORE_DATA_IMM_ImmediateData_start  96

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_DATA_IMM_ImmediateData_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 96;
   case 300: return 96;
   case 200: return 96;
   case 125: return 96;
   case 120: return 96;
   case 110: return 96;
   case 90: return 96;
   case 80: return 96;
   case 75: return 96;
   case 70: return 96;
   case 60: return 96;
   case 50: return 96;
   case 45: return 96;
   case 40: return 96;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* MI_STORE_REGISTER_MEM */


#ifndef GFX35_MI_STORE_REGISTER_MEM_length
#define GFX35_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX30_MI_STORE_REGISTER_MEM_length
#define GFX30_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX20_MI_STORE_REGISTER_MEM_length
#define GFX20_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX125_MI_STORE_REGISTER_MEM_length
#define GFX125_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX12_MI_STORE_REGISTER_MEM_length
#define GFX12_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX11_MI_STORE_REGISTER_MEM_length
#define GFX11_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX9_MI_STORE_REGISTER_MEM_length
#define GFX9_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX8_MI_STORE_REGISTER_MEM_length
#define GFX8_MI_STORE_REGISTER_MEM_length  4
#endif
#ifndef GFX75_MI_STORE_REGISTER_MEM_length
#define GFX75_MI_STORE_REGISTER_MEM_length  3
#endif
#ifndef GFX7_MI_STORE_REGISTER_MEM_length
#define GFX7_MI_STORE_REGISTER_MEM_length  3
#endif
#ifndef GFX6_MI_STORE_REGISTER_MEM_length
#define GFX6_MI_STORE_REGISTER_MEM_length  3
#endif
#ifndef GFX5_MI_STORE_REGISTER_MEM_length
#define GFX5_MI_STORE_REGISTER_MEM_length  3
#endif
#ifndef GFX45_MI_STORE_REGISTER_MEM_length
#define GFX45_MI_STORE_REGISTER_MEM_length  3
#endif
#ifndef GFX4_MI_STORE_REGISTER_MEM_length
#define GFX4_MI_STORE_REGISTER_MEM_length  3
#endif

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_REGISTER_MEM_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 3;
   case 70: return 3;
   case 60: return 3;
   case 50: return 3;
   case 45: return 3;
   case 40: return 3;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* MI_STORE_REGISTER_MEM::Memory Address */


#define GFX35_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX30_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX20_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX125_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX12_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX11_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX9_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX8_MI_STORE_REGISTER_MEM_MemoryAddress_bits  62
#define GFX75_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30
#define GFX7_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30
#define GFX6_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30
#define GFX5_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30
#define GFX45_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30
#define GFX4_MI_STORE_REGISTER_MEM_MemoryAddress_bits  30

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_REGISTER_MEM_MemoryAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 62;
   case 300: return 62;
   case 200: return 62;
   case 125: return 62;
   case 120: return 62;
   case 110: return 62;
   case 90: return 62;
   case 80: return 62;
   case 75: return 30;
   case 70: return 30;
   case 60: return 30;
   case 50: return 30;
   case 45: return 30;
   case 40: return 30;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX30_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX20_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX125_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX12_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX11_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX9_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX8_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX75_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX7_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX6_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX5_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX45_MI_STORE_REGISTER_MEM_MemoryAddress_start  66
#define GFX4_MI_STORE_REGISTER_MEM_MemoryAddress_start  66

static inline uint32_t ATTRIBUTE_PURE
MI_STORE_REGISTER_MEM_MemoryAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 66;
   case 300: return 66;
   case 200: return 66;
   case 125: return 66;
   case 120: return 66;
   case 110: return 66;
   case 90: return 66;
   case 80: return 66;
   case 75: return 66;
   case 70: return 66;
   case 60: return 66;
   case 50: return 66;
   case 45: return 66;
   case 40: return 66;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* PIPE_CONTROL */


#ifndef GFX35_PIPE_CONTROL_length
#define GFX35_PIPE_CONTROL_length  6
#endif
#ifndef GFX30_PIPE_CONTROL_length
#define GFX30_PIPE_CONTROL_length  6
#endif
#ifndef GFX20_PIPE_CONTROL_length
#define GFX20_PIPE_CONTROL_length  6
#endif
#ifndef GFX125_PIPE_CONTROL_length
#define GFX125_PIPE_CONTROL_length  6
#endif
#ifndef GFX12_PIPE_CONTROL_length
#define GFX12_PIPE_CONTROL_length  6
#endif
#ifndef GFX11_PIPE_CONTROL_length
#define GFX11_PIPE_CONTROL_length  6
#endif
#ifndef GFX9_PIPE_CONTROL_length
#define GFX9_PIPE_CONTROL_length  6
#endif
#ifndef GFX8_PIPE_CONTROL_length
#define GFX8_PIPE_CONTROL_length  6
#endif
#ifndef GFX75_PIPE_CONTROL_length
#define GFX75_PIPE_CONTROL_length  5
#endif
#ifndef GFX7_PIPE_CONTROL_length
#define GFX7_PIPE_CONTROL_length  5
#endif
#ifndef GFX6_PIPE_CONTROL_length
#define GFX6_PIPE_CONTROL_length  5
#endif
#ifndef GFX5_PIPE_CONTROL_length
#define GFX5_PIPE_CONTROL_length  4
#endif
#ifndef GFX45_PIPE_CONTROL_length
#define GFX45_PIPE_CONTROL_length  4
#endif
#ifndef GFX4_PIPE_CONTROL_length
#define GFX4_PIPE_CONTROL_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
PIPE_CONTROL_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 6;
   case 300: return 6;
   case 200: return 6;
   case 125: return 6;
   case 120: return 6;
   case 110: return 6;
   case 90: return 6;
   case 80: return 6;
   case 75: return 5;
   case 70: return 5;
   case 60: return 5;
   case 50: return 4;
   case 45: return 4;
   case 40: return 4;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* RENDER_SURFACE_STATE */


#ifndef GFX35_RENDER_SURFACE_STATE_length
#define GFX35_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX30_RENDER_SURFACE_STATE_length
#define GFX30_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX20_RENDER_SURFACE_STATE_length
#define GFX20_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX125_RENDER_SURFACE_STATE_length
#define GFX125_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX12_RENDER_SURFACE_STATE_length
#define GFX12_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX11_RENDER_SURFACE_STATE_length
#define GFX11_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX9_RENDER_SURFACE_STATE_length
#define GFX9_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX8_RENDER_SURFACE_STATE_length
#define GFX8_RENDER_SURFACE_STATE_length  16
#endif
#ifndef GFX75_RENDER_SURFACE_STATE_length
#define GFX75_RENDER_SURFACE_STATE_length  8
#endif
#ifndef GFX7_RENDER_SURFACE_STATE_length
#define GFX7_RENDER_SURFACE_STATE_length  8
#endif
#ifndef GFX6_RENDER_SURFACE_STATE_length
#define GFX6_RENDER_SURFACE_STATE_length  6
#endif
#ifndef GFX5_RENDER_SURFACE_STATE_length
#define GFX5_RENDER_SURFACE_STATE_length  6
#endif
#ifndef GFX45_RENDER_SURFACE_STATE_length
#define GFX45_RENDER_SURFACE_STATE_length  6
#endif
#ifndef GFX4_RENDER_SURFACE_STATE_length
#define GFX4_RENDER_SURFACE_STATE_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 16;
   case 300: return 16;
   case 200: return 16;
   case 125: return 16;
   case 120: return 16;
   case 110: return 16;
   case 90: return 16;
   case 80: return 16;
   case 75: return 8;
   case 70: return 8;
   case 60: return 6;
   case 50: return 6;
   case 45: return 6;
   case 40: return 5;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* RENDER_SURFACE_STATE::Alpha Clear Color */


#define GFX11_RENDER_SURFACE_STATE_AlphaClearColor_bits  32
#define GFX9_RENDER_SURFACE_STATE_AlphaClearColor_bits  32
#define GFX8_RENDER_SURFACE_STATE_AlphaClearColor_bits  1
#define GFX75_RENDER_SURFACE_STATE_AlphaClearColor_bits  1
#define GFX7_RENDER_SURFACE_STATE_AlphaClearColor_bits  1

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AlphaClearColor_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 32;
   case 90: return 32;
   case 80: return 1;
   case 75: return 1;
   case 70: return 1;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX11_RENDER_SURFACE_STATE_AlphaClearColor_start  480
#define GFX9_RENDER_SURFACE_STATE_AlphaClearColor_start  480
#define GFX8_RENDER_SURFACE_STATE_AlphaClearColor_start  252
#define GFX75_RENDER_SURFACE_STATE_AlphaClearColor_start  252
#define GFX7_RENDER_SURFACE_STATE_AlphaClearColor_start  252

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AlphaClearColor_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 480;
   case 90: return 480;
   case 80: return 252;
   case 75: return 252;
   case 70: return 252;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Auxiliary Surface Base Address */


#define GFX35_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX30_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX20_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX125_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX12_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX11_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX9_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX8_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  52
#define GFX75_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  20
#define GFX7_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits  20

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 52;
   case 300: return 52;
   case 200: return 52;
   case 125: return 52;
   case 120: return 52;
   case 110: return 52;
   case 90: return 52;
   case 80: return 52;
   case 75: return 20;
   case 70: return 20;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX30_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX20_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX125_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX12_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX11_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX9_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX8_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  332
#define GFX75_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  204
#define GFX7_RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start  204

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AuxiliarySurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 332;
   case 300: return 332;
   case 200: return 332;
   case 125: return 332;
   case 120: return 332;
   case 110: return 332;
   case 90: return 332;
   case 80: return 332;
   case 75: return 204;
   case 70: return 204;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Auxiliary Surface Pitch */


#define GFX35_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  10
#define GFX30_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  10
#define GFX20_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  10
#define GFX125_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  10
#define GFX12_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9
#define GFX11_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9
#define GFX9_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9
#define GFX8_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9
#define GFX75_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9
#define GFX7_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits  9

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AuxiliarySurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 10;
   case 300: return 10;
   case 200: return 10;
   case 125: return 10;
   case 120: return 9;
   case 110: return 9;
   case 90: return 9;
   case 80: return 9;
   case 75: return 9;
   case 70: return 9;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX30_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX20_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX125_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX12_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX11_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX9_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX8_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX75_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195
#define GFX7_RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start  195

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_AuxiliarySurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 195;
   case 300: return 195;
   case 200: return 195;
   case 125: return 195;
   case 120: return 195;
   case 110: return 195;
   case 90: return 195;
   case 80: return 195;
   case 75: return 195;
   case 70: return 195;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Blue Clear Color */


#define GFX11_RENDER_SURFACE_STATE_BlueClearColor_bits  32
#define GFX9_RENDER_SURFACE_STATE_BlueClearColor_bits  32
#define GFX8_RENDER_SURFACE_STATE_BlueClearColor_bits  1
#define GFX75_RENDER_SURFACE_STATE_BlueClearColor_bits  1
#define GFX7_RENDER_SURFACE_STATE_BlueClearColor_bits  1

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_BlueClearColor_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 32;
   case 90: return 32;
   case 80: return 1;
   case 75: return 1;
   case 70: return 1;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX11_RENDER_SURFACE_STATE_BlueClearColor_start  448
#define GFX9_RENDER_SURFACE_STATE_BlueClearColor_start  448
#define GFX8_RENDER_SURFACE_STATE_BlueClearColor_start  253
#define GFX75_RENDER_SURFACE_STATE_BlueClearColor_start  253
#define GFX7_RENDER_SURFACE_STATE_BlueClearColor_start  253

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_BlueClearColor_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 448;
   case 90: return 448;
   case 80: return 253;
   case 75: return 253;
   case 70: return 253;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Clear Value Address */


#define GFX125_RENDER_SURFACE_STATE_ClearValueAddress_bits  42
#define GFX12_RENDER_SURFACE_STATE_ClearValueAddress_bits  42
#define GFX11_RENDER_SURFACE_STATE_ClearValueAddress_bits  42

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_ClearValueAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 42;
   case 120: return 42;
   case 110: return 42;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX125_RENDER_SURFACE_STATE_ClearValueAddress_start  390
#define GFX12_RENDER_SURFACE_STATE_ClearValueAddress_start  390
#define GFX11_RENDER_SURFACE_STATE_ClearValueAddress_start  390

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_ClearValueAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 390;
   case 120: return 390;
   case 110: return 390;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Depth */


#define GFX35_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX30_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX20_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX125_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX12_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX11_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX9_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX8_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX75_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX7_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX6_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX5_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX45_RENDER_SURFACE_STATE_Depth_bits  11
#define GFX4_RENDER_SURFACE_STATE_Depth_bits  11

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Depth_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 11;
   case 75: return 11;
   case 70: return 11;
   case 60: return 11;
   case 50: return 11;
   case 45: return 11;
   case 40: return 11;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_Depth_start  117
#define GFX30_RENDER_SURFACE_STATE_Depth_start  117
#define GFX20_RENDER_SURFACE_STATE_Depth_start  117
#define GFX125_RENDER_SURFACE_STATE_Depth_start  117
#define GFX12_RENDER_SURFACE_STATE_Depth_start  117
#define GFX11_RENDER_SURFACE_STATE_Depth_start  117
#define GFX9_RENDER_SURFACE_STATE_Depth_start  117
#define GFX8_RENDER_SURFACE_STATE_Depth_start  117
#define GFX75_RENDER_SURFACE_STATE_Depth_start  117
#define GFX7_RENDER_SURFACE_STATE_Depth_start  117
#define GFX6_RENDER_SURFACE_STATE_Depth_start  117
#define GFX5_RENDER_SURFACE_STATE_Depth_start  117
#define GFX45_RENDER_SURFACE_STATE_Depth_start  117
#define GFX4_RENDER_SURFACE_STATE_Depth_start  117

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Depth_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 117;
   case 300: return 117;
   case 200: return 117;
   case 125: return 117;
   case 120: return 117;
   case 110: return 117;
   case 90: return 117;
   case 80: return 117;
   case 75: return 117;
   case 70: return 117;
   case 60: return 117;
   case 50: return 117;
   case 45: return 117;
   case 40: return 117;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Green Clear Color */


#define GFX11_RENDER_SURFACE_STATE_GreenClearColor_bits  32
#define GFX9_RENDER_SURFACE_STATE_GreenClearColor_bits  32
#define GFX8_RENDER_SURFACE_STATE_GreenClearColor_bits  1
#define GFX75_RENDER_SURFACE_STATE_GreenClearColor_bits  1
#define GFX7_RENDER_SURFACE_STATE_GreenClearColor_bits  1

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_GreenClearColor_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 32;
   case 90: return 32;
   case 80: return 1;
   case 75: return 1;
   case 70: return 1;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX11_RENDER_SURFACE_STATE_GreenClearColor_start  416
#define GFX9_RENDER_SURFACE_STATE_GreenClearColor_start  416
#define GFX8_RENDER_SURFACE_STATE_GreenClearColor_start  254
#define GFX75_RENDER_SURFACE_STATE_GreenClearColor_start  254
#define GFX7_RENDER_SURFACE_STATE_GreenClearColor_start  254

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_GreenClearColor_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 416;
   case 90: return 416;
   case 80: return 254;
   case 75: return 254;
   case 70: return 254;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Height */


#define GFX35_RENDER_SURFACE_STATE_Height_bits  14
#define GFX30_RENDER_SURFACE_STATE_Height_bits  14
#define GFX20_RENDER_SURFACE_STATE_Height_bits  14
#define GFX125_RENDER_SURFACE_STATE_Height_bits  14
#define GFX12_RENDER_SURFACE_STATE_Height_bits  14
#define GFX11_RENDER_SURFACE_STATE_Height_bits  14
#define GFX9_RENDER_SURFACE_STATE_Height_bits  14
#define GFX8_RENDER_SURFACE_STATE_Height_bits  14
#define GFX75_RENDER_SURFACE_STATE_Height_bits  14
#define GFX7_RENDER_SURFACE_STATE_Height_bits  14
#define GFX6_RENDER_SURFACE_STATE_Height_bits  13
#define GFX5_RENDER_SURFACE_STATE_Height_bits  13
#define GFX45_RENDER_SURFACE_STATE_Height_bits  13
#define GFX4_RENDER_SURFACE_STATE_Height_bits  13

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Height_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 14;
   case 300: return 14;
   case 200: return 14;
   case 125: return 14;
   case 120: return 14;
   case 110: return 14;
   case 90: return 14;
   case 80: return 14;
   case 75: return 14;
   case 70: return 14;
   case 60: return 13;
   case 50: return 13;
   case 45: return 13;
   case 40: return 13;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_Height_start  80
#define GFX30_RENDER_SURFACE_STATE_Height_start  80
#define GFX20_RENDER_SURFACE_STATE_Height_start  80
#define GFX125_RENDER_SURFACE_STATE_Height_start  80
#define GFX12_RENDER_SURFACE_STATE_Height_start  80
#define GFX11_RENDER_SURFACE_STATE_Height_start  80
#define GFX9_RENDER_SURFACE_STATE_Height_start  80
#define GFX8_RENDER_SURFACE_STATE_Height_start  80
#define GFX75_RENDER_SURFACE_STATE_Height_start  80
#define GFX7_RENDER_SURFACE_STATE_Height_start  80
#define GFX6_RENDER_SURFACE_STATE_Height_start  83
#define GFX5_RENDER_SURFACE_STATE_Height_start  83
#define GFX45_RENDER_SURFACE_STATE_Height_start  83
#define GFX4_RENDER_SURFACE_STATE_Height_start  83

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Height_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 80;
   case 300: return 80;
   case 200: return 80;
   case 125: return 80;
   case 120: return 80;
   case 110: return 80;
   case 90: return 80;
   case 80: return 80;
   case 75: return 80;
   case 70: return 80;
   case 60: return 83;
   case 50: return 83;
   case 45: return 83;
   case 40: return 83;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Minimum Array Element */


#define GFX35_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX30_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX20_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX125_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX12_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX11_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX9_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX8_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX75_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX7_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX6_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX5_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX45_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11
#define GFX4_RENDER_SURFACE_STATE_MinimumArrayElement_bits  11

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_MinimumArrayElement_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 11;
   case 75: return 11;
   case 70: return 11;
   case 60: return 11;
   case 50: return 11;
   case 45: return 11;
   case 40: return 11;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX30_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX20_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX125_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX12_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX11_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX9_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX8_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX75_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX7_RENDER_SURFACE_STATE_MinimumArrayElement_start  146
#define GFX6_RENDER_SURFACE_STATE_MinimumArrayElement_start  145
#define GFX5_RENDER_SURFACE_STATE_MinimumArrayElement_start  145
#define GFX45_RENDER_SURFACE_STATE_MinimumArrayElement_start  145
#define GFX4_RENDER_SURFACE_STATE_MinimumArrayElement_start  145

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_MinimumArrayElement_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 146;
   case 300: return 146;
   case 200: return 146;
   case 125: return 146;
   case 120: return 146;
   case 110: return 146;
   case 90: return 146;
   case 80: return 146;
   case 75: return 146;
   case 70: return 146;
   case 60: return 145;
   case 50: return 145;
   case 45: return 145;
   case 40: return 145;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Red Clear Color */


#define GFX11_RENDER_SURFACE_STATE_RedClearColor_bits  32
#define GFX9_RENDER_SURFACE_STATE_RedClearColor_bits  32
#define GFX8_RENDER_SURFACE_STATE_RedClearColor_bits  1
#define GFX75_RENDER_SURFACE_STATE_RedClearColor_bits  1
#define GFX7_RENDER_SURFACE_STATE_RedClearColor_bits  1

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_RedClearColor_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 32;
   case 90: return 32;
   case 80: return 1;
   case 75: return 1;
   case 70: return 1;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX11_RENDER_SURFACE_STATE_RedClearColor_start  384
#define GFX9_RENDER_SURFACE_STATE_RedClearColor_start  384
#define GFX8_RENDER_SURFACE_STATE_RedClearColor_start  255
#define GFX75_RENDER_SURFACE_STATE_RedClearColor_start  255
#define GFX7_RENDER_SURFACE_STATE_RedClearColor_start  255

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_RedClearColor_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 0;
   case 300: return 0;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 384;
   case 90: return 384;
   case 80: return 255;
   case 75: return 255;
   case 70: return 255;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Render Target View Extent */


#define GFX35_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX30_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX20_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX125_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX12_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX11_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX9_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX8_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX75_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX7_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  11
#define GFX6_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  9
#define GFX5_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  9
#define GFX45_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  9
#define GFX4_RENDER_SURFACE_STATE_RenderTargetViewExtent_bits  9

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_RenderTargetViewExtent_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 11;
   case 300: return 11;
   case 200: return 11;
   case 125: return 11;
   case 120: return 11;
   case 110: return 11;
   case 90: return 11;
   case 80: return 11;
   case 75: return 11;
   case 70: return 11;
   case 60: return 9;
   case 50: return 9;
   case 45: return 9;
   case 40: return 9;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX30_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX20_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX125_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX12_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX11_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX9_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX8_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX75_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX7_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  135
#define GFX6_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  136
#define GFX5_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  136
#define GFX45_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  136
#define GFX4_RENDER_SURFACE_STATE_RenderTargetViewExtent_start  136

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_RenderTargetViewExtent_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 135;
   case 300: return 135;
   case 200: return 135;
   case 125: return 135;
   case 120: return 135;
   case 110: return 135;
   case 90: return 135;
   case 80: return 135;
   case 75: return 135;
   case 70: return 135;
   case 60: return 136;
   case 50: return 136;
   case 45: return 136;
   case 40: return 136;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Surface Base Address */


#define GFX35_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX30_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX20_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX125_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX12_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX11_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX9_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX8_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  64
#define GFX75_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32
#define GFX7_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32
#define GFX6_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32
#define GFX5_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32
#define GFX45_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32
#define GFX4_RENDER_SURFACE_STATE_SurfaceBaseAddress_bits  32

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceBaseAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX30_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX20_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX125_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX12_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX11_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX9_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX8_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  256
#define GFX75_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32
#define GFX7_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32
#define GFX6_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32
#define GFX5_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32
#define GFX45_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32
#define GFX4_RENDER_SURFACE_STATE_SurfaceBaseAddress_start  32

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceBaseAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 256;
   case 300: return 256;
   case 200: return 256;
   case 125: return 256;
   case 120: return 256;
   case 110: return 256;
   case 90: return 256;
   case 80: return 256;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Surface Format */


#define GFX35_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX30_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX20_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX125_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX12_RENDER_SURFACE_STATE_SurfaceFormat_bits  10
#define GFX11_RENDER_SURFACE_STATE_SurfaceFormat_bits  10
#define GFX9_RENDER_SURFACE_STATE_SurfaceFormat_bits  10
#define GFX8_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX75_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX7_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX6_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX5_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX45_RENDER_SURFACE_STATE_SurfaceFormat_bits  9
#define GFX4_RENDER_SURFACE_STATE_SurfaceFormat_bits  9

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceFormat_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 9;
   case 300: return 9;
   case 200: return 9;
   case 125: return 9;
   case 120: return 10;
   case 110: return 10;
   case 90: return 10;
   case 80: return 9;
   case 75: return 9;
   case 70: return 9;
   case 60: return 9;
   case 50: return 9;
   case 45: return 9;
   case 40: return 9;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX30_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX20_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX125_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX12_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX11_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX9_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX8_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX75_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX7_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX6_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX5_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX45_RENDER_SURFACE_STATE_SurfaceFormat_start  18
#define GFX4_RENDER_SURFACE_STATE_SurfaceFormat_start  18

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceFormat_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 18;
   case 300: return 18;
   case 200: return 18;
   case 125: return 18;
   case 120: return 18;
   case 110: return 18;
   case 90: return 18;
   case 80: return 18;
   case 75: return 18;
   case 70: return 18;
   case 60: return 18;
   case 50: return 18;
   case 45: return 18;
   case 40: return 18;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Surface Pitch */


#define GFX35_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX30_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX20_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX125_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX12_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX11_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX9_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX8_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX75_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX7_RENDER_SURFACE_STATE_SurfacePitch_bits  18
#define GFX6_RENDER_SURFACE_STATE_SurfacePitch_bits  17
#define GFX5_RENDER_SURFACE_STATE_SurfacePitch_bits  17
#define GFX45_RENDER_SURFACE_STATE_SurfacePitch_bits  17
#define GFX4_RENDER_SURFACE_STATE_SurfacePitch_bits  17

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfacePitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 18;
   case 300: return 18;
   case 200: return 18;
   case 125: return 18;
   case 120: return 18;
   case 110: return 18;
   case 90: return 18;
   case 80: return 18;
   case 75: return 18;
   case 70: return 18;
   case 60: return 17;
   case 50: return 17;
   case 45: return 17;
   case 40: return 17;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX30_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX20_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX125_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX12_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX11_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX9_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX8_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX75_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX7_RENDER_SURFACE_STATE_SurfacePitch_start  96
#define GFX6_RENDER_SURFACE_STATE_SurfacePitch_start  99
#define GFX5_RENDER_SURFACE_STATE_SurfacePitch_start  99
#define GFX45_RENDER_SURFACE_STATE_SurfacePitch_start  99
#define GFX4_RENDER_SURFACE_STATE_SurfacePitch_start  99

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfacePitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 96;
   case 300: return 96;
   case 200: return 96;
   case 125: return 96;
   case 120: return 96;
   case 110: return 96;
   case 90: return 96;
   case 80: return 96;
   case 75: return 96;
   case 70: return 96;
   case 60: return 99;
   case 50: return 99;
   case 45: return 99;
   case 40: return 99;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Surface QPitch */


#define GFX35_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX30_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX20_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX125_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX12_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX11_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX9_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15
#define GFX8_RENDER_SURFACE_STATE_SurfaceQPitch_bits  15

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceQPitch_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 15;
   case 300: return 15;
   case 200: return 15;
   case 125: return 15;
   case 120: return 15;
   case 110: return 15;
   case 90: return 15;
   case 80: return 15;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX30_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX20_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX125_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX12_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX11_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX9_RENDER_SURFACE_STATE_SurfaceQPitch_start  32
#define GFX8_RENDER_SURFACE_STATE_SurfaceQPitch_start  32

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceQPitch_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Surface Type */


#define GFX35_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX30_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX20_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX125_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX12_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX11_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX9_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX8_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX75_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX7_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX6_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX5_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX45_RENDER_SURFACE_STATE_SurfaceType_bits  3
#define GFX4_RENDER_SURFACE_STATE_SurfaceType_bits  3

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceType_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 3;
   case 300: return 3;
   case 200: return 3;
   case 125: return 3;
   case 120: return 3;
   case 110: return 3;
   case 90: return 3;
   case 80: return 3;
   case 75: return 3;
   case 70: return 3;
   case 60: return 3;
   case 50: return 3;
   case 45: return 3;
   case 40: return 3;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX30_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX20_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX125_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX12_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX11_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX9_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX8_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX75_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX7_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX6_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX5_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX45_RENDER_SURFACE_STATE_SurfaceType_start  29
#define GFX4_RENDER_SURFACE_STATE_SurfaceType_start  29

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_SurfaceType_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 29;
   case 300: return 29;
   case 200: return 29;
   case 125: return 29;
   case 120: return 29;
   case 110: return 29;
   case 90: return 29;
   case 80: return 29;
   case 75: return 29;
   case 70: return 29;
   case 60: return 29;
   case 50: return 29;
   case 45: return 29;
   case 40: return 29;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Tile Mode */


#define GFX35_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX30_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX20_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX125_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX12_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX11_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX9_RENDER_SURFACE_STATE_TileMode_bits  2
#define GFX8_RENDER_SURFACE_STATE_TileMode_bits  2

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_TileMode_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX30_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX20_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX125_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX12_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX11_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX9_RENDER_SURFACE_STATE_TileMode_start  12
#define GFX8_RENDER_SURFACE_STATE_TileMode_start  12

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_TileMode_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 12;
   case 300: return 12;
   case 200: return 12;
   case 125: return 12;
   case 120: return 12;
   case 110: return 12;
   case 90: return 12;
   case 80: return 12;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RENDER_SURFACE_STATE::Width */


#define GFX35_RENDER_SURFACE_STATE_Width_bits  14
#define GFX30_RENDER_SURFACE_STATE_Width_bits  14
#define GFX20_RENDER_SURFACE_STATE_Width_bits  14
#define GFX125_RENDER_SURFACE_STATE_Width_bits  14
#define GFX12_RENDER_SURFACE_STATE_Width_bits  14
#define GFX11_RENDER_SURFACE_STATE_Width_bits  14
#define GFX9_RENDER_SURFACE_STATE_Width_bits  14
#define GFX8_RENDER_SURFACE_STATE_Width_bits  14
#define GFX75_RENDER_SURFACE_STATE_Width_bits  14
#define GFX7_RENDER_SURFACE_STATE_Width_bits  14
#define GFX6_RENDER_SURFACE_STATE_Width_bits  13
#define GFX5_RENDER_SURFACE_STATE_Width_bits  13
#define GFX45_RENDER_SURFACE_STATE_Width_bits  13
#define GFX4_RENDER_SURFACE_STATE_Width_bits  13

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Width_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 14;
   case 300: return 14;
   case 200: return 14;
   case 125: return 14;
   case 120: return 14;
   case 110: return 14;
   case 90: return 14;
   case 80: return 14;
   case 75: return 14;
   case 70: return 14;
   case 60: return 13;
   case 50: return 13;
   case 45: return 13;
   case 40: return 13;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RENDER_SURFACE_STATE_Width_start  64
#define GFX30_RENDER_SURFACE_STATE_Width_start  64
#define GFX20_RENDER_SURFACE_STATE_Width_start  64
#define GFX125_RENDER_SURFACE_STATE_Width_start  64
#define GFX12_RENDER_SURFACE_STATE_Width_start  64
#define GFX11_RENDER_SURFACE_STATE_Width_start  64
#define GFX9_RENDER_SURFACE_STATE_Width_start  64
#define GFX8_RENDER_SURFACE_STATE_Width_start  64
#define GFX75_RENDER_SURFACE_STATE_Width_start  64
#define GFX7_RENDER_SURFACE_STATE_Width_start  64
#define GFX6_RENDER_SURFACE_STATE_Width_start  70
#define GFX5_RENDER_SURFACE_STATE_Width_start  70
#define GFX45_RENDER_SURFACE_STATE_Width_start  70
#define GFX4_RENDER_SURFACE_STATE_Width_start  70

static inline uint32_t ATTRIBUTE_PURE
RENDER_SURFACE_STATE_Width_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 64;
   case 70: return 64;
   case 60: return 70;
   case 50: return 70;
   case 45: return 70;
   case 40: return 70;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RESOURCE_BARRIER */


#ifndef GFX35_RESOURCE_BARRIER_length
#define GFX35_RESOURCE_BARRIER_length  5
#endif
#ifndef GFX30_RESOURCE_BARRIER_length
#define GFX30_RESOURCE_BARRIER_length  5
#endif
#ifndef GFX20_RESOURCE_BARRIER_length
#define GFX20_RESOURCE_BARRIER_length  5
#endif

static inline uint32_t ATTRIBUTE_PURE
RESOURCE_BARRIER_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 5;
   case 300: return 5;
   case 200: return 5;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* RT_DISPATCH_GLOBALS */


#ifndef GFX35_RT_DISPATCH_GLOBALS_length
#define GFX35_RT_DISPATCH_GLOBALS_length  21
#endif
#ifndef GFX30_RT_DISPATCH_GLOBALS_length
#define GFX30_RT_DISPATCH_GLOBALS_length  21
#endif
#ifndef GFX20_RT_DISPATCH_GLOBALS_length
#define GFX20_RT_DISPATCH_GLOBALS_length  20
#endif
#ifndef GFX125_RT_DISPATCH_GLOBALS_length
#define GFX125_RT_DISPATCH_GLOBALS_length  20
#endif

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 21;
   case 300: return 21;
   case 200: return 20;
   case 125: return 20;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* RT_DISPATCH_GLOBALS::Callable Group Stride */


#define GFX35_RT_DISPATCH_GLOBALS_CallableGroupStride_bits  13
#define GFX30_RT_DISPATCH_GLOBALS_CallableGroupStride_bits  13

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_CallableGroupStride_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 13;
   case 300: return 13;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_CallableGroupStride_start  576
#define GFX30_RT_DISPATCH_GLOBALS_CallableGroupStride_start  576

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_CallableGroupStride_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 576;
   case 300: return 576;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Callable Group Table */


#define GFX35_RT_DISPATCH_GLOBALS_CallableGroupTable_bits  64
#define GFX30_RT_DISPATCH_GLOBALS_CallableGroupTable_bits  64
#define GFX20_RT_DISPATCH_GLOBALS_CallableGroupTable_bits  64
#define GFX125_RT_DISPATCH_GLOBALS_CallableGroupTable_bits  64

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_CallableGroupTable_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_CallableGroupTable_start  512
#define GFX30_RT_DISPATCH_GLOBALS_CallableGroupTable_start  512
#define GFX20_RT_DISPATCH_GLOBALS_CallableGroupTable_start  512
#define GFX125_RT_DISPATCH_GLOBALS_CallableGroupTable_start  512

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_CallableGroupTable_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 512;
   case 300: return 512;
   case 200: return 512;
   case 125: return 512;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Hit Group Stride */


#define GFX35_RT_DISPATCH_GLOBALS_HitGroupStride_bits  13
#define GFX30_RT_DISPATCH_GLOBALS_HitGroupStride_bits  13

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_HitGroupStride_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 13;
   case 300: return 13;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_HitGroupStride_start  195
#define GFX30_RT_DISPATCH_GLOBALS_HitGroupStride_start  195

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_HitGroupStride_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 195;
   case 300: return 195;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Hit Group Table */


#define GFX35_RT_DISPATCH_GLOBALS_HitGroupTable_bits  64
#define GFX30_RT_DISPATCH_GLOBALS_HitGroupTable_bits  64
#define GFX20_RT_DISPATCH_GLOBALS_HitGroupTable_bits  64
#define GFX125_RT_DISPATCH_GLOBALS_HitGroupTable_bits  64

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_HitGroupTable_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_HitGroupTable_start  256
#define GFX30_RT_DISPATCH_GLOBALS_HitGroupTable_start  256
#define GFX20_RT_DISPATCH_GLOBALS_HitGroupTable_start  256
#define GFX125_RT_DISPATCH_GLOBALS_HitGroupTable_start  256

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_HitGroupTable_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 256;
   case 300: return 256;
   case 200: return 256;
   case 125: return 256;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Launch Depth */


#define GFX35_RT_DISPATCH_GLOBALS_LaunchDepth_bits  32
#define GFX30_RT_DISPATCH_GLOBALS_LaunchDepth_bits  32
#define GFX20_RT_DISPATCH_GLOBALS_LaunchDepth_bits  32
#define GFX125_RT_DISPATCH_GLOBALS_LaunchDepth_bits  32

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchDepth_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_LaunchDepth_start  480
#define GFX30_RT_DISPATCH_GLOBALS_LaunchDepth_start  480
#define GFX20_RT_DISPATCH_GLOBALS_LaunchDepth_start  480
#define GFX125_RT_DISPATCH_GLOBALS_LaunchDepth_start  480

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchDepth_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 480;
   case 300: return 480;
   case 200: return 480;
   case 125: return 480;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Launch Height */


#define GFX35_RT_DISPATCH_GLOBALS_LaunchHeight_bits  32
#define GFX30_RT_DISPATCH_GLOBALS_LaunchHeight_bits  32
#define GFX20_RT_DISPATCH_GLOBALS_LaunchHeight_bits  32
#define GFX125_RT_DISPATCH_GLOBALS_LaunchHeight_bits  32

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchHeight_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_LaunchHeight_start  448
#define GFX30_RT_DISPATCH_GLOBALS_LaunchHeight_start  448
#define GFX20_RT_DISPATCH_GLOBALS_LaunchHeight_start  448
#define GFX125_RT_DISPATCH_GLOBALS_LaunchHeight_start  448

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchHeight_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 448;
   case 300: return 448;
   case 200: return 448;
   case 125: return 448;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Launch Width */


#define GFX35_RT_DISPATCH_GLOBALS_LaunchWidth_bits  32
#define GFX30_RT_DISPATCH_GLOBALS_LaunchWidth_bits  32
#define GFX20_RT_DISPATCH_GLOBALS_LaunchWidth_bits  32
#define GFX125_RT_DISPATCH_GLOBALS_LaunchWidth_bits  32

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchWidth_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_LaunchWidth_start  416
#define GFX30_RT_DISPATCH_GLOBALS_LaunchWidth_start  416
#define GFX20_RT_DISPATCH_GLOBALS_LaunchWidth_start  416
#define GFX125_RT_DISPATCH_GLOBALS_LaunchWidth_start  416

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_LaunchWidth_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 416;
   case 300: return 416;
   case 200: return 416;
   case 125: return 416;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Miss Group Stride */


#define GFX35_RT_DISPATCH_GLOBALS_MissGroupStride_bits  13
#define GFX30_RT_DISPATCH_GLOBALS_MissGroupStride_bits  13

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_MissGroupStride_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 13;
   case 300: return 13;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_MissGroupStride_start  208
#define GFX30_RT_DISPATCH_GLOBALS_MissGroupStride_start  208

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_MissGroupStride_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 208;
   case 300: return 208;
   case 200: return 0;
   case 125: return 0;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* RT_DISPATCH_GLOBALS::Miss Group Table */


#define GFX35_RT_DISPATCH_GLOBALS_MissGroupTable_bits  64
#define GFX30_RT_DISPATCH_GLOBALS_MissGroupTable_bits  64
#define GFX20_RT_DISPATCH_GLOBALS_MissGroupTable_bits  64
#define GFX125_RT_DISPATCH_GLOBALS_MissGroupTable_bits  64

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_MissGroupTable_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_RT_DISPATCH_GLOBALS_MissGroupTable_start  320
#define GFX30_RT_DISPATCH_GLOBALS_MissGroupTable_start  320
#define GFX20_RT_DISPATCH_GLOBALS_MissGroupTable_start  320
#define GFX125_RT_DISPATCH_GLOBALS_MissGroupTable_start  320

static inline uint32_t ATTRIBUTE_PURE
RT_DISPATCH_GLOBALS_MissGroupTable_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 320;
   case 300: return 320;
   case 200: return 320;
   case 125: return 320;
   case 120: return 0;
   case 110: return 0;
   case 90: return 0;
   case 80: return 0;
   case 75: return 0;
   case 70: return 0;
   case 60: return 0;
   case 50: return 0;
   case 45: return 0;
   case 40: return 0;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* SAMPLER_STATE */


#ifndef GFX35_SAMPLER_STATE_length
#define GFX35_SAMPLER_STATE_length  4
#endif
#ifndef GFX30_SAMPLER_STATE_length
#define GFX30_SAMPLER_STATE_length  4
#endif
#ifndef GFX20_SAMPLER_STATE_length
#define GFX20_SAMPLER_STATE_length  4
#endif
#ifndef GFX125_SAMPLER_STATE_length
#define GFX125_SAMPLER_STATE_length  4
#endif
#ifndef GFX12_SAMPLER_STATE_length
#define GFX12_SAMPLER_STATE_length  4
#endif
#ifndef GFX11_SAMPLER_STATE_length
#define GFX11_SAMPLER_STATE_length  4
#endif
#ifndef GFX9_SAMPLER_STATE_length
#define GFX9_SAMPLER_STATE_length  4
#endif
#ifndef GFX8_SAMPLER_STATE_length
#define GFX8_SAMPLER_STATE_length  4
#endif
#ifndef GFX75_SAMPLER_STATE_length
#define GFX75_SAMPLER_STATE_length  4
#endif
#ifndef GFX7_SAMPLER_STATE_length
#define GFX7_SAMPLER_STATE_length  4
#endif
#ifndef GFX6_SAMPLER_STATE_length
#define GFX6_SAMPLER_STATE_length  4
#endif
#ifndef GFX5_SAMPLER_STATE_length
#define GFX5_SAMPLER_STATE_length  4
#endif
#ifndef GFX45_SAMPLER_STATE_length
#define GFX45_SAMPLER_STATE_length  4
#endif
#ifndef GFX4_SAMPLER_STATE_length
#define GFX4_SAMPLER_STATE_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
SAMPLER_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 4;
   case 45: return 4;
   case 40: return 4;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* VERTEX_BUFFER_STATE */


#ifndef GFX35_VERTEX_BUFFER_STATE_length
#define GFX35_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX30_VERTEX_BUFFER_STATE_length
#define GFX30_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX20_VERTEX_BUFFER_STATE_length
#define GFX20_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX125_VERTEX_BUFFER_STATE_length
#define GFX125_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX12_VERTEX_BUFFER_STATE_length
#define GFX12_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX11_VERTEX_BUFFER_STATE_length
#define GFX11_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX9_VERTEX_BUFFER_STATE_length
#define GFX9_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX8_VERTEX_BUFFER_STATE_length
#define GFX8_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX75_VERTEX_BUFFER_STATE_length
#define GFX75_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX7_VERTEX_BUFFER_STATE_length
#define GFX7_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX6_VERTEX_BUFFER_STATE_length
#define GFX6_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX5_VERTEX_BUFFER_STATE_length
#define GFX5_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX45_VERTEX_BUFFER_STATE_length
#define GFX45_VERTEX_BUFFER_STATE_length  4
#endif
#ifndef GFX4_VERTEX_BUFFER_STATE_length
#define GFX4_VERTEX_BUFFER_STATE_length  4
#endif

static inline uint32_t ATTRIBUTE_PURE
VERTEX_BUFFER_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 4;
   case 300: return 4;
   case 200: return 4;
   case 125: return 4;
   case 120: return 4;
   case 110: return 4;
   case 90: return 4;
   case 80: return 4;
   case 75: return 4;
   case 70: return 4;
   case 60: return 4;
   case 50: return 4;
   case 45: return 4;
   case 40: return 4;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



/* VERTEX_BUFFER_STATE::Buffer Starting Address */


#define GFX35_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX30_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX20_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX125_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX12_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX11_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX9_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX8_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  64
#define GFX75_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32
#define GFX7_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32
#define GFX6_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32
#define GFX5_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32
#define GFX45_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32
#define GFX4_VERTEX_BUFFER_STATE_BufferStartingAddress_bits  32

static inline uint32_t ATTRIBUTE_PURE
VERTEX_BUFFER_STATE_BufferStartingAddress_bits(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 64;
   case 300: return 64;
   case 200: return 64;
   case 125: return 64;
   case 120: return 64;
   case 110: return 64;
   case 90: return 64;
   case 80: return 64;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#define GFX35_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX30_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX20_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX125_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX12_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX11_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX9_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX8_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX75_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX7_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX6_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX5_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX45_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32
#define GFX4_VERTEX_BUFFER_STATE_BufferStartingAddress_start  32

static inline uint32_t ATTRIBUTE_PURE
VERTEX_BUFFER_STATE_BufferStartingAddress_start(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 32;
   case 300: return 32;
   case 200: return 32;
   case 125: return 32;
   case 120: return 32;
   case 110: return 32;
   case 90: return 32;
   case 80: return 32;
   case 75: return 32;
   case 70: return 32;
   case 60: return 32;
   case 50: return 32;
   case 45: return 32;
   case 40: return 32;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}


/* VERTEX_ELEMENT_STATE */


#ifndef GFX35_VERTEX_ELEMENT_STATE_length
#define GFX35_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX30_VERTEX_ELEMENT_STATE_length
#define GFX30_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX20_VERTEX_ELEMENT_STATE_length
#define GFX20_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX125_VERTEX_ELEMENT_STATE_length
#define GFX125_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX12_VERTEX_ELEMENT_STATE_length
#define GFX12_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX11_VERTEX_ELEMENT_STATE_length
#define GFX11_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX9_VERTEX_ELEMENT_STATE_length
#define GFX9_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX8_VERTEX_ELEMENT_STATE_length
#define GFX8_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX75_VERTEX_ELEMENT_STATE_length
#define GFX75_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX7_VERTEX_ELEMENT_STATE_length
#define GFX7_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX6_VERTEX_ELEMENT_STATE_length
#define GFX6_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX5_VERTEX_ELEMENT_STATE_length
#define GFX5_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX45_VERTEX_ELEMENT_STATE_length
#define GFX45_VERTEX_ELEMENT_STATE_length  2
#endif
#ifndef GFX4_VERTEX_ELEMENT_STATE_length
#define GFX4_VERTEX_ELEMENT_STATE_length  2
#endif

static inline uint32_t ATTRIBUTE_PURE
VERTEX_ELEMENT_STATE_length(const struct intel_device_info *devinfo)
{
   switch (devinfo->verx10) {
   case 350: return 2;
   case 300: return 2;
   case 200: return 2;
   case 125: return 2;
   case 120: return 2;
   case 110: return 2;
   case 90: return 2;
   case 80: return 2;
   case 75: return 2;
   case 70: return 2;
   case 60: return 2;
   case 50: return 2;
   case 45: return 2;
   case 40: return 2;
   default:
      UNREACHABLE("Invalid hardware generation");
   }
}



#ifdef __cplusplus
}
#endif

#endif /* GENX_BITS_H */
