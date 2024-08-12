/**************************************************************************
 * @file usr_desplay_task.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/
#ifndef USR_DESPLAY_TASK__H_H
#define USR_DESPLAY_TASK__H_H
#include<stdint.h>
#include<stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
#include "mo_config.h"
#include "mcu_button.h"
#include "mo_input.h"
#include "mo_msg.h"
#include "lcd_init.h"
#include "lcd.h"

#define CURR_A " A"
#define CURR_mA "mA"
#define CURR_uA "uA"

#define POWER_W " W"
#define POWER_mW "mW"
#define POWER_uW "uW"

#define TABLE_START_Y 124
#define TABLE_START_X 0
#define ROW_DISTANCE 10
#define ROW_NUMBER 10
#define COLUMN_DES  25
#define COLUMN_NUM  10
#define INPUT_DAT_MAX (COLUMN_DES*COLUMN_NUM)
#define TABLE_HEIGHT (ROW_DISTANCE*ROW_NUMBER)
#define TABLE_WIDTH (COLUMN_DES*COLUMN_NUM)

#define OUT_CTRL_BLOCK_POSSITION_X 110
#define OUT_CTRL_BLOCK_POSSITION_Y 3
#define OUT_CTRL_B_WIDTH           86
#define OUT_CTRL_B_HEIGHT          40

#define OUT_CTRL_B_X_END (OUT_CTRL_BLOCK_POSSITION_X+OUT_CTRL_B_WIDTH)
#define OUT_CTRL_B_Y_END (OUT_CTRL_BLOCK_POSSITION_Y+OUT_CTRL_B_HEIGHT)

#define EN_UPLOAD_DAT_TO_APP 1

typedef struct point_t{
    mo_u16 x;
    mo_u16 y;
}PointT;

typedef struct table_infor{
    mo_u16 row_num;
    mo_u16 column_num;
    mo_u16 row_distance;
    mo_u16 column_distance;
}TableInforT;

typedef struct vol_scale_infor_{
    mo_u32 vol_dis_scale; /*Unit is mV*/
    char *p_Vunit_str;
}VolScaleInforT;

typedef struct cur_scale_infor_{
    mo_u32 curr_dis_scale;
    char *p_Cunit_str;
    mo_u32 curr_actual_scale;/*Unit is uA*/
}CurScaleInforT;

typedef struct usr_display_task
{
    u16 rec_fill_color;
    char *txt;
    u8 font_size;
    u16 txt_x;
    u16 txt_y;
    u16 txt_color;
}REC_STYLE_T;
typedef enum c_v_dis_type{
    CV_DIS_NONE=0,
    CV_DIS_EN,
    CV_DIS_DISABLE,
    CV_DIS_MAX___
}CV_dis_t;
const mo_task get_usr_display_task(void);
void usr_display_task_init(void);

void to_sync_all_cfg_to_app(mo_u8 in_req_vol, mo_u8 octl, mo_u32 out_cfg);

#ifdef __cplusplus
}
#endif

#endif //USR_DESPLAY_TASK__H_H


