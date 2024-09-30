/**
 * @file usr_input_task.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "at32f421_wk_config.h"
#include "mcu_button.h"
#include "mo_input.h"
#include "mo_msg.h"
#include "usr_input_task.h"
#include "usr_main_app.h"
typedef enum{
    REC_STEP_50=0,
    REC_STEP_100,
    REC_STEP_1000,
}RecStepSizeT;


typedef struct usr_input_task {
    mo_task_data task;
    mo_task cline_tsk;
    RecStepSizeT step_size;
    RecStepSizeT step_dec_size;

    mo_u8 rec_cw_debounce;
    mo_u8 rec_ccw_debounce;

} UsrInputTaskT;

static UsrInputTaskT input_task;

mo_task get_usr_input_task(void){
    return &input_task.task;
}

static void btn_msg_handle(mo_vk_code_t code, mo_btn_pattern_t btn_patt, u8 click_num) {
    mo_key_event_msg_t k_event={.vk_code=code,.vk_patt=btn_patt,.click_num=click_num};
    if(input_task.cline_tsk!=NULL)
        send_btn_msg(input_task.cline_tsk,(void*)&k_event,sizeof(k_event));
}

void usr_input_tsk_handle(mo_task tsk, mo_msg_id m_id, mo_msg msg) {
    switch (m_id) {
        case SYS_EVENT_KEY_POLLING:
            mcu_button_loop(USR_KEY_POLLING_INTERVAL);
            mo_msg_send(tsk,SYS_EVENT_KEY_POLLING,NULL,USR_KEY_POLLING_INTERVAL);
            break;
        case SYS_EVENT_KEY_LOW_LEVEL_POLLING:
            io_key_task_handle();
            mo_msg_send(tsk,SYS_EVENT_KEY_LOW_LEVEL_POLLING,NULL,1);
            break;
        
        case USR_ROTATE_INT_TOGGLE:
             if(gpio_input_data_bit_read(ROTATE_ENCODER_B_POART,ROTATE_ENCODER_B_PIN)){
                mo_msg_send(get_usr_input_task(),USR_ROTATE_ENCODER_CCW,0,0);
            }else{
                mo_msg_send(get_usr_input_task(),USR_ROTATE_ENCODER_CW,0,0);
            }
            break;    

        case USR_REC_STEP_1000_TIMEOUT:
            input_task.step_size = REC_STEP_100;
            break;    
        case USR_REC_STEP_50_TIMEOUT:
            input_task.step_size = REC_STEP_50;
            break;   
        case USR_ROTATE_ENCODER_CW:   
            if(input_task.cline_tsk!=NULL){
                switch (input_task.step_size) {
                    case REC_STEP_1000:
                        mo_msg_send(input_task.cline_tsk, USR_INC_1000_MSG, msg, 0);
                        break;
                    case REC_STEP_100:
                        mo_msg_send(input_task.cline_tsk, USR_INC_100_MSG, msg, 0);
                        break;
                    default:
                        mo_msg_send(input_task.cline_tsk, USR_INC_50_MSG, msg, 0);
                        break;
                }
                
                mo_msg_cancel_all(tsk,USR_REC_STEP_1000_TIMEOUT);
                mo_msg_send(tsk,USR_REC_STEP_1000_TIMEOUT,NULL,30);

                mo_msg_cancel_all(tsk,USR_REC_STEP_50_TIMEOUT);
                mo_msg_send(tsk,USR_REC_STEP_50_TIMEOUT,NULL,100);

                input_task.step_size = REC_STEP_1000;
                //   USR_DBG_INFO("+\r\n");
            }  
            break; 
        case USR_REC_STEP_DEC_1000_TIMEOUT:
            input_task.step_dec_size = REC_STEP_100;
            break;    
        case USR_REC_STEP_DEC_50_TIMEOUT:
            input_task.step_dec_size = REC_STEP_50;
            break;   
        case USR_ROTATE_ENCODER_CCW:
            if (input_task.cline_tsk != NULL) {

                switch (input_task.step_dec_size) {
                    case REC_STEP_1000:
                        mo_msg_send(input_task.cline_tsk, USR_DEC_1000_MSG, msg, 0);
                        break;
                    case REC_STEP_100:
                        mo_msg_send(input_task.cline_tsk, USR_DEC_100_MSG, msg, 0);
                        break;
                    default:
                        mo_msg_send(input_task.cline_tsk, USR_DEC_50_MSG, msg, 0);
                        break;
                }

                mo_msg_cancel_all(tsk,USR_REC_STEP_DEC_1000_TIMEOUT);
                mo_msg_send(tsk,USR_REC_STEP_DEC_1000_TIMEOUT,NULL,30);

                mo_msg_cancel_all(tsk,USR_REC_STEP_DEC_50_TIMEOUT);
                mo_msg_send(tsk,USR_REC_STEP_DEC_50_TIMEOUT,NULL,100);

                input_task.step_dec_size = REC_STEP_1000;
                // USR_DBG_INFO("-\r\n");
                
            }
            break;    

        default:
            break;
    }
}

/**
 * @brief 
 * 
 * @param cline_task point to an application task, that is received the btn message
 */
void usr_input_task_init(const mo_task cline_task){/*#TODO: Invoked in main function to init*/
    memset(&input_task, 0, sizeof(input_task));
    input_task.task.handler = usr_input_tsk_handle;
    input_task.cline_tsk = cline_task;
    
    mcu_btn_init(btn_msg_handle);

    mo_msg_cancel_all(&input_task.task,SYS_EVENT_KEY_POLLING);
    mo_msg_send(&input_task.task,SYS_EVENT_KEY_POLLING,NULL,100);

    mo_msg_cancel_all(&input_task.task,SYS_EVENT_KEY_LOW_LEVEL_POLLING);
    mo_msg_send(&input_task.task,SYS_EVENT_KEY_LOW_LEVEL_POLLING,NULL,100);
}
