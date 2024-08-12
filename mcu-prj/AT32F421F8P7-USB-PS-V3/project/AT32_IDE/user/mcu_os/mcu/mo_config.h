/**
 * @file mo_config.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef OS_DATA_TYPE_DEF__H__H
#define OS_DATA_TYPE_DEF__H__H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MO_FALSE
#define MO_FALSE 0
#endif

#ifndef MO_TRUE
#define MO_TRUE 1
#endif

/*******@TODO need to define these, those depend on your IDE*/
//MO_CH58x
typedef int32_t mo_s32;
typedef short mo_s16;
typedef signed char mo_s8;
typedef uint32_t mo_u32;
typedef unsigned short mo_u16;
typedef unsigned char mo_u8;
//
/***************************************/

#define MAX_DELAY_MSG 70u
#define MAX_READY_MSG 30u
#define MO_VERSION    "V1.0.0"
#define MO_DEBUG_EN   0

#if MO_DEBUG_EN

#define MO_LOG(format, ...) \
  printf("[%s:%d]" format, __func__, __LINE__, ##__VA_ARGS__)
// printf("[%s:%d->%s]"format,__FILE__,__LINE__,__func__,##__VA_ARGS__)
#define mo_printf(format, ...) printf(format, ##__VA_ARGS__)
#define mo_printf_buf(buf,len) do{\
  int i;\
  for(i=0;i<(len);i++){\
      printf("%x ",(buf)[i]);\
  }\
  printf("\r\n");\
}while(0);

#else

#define MO_LOG(format, ...)
#define mo_printf(format, ...)

#endif

#include "mo_event.h"
#include "mo_mcu.h"

/*********************@#TODO: ****************************************/
#define MCU_SLOT_SIZE 64ul /*a slot size in bytes*/
#define MAX_MCU_HEAP                                                          \
    (MCU_SLOT_SIZE * 32ul) /*define the maximum heap in byte, it must be less \
                              than or equal 2kB*/
#define MAX_MCU_SLOT          (MAX_MCU_HEAP / MCU_SLOT_SIZE)
#define MO_CRITICAL_ENTER()   mo_critical_enter()
#define MO_CRITICAL_EXIT(flg) mo_critical_exit(flg)
#define MO_GET_TICK()         mo_get_tick()
#define MO_TICK_TIMER_INIT()  mo_1ms_timer_nit()
#define MO_FORCE_SYS_RESET()  mo_reset_mcu()
#define MO_OS_SLEEP(time)     _sleep(time) /***Only for window demo**/
#define MO_DELAY_TICK(delay)  mo_delay_tick(delay)
/***************************************************************************/


#include "usr_debug.h"

#ifdef __cplusplus
}
#endif

#endif
