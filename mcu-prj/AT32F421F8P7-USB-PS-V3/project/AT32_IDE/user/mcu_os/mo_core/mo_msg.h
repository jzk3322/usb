/**
 * @file mo_msg.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __MESSAGE__H___
#define __MESSAGE__H___

#ifdef __cplusplus
extern "C" {
#endif

// #include "../mcu/mo_mcu.h"
#include "../mcu/mo_config.h"
#include "../mo_core/mo_list.h"

/*! @file message_.h @brief mo message types */
/*!
mo message identifier type.
*/
typedef mo_u16 mo_msg_id;
/*!
mo_delay delay type.
*/
typedef mo_u32 mo_delay;
/*!
mo_msg type.
*/
typedef const void *mo_msg;
/*!
mo_task type.
*/
typedef struct mo_task_data *mo_task;

typedef void (*mo_loop_hook)(void);

/*!
motask_data type.
*/
typedef struct mo_task_data {
    void (*handler)(mo_task, mo_msg_id, mo_msg);
} mo_task_data;

#define BOOL_V bool
typedef enum {
    MSG_PASS_SUCCESSFUL = 0,  // send message successfully
    MSG_POOL_FULL = 1,        // the message table is full
    MSG_PASS_FAILED = 2

} MSG_PASS_STATUS;

// #define MAX_MESSAGE_	50

// #define MAX_MSG_SLOT_	5
// #define A_SLOT_SIZE 	128
typedef struct msg_form {
    mo_task task;
    mo_msg_id id;
    mo_delay delay;
    mo_msg message;
    // bool msg_in_progress;
} MESSAGE_FORM_T;

typedef struct delay_msg_table {
    MESSAGE_FORM_T msg_mass;
    D_LIST_ELMT_T element;

} DELAY_MSG_TABLE_T;

typedef struct fifo_msg_ {
    MESSAGE_FORM_T msg_table[MAX_READY_MSG];
    mo_u8 size;
    mo_u8 pick_up_index;
    mo_u8 dump_index;
} FIFO_MSG_T;

typedef enum {

    TASK_ACK_OK = 1,
    TASK_ACK_FAIL,

    NONE
} TASK_ACK_RESULT_T;

typedef struct task_ack_msg__ {
    mo_task task;                 /***/
    mo_msg_id task_msg_id;        /**/
    TASK_ACK_RESULT_T ack_result; /*The result to return.*/
} TASK_ACK_MESSAGE_T;

void task_ack_to_(mo_task current_tsk, void *msg_from, TASK_ACK_RESULT_T result, mo_msg_id which_msg_to_ack);
void mo_msg_init(void);
void mo_msg_loop(mo_loop_hook hook);

MSG_PASS_STATUS mo_msg_send(const mo_task task, mo_msg_id id, mo_msg message, mo_delay delay);
mo_u16 mo_msg_cancel_all(const mo_task task, mo_msg_id id);


#ifdef __cplusplus
}
#endif
#endif
