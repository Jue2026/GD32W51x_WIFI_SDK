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
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "app_cfg.h"
#include "wrapper_os.h"
#include "arch/cc.h"
#include "lwipopts.h"

#ifdef LWIP_SSL_MQTT
#define LWIP_TASK_STK_POOL_SIZE                1024
#else
#define LWIP_TASK_STK_POOL_SIZE                512     /* 256 */
#endif

#if defined(PLATFORM_OS_UCOS) || defined(PLATFORM_OS_LITEOS)
#define LWIP_TASK_TCPIP_PRIO                   TASK_PRIO_APP_BASE
#elif defined(PLATFORM_OS_FREERTOS) || defined(PLATFORM_OS_AOS_RHINO) || defined(PLATFORM_OS_RTTHREAD)
#define LWIP_TASK_TCPIP_PRIO                   (TASK_PRIO_APP_BASE + TASK_PRIO_HIGHER(1))
#endif

#define LWIP_TASK_MAX                          1

extern os_task_t lwip_task_tcb[LWIP_TASK_MAX];
extern uint32_t lwip_task_stk[LWIP_TASK_STK_POOL_SIZE] __ALIGNED(8);

typedef os_sema_t           sys_sem_t;
typedef os_queue_t          sys_mbox_t;
typedef uint8_t             sys_thread_t;

#define SYS_MBOX_NULL       (void *)0
#define SYS_SEM_NULL        (void *)0

#define sys_sem_valid(sem)             (*((os_sema_t *)sem) != NULL)
#define sys_sem_set_invalid(sem)       (*((os_sema_t *)sem)  = NULL)
#define sys_mbox_valid(mbox)           (*((os_queue_t *)mbox) != NULL)
#define sys_mbox_set_invalid(mbox)     (*((os_queue_t *)mbox) = NULL)

#define sys_now             sys_current_time_get
#define sys_jiffies         (sys_now * OS_TICK_RATE_HZ / 1000)
#define sys_msleep          sys_ms_sleep

#define sys_sem_free(s)     sys_sema_free(s)
#define sys_sem_signal(s)   sys_sema_up(s)

#define sys_mbox_free(m)    sys_queue_free(m)

extern void sys_ms_sleep(uint32_t ms);


#endif
