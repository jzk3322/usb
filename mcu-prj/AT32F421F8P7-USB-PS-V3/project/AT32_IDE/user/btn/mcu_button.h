/**
 * @file mcu_button.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MO_BUTTON_HHH__HH_
#define MO_BUTTON_HHH__HH_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "io_key.h"


#ifndef u16
#define u16 uint16_t
#endif

#ifndef u8
#define u8 uint8_t
#endif

typedef enum {
  VK_NULL = 0,
  MO_VK_DW,
  MO_VK_UP,
} vk_state_t;

typedef enum {

    BTN_DOWN,
    BTN_UP,
    BTN_SINGLE_CLICK,
    BTN_MULTI_CLICK,
    BTN_LP,   /**long press**/
    BTN_VLP,  /**Very long press*/
    BTN_VVLP, /**Very very long press*/
    BTN_VVVLP,
    BTN_VVVVLP,

} mo_btn_pattern_t;

/*#TODO: Here need to cfg. that depend on your requirement*/
#define BTN_LOOP_INTERVAL   10ul                          /*mS**/
#define MULTI_CLICK_TIMEOUT (500ul / BTN_LOOP_INTERVAL)   /**mS*/
#define BTN_LP_TIMEOUT      (2500ul / BTN_LOOP_INTERVAL)  /**mS*/
#define BTN_VLP_TIMEOUT     (4000ul / BTN_LOOP_INTERVAL)  /**mS*/
#define BTN_VVLP_TIMEOUT    (5500ul / BTN_LOOP_INTERVAL)  /**mS*/
#define BTN_VVVLP_TIMEOUT   (8000ul / BTN_LOOP_INTERVAL)  /**mS*/
#define BTN_VVVVLP_TIMEOUT  (10000ul / BTN_LOOP_INTERVAL) /**mS*/
/**************************************************************************************/

#define BTN_TIMING_TABLE                                                    \
  {                                                                         \
    MULTI_CLICK_TIMEOUT, BTN_LP_TIMEOUT, BTN_VLP_TIMEOUT, BTN_VVLP_TIMEOUT, \
        BTN_VVVLP_TIMEOUT, BTN_VVVVLP_TIMEOUT,                              \
  }

#define VK_CODE_NULL HW_KEY_NULL
#define mo_vk_code_t mo_hw_key_t

typedef struct btn_timing_ {
  u16 m_click_tim;
  u16 lp_tim;
  u16 vlp_tim;
  u16 vvlp_tim;
  u16 vvvlp_tim;
  u16 vvvvlp_tim;
} mo_btn_timing_t;

typedef struct btn_widget {
  mo_vk_code_t vk_code;
  u16 hold_cnt;
  u8 click_cnt;
  vk_state_t vk_state;

} btn_widget_t;

typedef struct btn_uint {
  btn_widget_t btn_widget;
} btn_unit_t;


typedef struct {
    mo_vk_code_t vk_code;
    mo_btn_pattern_t vk_patt;
    u8 click_num;
} mo_key_event_msg_t;

typedef void (*btn_msg_cb_handle)(mo_vk_code_t code, mo_btn_pattern_t btn_patt, u8 click_num);

typedef struct button_task {
  u8 vk_number;
  u8 btn_polling_tick;
  btn_widget_t btn_widget_set[HW_k_NUMBER_];
  btn_msg_cb_handle btn_msg_cb;
} btn_task_t;

u8 mcu_btn_init(btn_msg_cb_handle btn_msg_cb);/*#TODO: Need Invoked to init btn function in main fun.*/
void mcu_button_loop(u8 interval/*mS*/); /*#TODO: Invoked in a loop*/


#ifdef __cplusplus
}
#endif
#endif
