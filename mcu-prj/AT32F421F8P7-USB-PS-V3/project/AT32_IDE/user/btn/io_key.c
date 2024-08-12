/**
 * @file io_key.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "at32f421_gpio.h"
#include "mo_config.h"
#include "io_key.h"

static KEY_TASK_T k_task;

/**************************************************************************
 * @brief Get the key state object
 * #TODO:    You have to make the function below (get_gpio_key_state) that depend on your 
            hardware cfg and which chips you used.
 * @param port 
 * @param pin 
 * @param param 
 * @return IO_KEY_STATE 
 *************************************************************************/
static IO_KEY_STATE get_a_key_state(void *port,u16 pin,void *param){
    return gpio_input_data_bit_read((gpio_type*)port,pin);
}

const ExKyeInfo ex_key_map[GPIO_KEY_MAX] = {
    // {
    //     .kcode = HW_KEY_LEFT,
    //     .port = GPIOB,
    //     .pin = GPIO_PINS_1,
    //     .k_active_level = IO_PIN_RESET,
    //     .get_k_state = get_a_key_state,
    // },
    // {
    //     .kcode = HW_KEY_RIGHT,
    //     .port = GPIOB,
    //     .pin = GPIO_PINS_2,
    //     .k_active_level = IO_PIN_RESET,
    //     .get_k_state = get_a_key_state,
    // },
    {
        .kcode = HW_KEY_CENTER,
        .port = GPIOA,
        .pin = GPIO_PINS_2,
        .k_active_level = IO_PIN_RESET,
        .get_k_state = get_a_key_state,
    },
   
};
/**********************************************************************************/

void io_key_task_handle(void) { /*#TODO:Invoked in 1mS loop*/
    IO_KEY_STATE pio;
    u8 j;
    const ExKyeInfo *pKTable;

    k_task.app_tick++;
    if (k_task.app_tick < KEY_POLLING_INTERVAL) return;
    k_task.app_tick = 0;
    // MO_LOG("PIO:%d\r\n",23);
    for (j = 0; j < GPIO_KEY_MAX; j++) {
        pKTable=&k_task.pKeyTable[j];

        if (pKTable->port != 0) {
            pio = pKTable->get_k_state(pKTable->port,pKTable->pin,NULL);
           // MO_LOG("PIO:%d\r\n",pio);
            
            if (pio != pKTable->k_active_level) { /*Key Up*/
                if (k_task.ex_k_cnt[j] != 0) {
                    k_task.ex_k_cnt[j]--;
                    if (k_task.ex_k_cnt[j] == 0 && k_task.k_event_cb) {
                        k_task.k_event_cb(IO_K_EVENT_VK_UP, pKTable->kcode);
                    }
                }
            } else { /*Key Down*/
                if (k_task.ex_k_cnt[j] < KEY_DEBOUNCE_TIME) {
                    k_task.ex_k_cnt[j]++;
                    if (k_task.ex_k_cnt[j] >= KEY_DEBOUNCE_TIME && k_task.k_event_cb) {
                        k_task.k_event_cb(IO_K_EVENT_VK_DW, pKTable->kcode);
                    }
                }
            }
        }
    }
}

/**************************************************************************
 * @brief #TODO: you should to init the GPIO as input mode or other input devices
 *  in below function (key_gpio_init)
 * 
 *************************************************************************/
static void key_gpio_init(void){


}

/**************************************************************************
 * @brief 
 * 
 * @param k_event_cb 
 *************************************************************************/
void io_key_task_init(io_k_event_cb k_event_cb) {
  memset(&k_task, 0, sizeof(k_task));
  k_task.pKeyTable = ex_key_map;
  k_task.k_event_cb = k_event_cb;
  key_gpio_init();
}

/**************************************************************/
