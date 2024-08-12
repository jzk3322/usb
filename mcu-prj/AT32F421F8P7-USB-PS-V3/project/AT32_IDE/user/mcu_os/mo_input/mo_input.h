/**
 * @file mo_input.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MO_INPUT_HHH__HH_
#define MO_INPUT_HHH__HH_
#ifdef __cplusplus
extern "C" {
#endif

#include "../mcu/mo_config.h"
#include "../mo_core/mo_heap.h"
#include "../mo_core/mo_msg.h"

void send_btn_msg(const mo_task task,void *k_event, mo_u32 sizeof_k_even);

#ifdef __cplusplus
}
#endif

#endif
