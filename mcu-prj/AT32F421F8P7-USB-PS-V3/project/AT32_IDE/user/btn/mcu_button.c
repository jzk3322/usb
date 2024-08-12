/**
 * @file mcu_button.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mo_config.h"
#include "mcu_button.h"

static btn_task_t btn_tsk = {0};
const mo_btn_timing_t btn_timeing = BTN_TIMING_TABLE;

static btn_widget_t *mo_get_vk_widget_by_code(mo_vk_code_t vk_c) {
  btn_widget_t *widget = NULL;
  u8 i;
  u16 active_index = 0, null_index = 0;

  for (i = 0; i < btn_tsk.vk_number; i++) {
    if (btn_tsk.btn_widget_set[i].vk_code == VK_CODE_NULL) {
      if (!null_index) null_index = i + 1;
    }
    if (btn_tsk.btn_widget_set[i].vk_code == vk_c) {
      active_index = i + 1;
      break;
    }
  }

  if (active_index) {
    widget = &btn_tsk.btn_widget_set[active_index - 1];
  } else if (null_index) {
    widget = &btn_tsk.btn_widget_set[null_index - 1];
  }

  return widget;
}

static void send_btn_msg(mo_vk_code_t code, mo_btn_pattern_t btn_patt, u8 click_num) {
    if (btn_tsk.btn_msg_cb != NULL) {
        btn_tsk.btn_msg_cb(code, btn_patt,click_num);
    }
}

static void send_hold_vk_msg(btn_widget_t *widget, mo_btn_pattern_t btn_patt) {

    if (widget == NULL) return;

    if (widget->vk_state == MO_VK_DW) {
        send_btn_msg(widget->vk_code, btn_patt, 0);
    } else {
        widget->click_cnt = 0;
        widget->vk_code = VK_CODE_NULL;
    }
    
}

// interval: that defined the interval in your main loop, uint is mS
void mcu_button_loop(u8 interval /*mS*/) { /*#TODO: Invoked in a loop,*/

    btn_widget_t *widget;
    mo_btn_pattern_t bt_patt;
    u8 i;

    btn_tsk.btn_polling_tick++;
    if (btn_tsk.btn_polling_tick >= (BTN_LOOP_INTERVAL / interval)) {
        btn_tsk.btn_polling_tick = 0;

        for (i = 0; i < btn_tsk.vk_number; i++) {
            widget = &btn_tsk.btn_widget_set[i];
            if (widget->vk_code != VK_CODE_NULL) {
                widget->hold_cnt++;
                 MO_LOG("widget->hold_cnt %d \r\n",widget->hold_cnt);
                if (widget->hold_cnt == btn_timeing.m_click_tim) {
                    if (widget->vk_state == MO_VK_UP) {
                        if (widget->click_cnt > 1) {
                            bt_patt = BTN_MULTI_CLICK;
                        } else {
                            bt_patt = BTN_SINGLE_CLICK;
                        }
                        send_btn_msg(widget->vk_code, bt_patt, widget->click_cnt);
                        widget->click_cnt = 0;
                        widget->vk_code = VK_CODE_NULL;
                    }
                } else if (widget->hold_cnt == btn_timeing.lp_tim) {
                    send_hold_vk_msg(widget, BTN_LP);
                } else if (widget->hold_cnt == btn_timeing.vlp_tim) {
                    send_hold_vk_msg(widget, BTN_VLP);
                } else if (widget->hold_cnt == btn_timeing.vvlp_tim) {
                    send_hold_vk_msg(widget, BTN_VVLP);
                } else if (widget->hold_cnt == btn_timeing.vvvlp_tim) {
                    send_hold_vk_msg(widget, BTN_VVVLP);
                } else if (widget->hold_cnt == btn_timeing.vvvvlp_tim) {
                    send_hold_vk_msg(widget, BTN_VVVVLP);
                } else if (widget->hold_cnt > btn_timeing.vvvvlp_tim) {
                    // widget->vk_code = VK_CODE_NULL;
                }
            }
        }
    }
}

static void io_k_event_cb_handle(ioKeyEvent k_event, mo_hw_key_t kcode) {
    u8 i;
    mo_vk_code_t vk_code;
    btn_widget_t *widget;

    MO_LOG("k event:%d,kcode:%d\r\n",k_event,kcode);

    switch (k_event) {
        case IO_K_EVENT_VK_UP:
            vk_code = kcode;
            for (i = 0; i < btn_tsk.vk_number; i++) {
                if (btn_tsk.btn_widget_set[i].vk_code == vk_code) {
                    widget = &btn_tsk.btn_widget_set[i];
                    widget->vk_code = vk_code;

                    if (widget->vk_state != MO_VK_UP) {
                        widget->vk_state = MO_VK_UP;
                        send_btn_msg(vk_code, BTN_UP, 0);
                    }

                    if (widget->hold_cnt < btn_timeing.m_click_tim){
                        widget->click_cnt++;
                    } else {
                        widget->click_cnt = 0;
                        widget->vk_code = VK_CODE_NULL;
                    }

                    widget->hold_cnt = 0;

                    break;
                } 
            }
            break;

        case IO_K_EVENT_VK_DW:
            vk_code = kcode;
            widget = mo_get_vk_widget_by_code(vk_code);
            if (widget != NULL) {
                if (widget->vk_code == VK_CODE_NULL) {
                    widget->click_cnt = 0;
                    widget->hold_cnt = 0;
                }
                if (widget->vk_state != MO_VK_DW) {
                    widget->vk_state = MO_VK_DW;
                    widget->hold_cnt = 0;
                    send_btn_msg(vk_code, BTN_DOWN, 0);
                }
                widget->vk_code = vk_code;
            }
            break;

        default:
            break;
    }
}

/*!
  @brief init the button task, and register a client task to receive the input
  event by user.
  @param client_tsk: point to a client task that will be receiving the input
  event.
  @return   FALSE: failed
            TRUE: successfully
*/
u8 mcu_btn_init(btn_msg_cb_handle btn_msg_cb) {
    u8 ret = 1;

    io_key_task_init(io_k_event_cb_handle);
    btn_tsk.btn_msg_cb = btn_msg_cb;
    btn_tsk.vk_number = HW_k_NUMBER_;

    return ret;
}
