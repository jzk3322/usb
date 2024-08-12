/**
 * @file mo_msg.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "../mcu/mo_config.h"
#include "mo_heap.h"
#include "mo_msg.h"


static FIFO_MSG_T msg_ready_pool;

static DELAY_MSG_TABLE_T delay_msg_table[MAX_DELAY_MSG];
static D_LIST_T delay_msg_unengaged_list;
static D_LIST_T delay_msg_engaged_list;

static MSG_PASS_STATUS fifo_put_msg(FIFO_MSG_T *fifo_msg_t, const MESSAGE_FORM_T *msg_t) {
    MSG_PASS_STATUS rec = MSG_POOL_FULL;
    mo_bool ret_it;

    if (fifo_msg_t->size < (MAX_READY_MSG)) {
        ret_it = MO_CRITICAL_ENTER();

        memcpy(&fifo_msg_t->msg_table[fifo_msg_t->dump_index], msg_t, sizeof(MESSAGE_FORM_T));
        fifo_msg_t->size++;
        fifo_msg_t->dump_index++;

        if (fifo_msg_t->dump_index >= MAX_READY_MSG) fifo_msg_t->dump_index = 0;

        MO_CRITICAL_EXIT(ret_it);
        rec = MSG_PASS_SUCCESSFUL;
    }

    return rec;
}
static MESSAGE_FORM_T *fifo_get_msg(FIFO_MSG_T *fifo_msg_t, MESSAGE_FORM_T *msg_t) {
    MESSAGE_FORM_T *msg_add = NULL;
    mo_bool ret_it;
    // MESSAGE_FORM_T *pmsg=NULL;

    if (fifo_msg_t->size > 0) {
        ret_it = MO_CRITICAL_ENTER();

        msg_add = &fifo_msg_t->msg_table[fifo_msg_t->pick_up_index];
        memcpy(msg_t, msg_add, sizeof(MESSAGE_FORM_T));
        fifo_msg_t->pick_up_index++;
        fifo_msg_t->size--;

        if (fifo_msg_t->pick_up_index >= MAX_READY_MSG) fifo_msg_t->pick_up_index = 0;

        MO_CRITICAL_EXIT(ret_it);
        // ret=TRUE;
    }
    return msg_add;
}

/****
@return the message whether passed.
   MSG_POOL_FULL: the message table is full
   MSG_PASS_SUCCESSFUL: the message pass to message table successfully
****/
// mo_u32 debug_msg_in_cnt=0;

MSG_PASS_STATUS put_delay_msg(const MESSAGE_FORM_T *msg_t) {
    mo_bool ret_it;
    D_LIST_ELMT_T *msg_list_elem;

    MSG_PASS_STATUS rec = MSG_POOL_FULL;

    ret_it = MO_CRITICAL_ENTER();

    msg_list_elem = dlist_remove(&delay_msg_unengaged_list, delay_msg_unengaged_list.head);

    if (msg_list_elem != NULL) {
        //	debug_msg_in_cnt++;
        memcpy(msg_list_elem->dat, msg_t, sizeof(MESSAGE_FORM_T));
        if (dlist_ins_next(&delay_msg_engaged_list, delay_msg_engaged_list.head, msg_list_elem, msg_list_elem->dat) == MO_TRUE) {
            rec = MSG_PASS_SUCCESSFUL;
        }
    }

    MO_CRITICAL_EXIT(ret_it);

    return rec;
}

void mo_msg_init(void) {
    mo_u32 temp, i;
    // LIST_ELMT_T *msg_list_elem;
    MESSAGE_FORM_T msg_t;

    mo_bool ret_it;
    ret_it = MO_CRITICAL_ENTER();

    temp = sizeof(delay_msg_table);
    memset(&delay_msg_table, 0x00, temp);
    memset(&msg_ready_pool, 0x00, sizeof(FIFO_MSG_T));
    memset(&msg_t, 0x00, sizeof(MESSAGE_FORM_T));

    delay_msg_unengaged_list.size = 0;
    delay_msg_unengaged_list.head = NULL;
    delay_msg_unengaged_list.tail = NULL;

    delay_msg_engaged_list.size = 0;
    delay_msg_engaged_list.head = NULL;
    delay_msg_engaged_list.tail = NULL;

    for (i = 0; i < MAX_DELAY_MSG; i++) {
        dlist_ins_next(&delay_msg_unengaged_list, delay_msg_unengaged_list.head, &delay_msg_table[i].element, &delay_msg_table[i].msg_mass);
    }

    MO_CRITICAL_EXIT(ret_it);
}

/*!
  @brief Send a message to the corresponding task immediately.
  @param task The task to deliver the message to.
  @param id The message type identifier.
  @param message The message data (if any).
  @return the message whether passed.
        MSG_POOL_FULL: the message table is full
        MSG_PASS_SUCCESSFUL: the message pass to message table successfully
*/
MSG_PASS_STATUS mo_msg_send(const mo_task task, mo_msg_id id, mo_msg message, mo_delay delay) {
    //	mo_u8 i;
    MSG_PASS_STATUS rec;
    MESSAGE_FORM_T msg_t;

    if (task == NULL) return MSG_PASS_FAILED;

    msg_t.id = id;
    msg_t.task = task;
    msg_t.delay = delay;
    msg_t.message = message;
    // printf("task->handler-=-%d ",task->handler);
    // printf("msg_t.task->handler=-=-%d ",msg_t.task->handler);
    if (msg_t.delay) {
        rec = put_delay_msg(&msg_t);
    } else {
        rec = fifo_put_msg(&msg_ready_pool, &msg_t);
    }

    return rec;
}

/*!
  @brief Cancel all queued messages with the given task and message id.

  @param task The task to cancel message for.
  @param id The message identifier to search for.
  @return A count of how many such messages were cancelled.
*/
mo_u16 mo_msg_cancel_all(mo_task task, mo_msg_id id) {
    mo_u8 i;
    mo_bool ret_it;
    mo_u8 delete_msg = 0;
    D_LIST_ELMT_T *list_elem = NULL;
    D_LIST_ELMT_T *free_list_elem = NULL;
    MESSAGE_FORM_T *msg_t;

    if (task == NULL) return 0;

    ret_it = MO_CRITICAL_ENTER();

    /******Delete delay message from delay_msg_engaged_list ********/
    list_elem = delay_msg_engaged_list.head;

    while (list_elem != NULL) {
        msg_t = (MESSAGE_FORM_T *)list_elem->dat;

        if (msg_t != NULL) {
            if ((msg_t->id == id) && (msg_t->task == task)) {
                free_list_elem = dlist_remove(&delay_msg_engaged_list, list_elem);

                if (free_list_elem != NULL) {
                    mo_free(msg_t->message);
                    memset(msg_t, 0x00, sizeof(MESSAGE_FORM_T));
                    dlist_ins_next(&delay_msg_unengaged_list, delay_msg_unengaged_list.head, free_list_elem, free_list_elem->dat);
                    delete_msg++;
                }
            }
        }

        list_elem = list_elem->next;
    }
    /*************/

    /******Delete delay message from msg_ready_pool ********/

    if (msg_ready_pool.size) {
        i = msg_ready_pool.pick_up_index;

        while (msg_ready_pool.dump_index != i) {
            if ((msg_ready_pool.msg_table[i].id == id) && (msg_ready_pool.msg_table[i].task == task)) {
                // ret_it=MO_CRITICAL_ENTER();
                mo_free(msg_ready_pool.msg_table[i].message);
                memset(&msg_ready_pool.msg_table[i], 0x00, sizeof(MESSAGE_FORM_T));

                delete_msg++;
            }

            i++;
            if (i >= MAX_READY_MSG) i = 0;
        }
    }
    /***********/

    MO_CRITICAL_EXIT(ret_it);

    return delete_msg;
}

#if 0
/*!
  @brief this fuction is used to send a message to another device to do somthing,.

  @param current_task The task to send the message to.
  @param dev_task The message will be recieved by this device.
  @param dev_msg  defined the message to be passed to dev_task.
  @param ack_msg when the dev_task has been finished excuting dev_msg, that will ack the current_task with this message
*/

mo_u8 msg_control_device(mo_task current_tsk,mo_task dev_tsk, mo_msg_id dev_msg, mo_msg_id ack_msg, Delay dly){
	TASK_ACK_MESSAGE_T *tsk_ack=NULL;
	mo_u8 ret=0;

	
	tsk_ack=(TASK_ACK_MESSAGE_T*)mcu_malloc(sizeof(TASK_ACK_MESSAGE_T));
	if(tsk_ack != NULL){
		memset(tsk_ack,0x00,sizeof(TASK_ACK_MESSAGE_T));
		
		

		tsk_ack->task = current_tsk;
		tsk_ack->task_msg_id = ack_msg;
		
		message_send(dev_tsk,dev_msg,tsk_ack,dly);
		ret = 1;
	}
	return ret;
}
/*!
  @brief this fuction is used to ack a message to another task
  @param current_task The task to send the message to.
  @param *msg_from 
  @param result  
  @param which_msg_to_ack 
*/
void task_ack_to_(mo_task current_tsk, void *msg_from,TASK_ACK_RESULT_T result, mo_msg_id which_msg_to_ack){
	TASK_ACK_MESSAGE_T *request_tsk=NULL;
	TASK_ACK_MESSAGE_T *tsk_ack_to=NULL;

	if(msg_from != NULL){
		request_tsk=(TASK_ACK_MESSAGE_T *)msg_from;
		if(request_tsk->task == NULL)return;
		
		tsk_ack_to=(TASK_ACK_MESSAGE_T *)mcu_malloc(sizeof(TASK_ACK_MESSAGE_T));
		if(tsk_ack_to != NULL){
			tsk_ack_to->task = current_tsk;
			tsk_ack_to->ack_result = result;
			tsk_ack_to->task_msg_id = which_msg_to_ack;
			
			message_send(request_tsk->task,request_tsk->task_msg_id,tsk_ack_to,0);
		}
	}

}
#endif

static void mo_delay_msg_loop(void) {
    MESSAGE_FORM_T *msg_t;
    mo_bool ret_it;
    // mo_u8 found_msg_cnt;
    // mo_u8 msg_size_backup;
    D_LIST_ELMT_T *list_elem = NULL;
    D_LIST_ELMT_T *free_list_elem = NULL;
    D_LIST_ELMT_T *polling_elem = NULL;

    polling_elem = delay_msg_engaged_list.head;
    list_elem = polling_elem;
    // find_msg_count=0;
    while (polling_elem != NULL && (delay_msg_engaged_list.size)) {
        // find_msg_count ++;
        msg_t = (MESSAGE_FORM_T *)polling_elem->dat;
        list_elem = polling_elem;
        polling_elem = polling_elem->next;

        if (msg_t->delay)
            msg_t->delay--;
        else {
            ret_it = MO_CRITICAL_ENTER();
            // debug_msg_out_cnt++;
            fifo_put_msg(&msg_ready_pool, msg_t);
            memset(msg_t, 0x00, sizeof(MESSAGE_FORM_T)); /*Release current message*/

            free_list_elem = dlist_remove(&delay_msg_engaged_list, list_elem);

            if (free_list_elem != NULL) {
                dlist_ins_next(&delay_msg_unengaged_list, delay_msg_unengaged_list.head, free_list_elem, free_list_elem->dat);
            }

            MO_CRITICAL_EXIT(ret_it);
        }
    }
}

/*!
  @brief The main scheduler loop; it waits until the next message is due and
  then sends it to the corresponding task. Never returns.
*/
static mo_u32 msg_tick = 0;
#if MO_DEBUG_EN
const char *mo_theme[] = {
    "----------------------------\r\n", "----------------------------\r\n",
    "----------------------------\r\n", "----------------------------\r\n",
    "-------- hell mcu os -------\r\n", "----------------------------\r\n",
    "----------------------------\r\n", "----------------------------\r\n",
    "----------------------------\r\n", "----------------------------\r\n",

};
#endif

#ifdef MO_CH58x
__HIGH_CODE
__attribute__((noinline))
#endif
void mo_msg_loop(mo_loop_hook hook) {
    MESSAGE_FORM_T msg_t;
    MESSAGE_FORM_T *msg_add;
    mo_bool ret_it;
    mo_loop_hook lp_hook = hook;
    int i;
    MO_TICK_TIMER_INIT();

#if MO_DEBUG_EN
    MO_LOG("main\r\n");
    for (i = 0; i < sizeof(mo_theme) / (sizeof(char *)); i++) {
        mo_printf("%s", mo_theme[i]);
    }
    mo_printf("[mcu_os]%s\r\n", MO_VERSION);
#endif

    while (1) {
        if (MO_GET_TICK() != msg_tick) {
            msg_tick = MO_GET_TICK();
            mo_delay_msg_loop();
            // printf("mo msg loop runing \n");
        }

        msg_add = fifo_get_msg(&msg_ready_pool, &msg_t);
        if (msg_add != NULL) {
            if (msg_t.id != 0 && msg_t.task != NULL) {
                if (msg_t.task->handler != NULL) msg_t.task->handler(msg_t.task, msg_t.id, msg_t.message);

                ret_it = MO_CRITICAL_ENTER();

                mo_free(msg_t.message);
                memset(msg_add, 0x00, sizeof(MESSAGE_FORM_T)); /*Release current message*/

                MO_CRITICAL_EXIT(ret_it);
            }
        }
        if (lp_hook != NULL) {
            lp_hook();
        }
        //	MO_OS_SLEEP(10);
    }
}
/**
 * @brief MCU OS 启动，
 * 
 * @param hook 用户的一个回调函数，他在mo的消息循环中被反复调用。
 * @return int 
 */

int mo_schedule_start(mo_loop_hook hook){

    mo_msg_init();

    mo_msg_loop(hook); /*Never return*/

    return 0;
}
