/**
 * @file usr_main_app.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef USR_MAIN_APP_HHHHhhh
#define USR_MAIN_APP_HHHHhhh

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "mo_config.h"
#include "mcu_button.h"
#include "mo_input.h"
#include "mo_msg.h"
#include "usr_input_task.h"


#define VOUT_MAX         34000ul /*mV*/
#define VOUT_MIN         2000ul  /*mV*/
#define DEFAULT_O_VOL    3300ul  /*mV*/
#define DEFAULT_OC_LIMIT 8000ul  /*mA*/
#define OC_MAX           8000ul  /*mA*/
#define OC_MIN           10ul   /*mA*/

// #define PD_CFG1_PORT           GPIOA
// #define PD_CFG2_PORT           GPIOA
// #define PD_CFG3_PORT           GPIOA
// #define PD_CGF1_PIN            GPIO_PINS_10
// #define PD_CGF2_PIN            GPIO_PINS_9
// #define PD_CGF3_PIN            GPIO_PINS_8
#define ROTATE_ENCODER_B_POART GPIOA
#define ROTATE_ENCODER_B_PIN   GPIO_PINS_1

// #define CFG_PD_SET(cfg1,cfg2,cfg3) {\
//     gpio_bits_write(PD_CFG1_PORT, PD_CGF1_PIN, cfg1);\
//     gpio_bits_write(PD_CFG2_PORT, PD_CGF2_PIN, cfg2);\
//     gpio_bits_write(PD_CFG3_PORT, PD_CGF3_PIN, cfg3);\
// }
#define LOW_CALIBRATION_VOL  3300ul /*5.00V*/
#define HEIGHT_CALIBRATION_VOL 34000ul /*28.00V*/
#define MIDDLE_CALIBRATION_VOL 15000UL//17000ul /*17.00V*/

#define OUT_VOL_SW_THRESHOLD    10000ul/*mV*/
#define OUT_VOL_SW_M_THRESHOLD  20000ul/*mV*/

#define LOW_CAL_STEP     10ul
#define HEIGHT_CAL_STEP  10ul
#define MIDDLE_CAL_STEP  10ul

#define L_CAL_TRY_TIMES  120ul
#define H_CAL_TRY_TIMES  120ul


#define SC8701_CE_CTRL_PORT GPIOA
#define SC8701_CE_CTRL_PIN  GPIO_PINS_14
#define SC8701_CE_CTRL(en)  gpio_bits_write(SC8701_CE_CTRL_PORT, SC8701_CE_CTRL_PIN, (confirm_state)(!en))

#define MOS_CTL_PORT GPIOA
#define MOS_CTL_PIN  GPIO_PINS_4
#define MOS_CTL(en)   gpio_bits_write(MOS_CTL_PORT,MOS_CTL_PIN,(confirm_state)!en)

typedef struct monitor_datas_{
    mo_u32 in_vol;
    mo_u32 in_curr;
    mo_u32 out_vol;
    mo_u32 out_curr;
    mo_s32 measured_in_vl;
}MonitorDatsT;

typedef struct out__vc_datas_{
    mo_u32 out_vol;
    mo_u32 out_curr;
}OutVolCurrT;

typedef enum {
    CFG_MODE_NULL=0,

    VOL_CFG_M,
    CURR_CFG_M,

    CFG_MODE_MAX__
}CFG_MODE_T;

typedef enum {
    PD_REQ_5V,
    PD_REQ_9V,
    PD_REQ_12V,
    PD_REQ_15V,
    PD_REQ_20V,
    PD_REQ_MAX___
}PD_REQ_VOL_T;

typedef enum{
    OUT_CTRL_DISABLE=0,
    OUT_CTRL_ENABLE=1,
    
}OUT_CTRL_T;
typedef struct config_datas_{
    mo_u16 out_vol;
    mo_u16 limit_curr;/*mA*/
    // float pb1;
    // float pb2;
    mo_s16 out_vol_offset;
    mo_s16 out_vol_h_offset;
    mo_s16 out_vol_m_offset;
    mo_u16 out_dac_value;
}CfigDatsT;


mo_task get_usr_main_task(void);
void usr_main_app_task_init(void);

CFG_MODE_T main_app_get_cfg_mode(void);
CfigDatsT main_app_get_cfg_dat(void);


#ifdef __cplusplus
}
#endif


#endif
