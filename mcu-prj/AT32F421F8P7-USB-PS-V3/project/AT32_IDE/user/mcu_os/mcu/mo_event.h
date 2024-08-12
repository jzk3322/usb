/**
 * @file mo_event.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef __SYS_EVENTS_H__H
#define __SYS_EVENTS_H__H
#ifdef __cplusplus
extern "C" {
#endif
// #define USR_EVENT_BASED         0X4000
// #define SYS_EVENT_BASED         0X8000

typedef enum {

    EVENT_INVALID = 0,
    /*****user event start from 0x4000***/
    USR_EVENT_BASED,
    USR_EVENT_MAIN_VOL_UP = USR_EVENT_BASED,
    USR_EVENT_MAIN_VOL_DW,
    USR_EVENT_POWER_ON,
    USR_EVENT_POWER_OFF,
    USR_EVENT_END,
    USR_EVENT_PRINTF_TEST,
    USR_EVENT_IO_KEY_POLLING,
    USR_EVENT_BT_MSG_RX_POLLING,
    USR_EVENT_BT_NEXT,
    USR_EVENT_BT_PREV,
    USR_EVENT_PLAY_PAUSE,
    USR_EVENT_BT_VOL_UP,
    USR_EVENT_BT_VOL_DW,
    USR_EVENT_VOL_UP_HOLD,
    USR_EVENT_VOL_DW_HOLD,
    USR_EVENT_BT_PAIRING,
    USR_DELAY_INIT_AMP,
    USR_SWITCH_NEXT_SOURCE,
    USR_BT_STATUS_CHANGE,
    USR_SWITCH_TO_DEFAULT_SOURCE,
    USR_SWITCH_TO_AUX,
    USR_SWITCH_TO_USB,
    USR_SWITCH_TO_BT,

    USR_DISPLAY_CHARGE_CURR,
    USR_DIS_CHARGE_TIMEOUT,
    USR_READ_TX_SINK_PDOS,
    USR_EVENT_TEST_POLLING,
    USR_EVENT_DELAY_TO_INIT,
    USR_CFG_UP_CONTINUOUS,
    USR_CFG_DW_CONTINUOUS,
    USR_CFG_TIMEOUT_HANDLE,

    USR_LCD_INIT_START,
    USR_LCD_UPDATE_MONITOR_DAT,
    USR_LCD_UPDATE_OUT_CFG_DAT,
    USR_LCD_CFG_UI_ACTIVE,
    USR_LCD_CFG_UI_INACTIVE,
    USR_LCD_CFG_UI_ACTIVE_UPDATE,
    USR_LCD_TEST_MSG,
    USR_LCD_PD_REQ_UI_UPDATE,
    USR_LCD_POWER_OUT_UI_UPDATE,
    USR_LCD_POWER_OUT_UI_UPDATE_BY_APP,
    USR_LCD_TOGGLE_WEIXIN_MINI_PROGRAM_ICON,
    USR_LCD_MEASURE_VOL_TOGGLE,
    USR_UART_RX_RECEIVED_DATAS,
    USR_UPDATE_CFG_VOLTAGE_TO_IC,
    USR_SYNC_PD_REQ_VOL_TO_HOST,
    USR_SYNC_OVOL_TO_HOST,
    USR_HOST_CONNECTED_TO_SYNC_DATS,
    USR_ROTATE_ENCODER_CW,
    USR_ROTATE_ENCODER_CCW,
    USR_ROTATE_INT_TOGGLE,

    USR_INC_50_MSG,
    USR_INC_100_MSG,
    USR_INC_1000_MSG,
    USR_DEC_50_MSG,
    USR_DEC_100_MSG,
    USR_DEC_1000_MSG,

    USR_REC_STEP_1000_TIMEOUT,
    USR_REC_STEP_100_TIMEOUT,
    USR_REC_STEP_50_TIMEOUT,

    USR_REC_STEP_DEC_1000_TIMEOUT,
    USR_REC_STEP_DEC_100_TIMEOUT,
    USR_REC_STEP_DEC_50_TIMEOUT,

    /*****user event end******************************/

    /*****System event start from 0x8000***/
    SYS_EVENT_BASED,
    SYS_EVENT_KEY_POLLING = SYS_EVENT_BASED,
    SYS_EVENT_KEY_LOW_LEVEL_POLLING,
    SYS_EVENT_LED_REFRESH,
    SYS_EVENT_READ_CHARGE_STATUS,
    
    SYS_EVENT_BUTTON_LOOP,
    SYS_KEY_EVENT,

    SYS_EVENT_END,
    /*****System event end******************************/

    /********************factory test event***********************/
    TEST_EVENT_BASED,
    TEST_TRIGGER_START = TEST_EVENT_BASED,
    TEST_EVENT_END,
    /**********************factory test event end**************************/

    MO_EVENT_MAX

} MO_EVENTS_T;

#ifdef __cplusplus
}
#endif

#endif
