/**
 * @file mo_input.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include "mo_input.h"

/**
 * @brief 
 * 
 * @param task 
 * @param k_event 
 * @param sizeof_k_even 
 */

void send_btn_msg(const mo_task task,void *k_event, mo_u32 sizeof_k_even) {
    void *msg = mo_malloc(sizeof_k_even);

    if (msg != NULL) {
        memcpy(msg,k_event,sizeof_k_even);
        mo_msg_send(task, SYS_KEY_EVENT, msg, 0);
    }

}

