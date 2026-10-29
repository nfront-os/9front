/* <machine/endian.h> of the BSDs, which LLVM's bit.h wants: gcc knows the order */
#ifndef __APE_MACHINE_ENDIAN_H
#define __APE_MACHINE_ENDIAN_H
#define LITTLE_ENDIAN	__ORDER_LITTLE_ENDIAN__
#define BIG_ENDIAN	__ORDER_BIG_ENDIAN__
#define PDP_ENDIAN	__ORDER_PDP_ENDIAN__
#define BYTE_ORDER	__BYTE_ORDER__
#define _LITTLE_ENDIAN	LITTLE_ENDIAN
#define _BIG_ENDIAN	BIG_ENDIAN
#define _BYTE_ORDER	BYTE_ORDER
#endif
