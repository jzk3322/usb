/**
 * @file io_key.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef KEY_HHHH____
#define KEY_HHHH____

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

/*#TODO: Here need to cfg. that depend on your requirement*/
typedef enum{
    HW_KEY_NULL=0,
    // HW_KEY_LEFT,
    // HW_KEY_RIGHT,
    HW_KEY_CENTER,

    HW_KEY_MAX_
}mo_hw_key_t;

#define GPIO_KEY_MAX         (HW_KEY_MAX_-1)
#define KEY_POLLING_INTERVAL 5u
#define KEY_DEBOUNCE_TIME    (10 / KEY_POLLING_INTERVAL) /*Ms*/
/**************************************************************/

#define HW_k_NUMBER_ (HW_KEY_MAX_-1)

typedef enum{
    IO_K_EVENT_VK_NULL=0,
    IO_K_EVENT_VK_UP,
    IO_K_EVENT_VK_DW,
    IO_K_EVENT_ENTER,

    IO_K_EVENT_VK_MAX____
}ioKeyEvent;

typedef void (*io_k_event_cb)(ioKeyEvent k_event,mo_hw_key_t kcode);

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u8
#define u8 uint8_t
#endif

typedef enum { IO_PIN_RESET = 0U, IO_PIN_SET } IO_PinState;
#define IO_KEY_STATE IO_PinState
typedef IO_KEY_STATE (*get_key_state)(void *port,u16 pin,void *param);

typedef struct ex_key_infor {
    mo_hw_key_t kcode;
    void *port; /**/
    u16 pin;
    IO_KEY_STATE k_active_level;
    get_key_state get_k_state;
} ExKyeInfo;

typedef struct key_task {
    u8 ex_k_cnt[GPIO_KEY_MAX];
    const ExKyeInfo *pKeyTable;
    //ioKeyEvent k_event;
    u16 app_tick;
    io_k_event_cb k_event_cb;
} KEY_TASK_T;


void io_key_task_handle(void); /*#TODO:Invoked in 1mS loop*/
void io_key_task_init(io_k_event_cb k_event_cb);


#ifdef __cplusplus
}
#endif

#endif