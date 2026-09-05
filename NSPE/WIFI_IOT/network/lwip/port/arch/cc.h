/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * Copyright (c) 2024, GigaDevice Semiconductor Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#include <stdio.h>
#include "app_cfg.h"
#include "app_type.h"
#include "debug_print.h"

#if 0
/*-------------data type------------------------------------------------------*/

typedef unsigned char	u8_t;    		/* Unsigned 8 bit quantity         */
typedef signed char    	s8_t;    		/* Signed    8 bit quantity        */
typedef unsigned short u16_t;   	/* Unsigned 16 bit quantity        */
typedef signed short   	s16_t;   	/* Signed   16 bit quantity        */
typedef unsigned int  u32_t;   	/* Unsigned 32 bit quantity        */
typedef signed int    	s32_t;   	/* Signed   32 bit quantity        */
//typedef u32  		u32_t;   		/* Unsigned 32 bit quantity        */
//typedef s32    	s32_t;   		/* Signed   32 bit quantity        */
typedef u32_t 	mem_ptr_t;   	/* Unsigned 32 bit quantity        */
#endif

#define BYTE_ORDER 	LITTLE_ENDIAN

//#if CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL
#define SYS_ARCH_DECL_PROTECT(lev) SYS_SR_ALLOC()
#define SYS_ARCH_PROTECT(lev)		SYS_CRITICAL_ENTER()
#define SYS_ARCH_UNPROTECT(lev)	SYS_CRITICAL_EXIT()
//#endif


/*----------------------------------------------------------------------------*/

/* define compiler specific symbols */
#if defined ( __CC_ARM ) || defined ( __GNUC__ )
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __PACKED
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#endif

#if defined ( __ICCARM__ )
  #if __ICCARM_V8
    #define PACK_STRUCT_BEGIN
    #define PACK_STRUCT_STRUCT __attribute__((packed, aligned(1)))
  #else
    #define PACK_STRUCT_BEGIN __packed
    #define PACK_STRUCT_STRUCT
  #endif
  #define PACK_STRUCT_END
  #define PACK_STRUCT_FIELD(x) x
#endif


/*---define (sn)printf formatters for these lwip types, for lwip DEBUG/STATS--*/

#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/*--------------macros--------------------------------------------------------*/
#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); \
    } while(0)
#endif

#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {co_printf x;} while(0)
#endif

#endif
