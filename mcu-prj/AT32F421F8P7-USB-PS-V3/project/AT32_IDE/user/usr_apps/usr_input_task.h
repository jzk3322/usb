/**
 * @file usr_input_task.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef USER_INPUT_TASK_HHHH____HH
#define USER_INPUT_TASK_HHHH____HH
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "io_key.h"

#define USR_KEY_POLLING_INTERVAL 10

void usr_input_task_init(const mo_task cline_task);
mo_task get_usr_input_task(void);
#ifdef __cplusplus
}
#endif

#endif
