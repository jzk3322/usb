/**
 * @file usr_main_app.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// #include "sc8721.h"
#include "ina226.h"
#include "usr_desplay_task.h"
#include "usr_debug.h"
#include "usr_flash.h"
#include "usr_main_app.h"

typedef struct usr_main_app_tsk {
    mo_task_data tsk;
    CFG_MODE_T cfg_mode;
    CfigDatsT cfg_dat;
    CfigDatsT curr_set;
    MonitorDatsT m_dat;
    PD_REQ_VOL_T pd_req_vol;
    OUT_CTRL_T out_ctrl;
    mo_u8 app_set_flg;
} UsrMainAppTskT;

static UsrMainAppTskT main_tsk;
static void set_out_voltage(mo_u16 vol);
static void set_out_current(mo_u16 curr);
static void usr_save_cfg_data_to_flash(void);

CfigDatsT main_app_get_cfg_dat(void){
    return main_tsk.cfg_dat;
}

CFG_MODE_T main_app_get_cfg_mode(void){
    return main_tsk.cfg_mode;
}


static void out_voltage_decrease_handle(u16 step){
    CfigDatsT *cdat;

    if(main_tsk.cfg_dat.out_vol>VOUT_MIN){
        
        if(main_tsk.cfg_dat.out_vol>step)main_tsk.cfg_dat.out_vol-=step;
        else main_tsk.cfg_dat.out_vol=VOUT_MIN;
        if(main_tsk.cfg_dat.out_vol<VOUT_MIN)main_tsk.cfg_dat.out_vol=VOUT_MIN;
        
        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
        }

        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_SYNC_OVOL_TO_HOST,cdat,0);
        }

        if(!main_tsk.out_ctrl || main_tsk.app_set_flg==0){
            set_out_voltage(main_tsk.cfg_dat.out_vol); 
            main_tsk.curr_set.out_vol = main_tsk.cfg_dat.out_vol;
        }
        
    }
}

static void out_voltage_increase_handle(u16 step) {
    CfigDatsT *cdat;

    if (main_tsk.cfg_dat.out_vol < VOUT_MAX) {
        main_tsk.cfg_dat.out_vol += step;
        if(main_tsk.cfg_dat.out_vol>VOUT_MAX)main_tsk.cfg_dat.out_vol=VOUT_MAX;

        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
        }

        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_SYNC_OVOL_TO_HOST,cdat,0);
        }

        if(!main_tsk.out_ctrl || main_tsk.app_set_flg==0){
            set_out_voltage(main_tsk.cfg_dat.out_vol); 
            main_tsk.curr_set.out_vol = main_tsk.cfg_dat.out_vol;
        }

    }

}

static void out_voltage_set_handle(mo_u32 ovl) {
    CfigDatsT *cdat;

    main_tsk.cfg_dat.out_vol = ovl;
    if(main_tsk.cfg_dat.out_vol>VOUT_MAX)main_tsk.cfg_dat.out_vol=VOUT_MAX;
    if(main_tsk.cfg_dat.out_vol<VOUT_MIN)main_tsk.cfg_dat.out_vol=VOUT_MIN;

    cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
    if(cdat){
        *cdat = main_tsk.cfg_dat;
        mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
    }

    if(!main_tsk.out_ctrl){
        set_out_voltage(main_tsk.cfg_dat.out_vol); 
        main_tsk.curr_set.out_vol = main_tsk.cfg_dat.out_vol;
    }else{
        main_tsk.app_set_flg=1;
    }
    
    mo_msg_cancel_all(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE);
    mo_msg_send(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE,NULL,3000);
}
static void out_current_set_handle(mo_u32 curr) {
    CfigDatsT *cdat;

    main_tsk.cfg_dat.limit_curr = curr;
    if(main_tsk.cfg_dat.limit_curr>OC_MAX)main_tsk.cfg_dat.limit_curr=OC_MAX;
    if(main_tsk.cfg_dat.limit_curr<OC_MIN)main_tsk.cfg_dat.limit_curr=OC_MIN;

    cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
    if(cdat){
        *cdat = main_tsk.cfg_dat;
        mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
    }

    //  set_out_voltage(main_tsk.cfg_dat.out_vol); 
    //  main_tsk.curr_set.out_vol = main_tsk.cfg_dat.out_vol;
    mo_msg_cancel_all(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE);
    mo_msg_send(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE,NULL,3000);
    
}

static void out_limit_curr_decrease_handle(u16 step){
    CfigDatsT *cdat;

    if(main_tsk.cfg_dat.limit_curr>OC_MIN){
        
        if(main_tsk.cfg_dat.limit_curr>step)main_tsk.cfg_dat.limit_curr-=step;
        else main_tsk.cfg_dat.limit_curr=OC_MIN;

        if(main_tsk.cfg_dat.limit_curr<OC_MIN)main_tsk.cfg_dat.limit_curr=OC_MIN;

        // sc8721_current_limit_cfg(main_tsk.cfg_dat.limit_curr);
        set_out_current(main_tsk.cfg_dat.limit_curr);

        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
        }
    }


}

static void out_limit_curr_increase_handle(u16 step) {
    CfigDatsT *cdat;
    if (main_tsk.cfg_dat.limit_curr < OC_MAX) {
        main_tsk.cfg_dat.limit_curr += step;
        if(main_tsk.cfg_dat.limit_curr>OC_MAX)main_tsk.cfg_dat.limit_curr=OC_MAX;

        // sc8721_current_limit_cfg(main_tsk.cfg_dat.limit_curr);
        set_out_current(main_tsk.cfg_dat.limit_curr);
        cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
        if(cdat){
            *cdat = main_tsk.cfg_dat;
            mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
        }
    }


}

static void cfg_increase_handle(u16 step){

    if(main_tsk.cfg_mode == VOL_CFG_M){
        out_voltage_increase_handle(step);
    }else if(main_tsk.cfg_mode == CURR_CFG_M){
        out_limit_curr_increase_handle(step);
    }else{
    }

    if(main_tsk.cfg_mode != CFG_MODE_NULL){
        mo_msg_cancel_all(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE);
        mo_msg_send(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE,NULL,5000);
    }
     mo_msg_cancel_all(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE);
    mo_msg_send(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE,NULL,3000);
}

static void cfg_decrease_handle(u16 step){

    if(main_tsk.cfg_mode == VOL_CFG_M){
        out_voltage_decrease_handle(step);
    }else if(main_tsk.cfg_mode == CURR_CFG_M){
        out_limit_curr_decrease_handle(step/10);
    }else{
    }  

    if(main_tsk.cfg_mode != CFG_MODE_NULL){
        mo_msg_cancel_all(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE);
        mo_msg_send(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE,NULL,5000);
    }
    
    mo_msg_cancel_all(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE);
    mo_msg_send(&main_tsk.tsk,USR_SAVE_CFG_DAT_TO_FLASHE,NULL,3000);
}

static void togle_power_out_handle(void){

    OUT_CTRL_T *sync_out_ctrl=NULL;
    main_tsk.out_ctrl = !main_tsk.out_ctrl;
    OUT_CTRL(main_tsk.out_ctrl);

    USR_DBG_INFO("OUT_CTRL:%d\r\n",main_tsk.out_ctrl);

    sync_out_ctrl = (OUT_CTRL_T*)mo_malloc(sizeof(OUT_CTRL_T));
    if(sync_out_ctrl){
        *sync_out_ctrl = main_tsk.out_ctrl;
        mo_msg_send(get_usr_display_task(),USR_LCD_POWER_OUT_UI_UPDATE,sync_out_ctrl,0);
    }
    // if(main_tsk.out_ctrl){
    //     set_out_voltage(DEFAULT_O_VOL);
    //     main_tsk.curr_set.out_vol = DEFAULT_O_VOL;
    //     mo_msg_cancel_all(get_usr_main_task(),USR_UPDATE_CFG_VOLTAGE_TO_IC);
    //     mo_msg_send(get_usr_main_task(),USR_UPDATE_CFG_VOLTAGE_TO_IC,NULL,25);
    // }
}

static void set_power_out_handle(OUT_CTRL_T en,MO_EVENTS_T notify_ev){

    OUT_CTRL_T *sync_out_ctrl=NULL;
    main_tsk.out_ctrl = en;
    OUT_CTRL(main_tsk.out_ctrl);

    sync_out_ctrl = (OUT_CTRL_T*)mo_malloc(sizeof(OUT_CTRL_T));
    if(sync_out_ctrl){
        *sync_out_ctrl = main_tsk.out_ctrl;
        mo_msg_send(get_usr_display_task(),notify_ev,sync_out_ctrl,0);
    }

}

static void key_event_handle(mo_key_event_msg_t *k_event){
    switch (k_event->vk_code){
        case HW_KEY_CENTER:
            if(k_event->vk_patt == BTN_SINGLE_CLICK){
                togle_power_out_handle();
            }else if(k_event->vk_patt == BTN_MULTI_CLICK && k_event->click_num == 2){
                if(main_tsk.cfg_mode == CFG_MODE_NULL || CURR_CFG_M==main_tsk.cfg_mode){
                    main_tsk.cfg_mode = VOL_CFG_M;
                }else{
                    main_tsk.cfg_mode = CURR_CFG_M;
                }
                //MO_LOG("v_out_en:%d\r\n",main_tsk.v_out_en);
                mo_msg_cancel_all(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE);
                mo_msg_send(&main_tsk.tsk,USR_CFG_TIMEOUT_HANDLE,NULL,5000);

                mo_msg_send(get_usr_display_task(),USR_LCD_CFG_UI_ACTIVE,0,0);
            }else if(k_event->vk_patt == BTN_MULTI_CLICK && k_event->click_num == 4){
                mo_msg_send(get_usr_display_task(),USR_LCD_TEST_MSG,0,0);
            }else if(k_event->vk_patt == BTN_MULTI_CLICK && k_event->click_num == 3){
              
            }else if(k_event->vk_patt == BTN_LP){
                // mo_msg_send(get_usr_display_task(),USR_LCD_MEASURE_VOL_TOGGLE,0,0);
            }else if(k_event->vk_patt == BTN_MULTI_CLICK && k_event->click_num == 5){
                // mo_msg_send(get_usr_display_task(),USR_LCD_TOGGLE_WEIXIN_MINI_PROGRAM_ICON,0,0);
                mo_msg_send(&main_tsk.tsk,USR_OUT_V_CALIBRATION,NULL,0);
            }
            break;
        
        default:
            break;
    }
}
#define MAX_BUF_SIZE__ 128
mo_s8 usr_mem_cpy(mo_u8 *start_add,mo_u8 *end_add, mo_u8 *p_get_dat){
    uint16_t max_dat=0;

    if(start_add==NULL || end_add==NULL || p_get_dat==NULL)return -1;

    while(&start_add[max_dat] != end_add && max_dat<MAX_BUF_SIZE__){
        p_get_dat[max_dat]=start_add[max_dat];
        max_dat++;
    }

    p_get_dat[max_dat]='\0';

    return 0;

}
#define MAX_POINT_LEN  3
const mo_u32 _10_pw_table[]={
    1ul,/*10^0*/
    10ul,/*10^1*/
    100ul,/*10^2*/
    1000ul,/*10^3*/
    10000ul,/*10^4*/
    100000ul,/*10^5*/
    1000000ul,/*10^6*/
    10000000ul,/*10^7*/
    100000000ul,/*10^8*/
    1000000000ul,/*10^9*/
};

int str_vol_to_s32(mo_u8 *p_str,mo_s32 *int_dat){
    mo_u8 *p_start=p_str;
    mo_u8 *p_point;
    mo_u8 int_bits=0,point_bits=0;
    int i;
    mo_s32 tmp_dat=0;

    if(p_str == NULL || int_dat==NULL)return -1;

    p_point = (strpbrk(p_str, "."));

    if(p_point){
        p_point[0]='\0';
        p_point++;
        int_bits=strlen(p_str);
        point_bits = strlen(p_point);
        if(point_bits>MAX_POINT_LEN)point_bits=MAX_POINT_LEN;

        // MO_LOG("point_bits:%d\r\n",point_bits);
    }else{
        int_bits=strlen(p_str);
    }

    for(i=int_bits; i>0; i--){
        tmp_dat += (p_str[i-1]-'0')*_10_pw_table[int_bits-i];
    }
    *int_dat=tmp_dat*1000ul;
    tmp_dat=0;
    for(i=0; i<point_bits; i++){
        tmp_dat += ((p_point[i]-'0')*_10_pw_table[(MAX_POINT_LEN-(i+1))]);
    }
    *int_dat+=tmp_dat;

    // MO_LOG("dec vol:%d,%d,%d\r\n",*int_dat,point_bits,tmp_dat);

    return 0;
}

static void uart_rx_msg_handle(mo_u8 *p_dat){
    mo_u8 buf[MAX_BUF_SIZE__+1];
    mo_u8 dat_buf[MAX_BUF_SIZE__+1];
    mo_u8 max_cmd=30;
    mo_u8 *p_in_dat=p_dat;
    mo_u8 *p_cmd_end;
    mo_u8 *p_dat_end;
    
    // MO_LOG("rec,%s\r\n",p_dat);

    while (max_cmd--){

        p_cmd_end = (strpbrk(p_in_dat, "="));
        if(p_cmd_end==NULL)break;
        p_dat_end = (strpbrk(p_in_dat, "@"));
        if(p_dat_end==NULL)break;

        usr_mem_cpy(p_in_dat,p_cmd_end,buf);
        usr_mem_cpy(&p_cmd_end[1],p_dat_end,dat_buf);
       // MO_LOG("decode:%s,%s\r\n",buf,dat_buf);
        if(strcmp("i_cfg",buf)==0){
           
            // mo_u8 *p_vol;
            // mo_u8 req_vol;
            // req_vol = set_pd_req_vol_handle(dat_buf[0]-'0');
            
            // p_vol = (mo_u8*)mo_malloc(sizeof(mo_u8));
            // if(p_vol){
            //     *p_vol = req_vol;
            //     mo_msg_send(get_usr_display_task(),USR_LCD_PD_REQ_UI_UPDATE,p_vol,0);
            // }
        }else if(strcmp("o_cfg",buf)==0){
            mo_s32 int_dat;
            // MO_LOG("dat:%s\r\n",dat_buf);
           if(str_vol_to_s32(dat_buf,&int_dat) != -1) {
                out_voltage_set_handle(int_dat);
           }
        }else if(strcmp("sync_cfg",buf)==0){
            // to_sync_all_cfg_to_app(set_pd_req_vol_handle(main_tsk.pd_req_vol),main_tsk.out_ctrl,main_tsk.cfg_dat.out_vol);
        }else if(strcmp("o_ctr",buf)==0){
            set_power_out_handle((OUT_CTRL_T) (dat_buf[0]-'0'),USR_LCD_POWER_OUT_UI_UPDATE_BY_APP);
        }
        else{
        //    MO_LOG("not support this cmd:%s,%s\r\n",buf,dat_buf); 
        }
        
        p_in_dat = (p_dat_end+1);
        if(*p_in_dat=='\n')break;
        
    }
    
}

/**
 * Vout=Vout_set*(1/6+5/6*D)
 * Vout/Vout_se-1/6=5D/6
 * Vout*6/Vout_set=5D
 * (Vout*6/Vout_set-1)/5=D
 */
u8 vout_up=0;

static void set_out_voltage(mo_u16 vol){

    mo_u32 pwm_duty_cycle;
    mo_u32 vout_max=main_tsk.cfg_dat.l_vmax_set;
    mo_u8 fb_ctl=0;
    mo_u32 wait_pwm=0;

    if(vol > main_tsk.cfg_dat.l_vmax_set){
        
        if(vol >= HEIGHT_CALIBRATION_VOL){
            vout_max = main_tsk.cfg_dat.h_vmax_set;
        }else{
            vout_max = main_tsk.cfg_dat.m_vmax_set;
        }
        if(vout_up==0){
            vout_up = 1;
            wait_pwm=1;
        }
        fb_ctl= 1;
    }else if(vout_up){
        vout_up=0;
    }
    pwm_duty_cycle = ((vol*6*PWM_MAX)/vout_max-PWM_MAX)/5;

    USR_DBG_DBUG("V:%d,pwm:%d \r\n",vol,pwm_duty_cycle); 
    
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, pwm_duty_cycle); 
    if(wait_pwm){
        // while (wait_pwm)
        // {wait_pwm--;}
        MO_DELAY_TICK(2);
    }
    
    
    gpio_bits_write(GPIOA,GPIO_PINS_4,fb_ctl);
    


}

// static void set_out_voltage(uint16_t duty)
static void set_out_current(mo_u16 curr){

    /*ILIMx=ILIMx_SET×D*/
   mo_u32 pwm_duty_cycle = (curr*PWM_MAX)/I_SET_MAX;
   tmr_channel_value_set(TMR15, TMR_SELECT_CHANNEL_2, pwm_duty_cycle);  
   USR_DBG_DBUG("curr:%d,pwm:%d\r\n",curr,pwm_duty_cycle);
}

#define CAL_TOLERANCE 13u// tolerance
static int out_voltage_calibration_handle(void){
    CfigDatsT cfg_datas;
    int ret=-1;
    int flash_w=0;
    int vbus_vol;
    int try_cnt=L_CAL_TRY_TIMES;
    CfigDatsT *cdat;
    mo_u16 l_vmax_set = main_tsk.cfg_dat.l_vmax_set;
    mo_u16 h_vmax_set = main_tsk.cfg_dat.h_vmax_set;
    mo_u16 m_vmax_set = main_tsk.cfg_dat.m_vmax_set;
    mo_u16 h_t;
    // usr_flash_write(TEST_FLASH_ADDRESS_START,&main_tsk.cfg_dat,sizeof(main_tsk.cfg_dat)/sizeof(mo_u16));
    // flash_read(TEST_FLASH_ADDRESS_START,&cfg_datas,sizeof(cfg_datas)/sizeof(mo_u16));
    // OUT_CTRL(0);
    // MO_DELAY_TICK(100);
    // set_out_voltage(LOW_CALIBRATION_VOL);
    // MO_DELAY_TICK(100);

    /**low fild voltage calibration handle */
    OUT_CTRL_T *sync_out_ctrl;

    main_tsk.out_ctrl=1;
    OUT_CTRL(main_tsk.out_ctrl);
    // sync_out_ctrl = (OUT_CTRL_T*)mo_malloc(sizeof(OUT_CTRL_T));
    // if(sync_out_ctrl){
    //     *sync_out_ctrl = main_tsk.out_ctrl;
    //     mo_msg_send(get_usr_display_task(),USR_LCD_POWER_OUT_UI_UPDATE,sync_out_ctrl,0);
    // }
    
    while(try_cnt--){
        set_out_voltage(LOW_CALIBRATION_VOL);
        MO_DELAY_TICK(200);
        if(ina226_read_bus_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
            h_t = LOW_CALIBRATION_VOL+CAL_TOLERANCE;
            if(vbus_vol < (h_t) && vbus_vol > (LOW_CALIBRATION_VOL)){
                ret = 0;
                break;
            }else{
                if(vbus_vol > h_t){
                    main_tsk.cfg_dat.l_vmax_set += LOW_CAL_STEP;
                }else{
                    main_tsk.cfg_dat.l_vmax_set -= LOW_CAL_STEP; 
                }
            }
        }else{
            break;
        }
    }

    if(ret != -1){
        flash_w=1;
        USR_DBG_INFO("l_vmax_set succeffull\r\n");
        ret = -1;
    }else{
        main_tsk.cfg_dat.l_vmax_set = l_vmax_set;
        USR_DBG_INFO("l_vmax_set failed\r\n");
    }
/*******************************middle field voltage calibration handle *************************************/
    // OUT_CTRL(0);
    // MO_DELAY_TICK(100);
    // set_out_voltage(MIDDLE_CALIBRATION_VOL);
    // MO_DELAY_TICK(50);
    // OUT_CTRL(1);
    try_cnt = H_CAL_TRY_TIMES;
     while(try_cnt--){
        set_out_voltage(MIDDLE_CALIBRATION_VOL);
        MO_DELAY_TICK(200);
        if(ina226_read_bus_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
            h_t = MIDDLE_CALIBRATION_VOL+20+10;
            if(vbus_vol < (h_t) && vbus_vol > (MIDDLE_CALIBRATION_VOL+20)){
                ret=0;
                break;
            }else{
                if(vbus_vol > h_t){
                    main_tsk.cfg_dat.m_vmax_set += MIDDLE_CAL_STEP;
                }else{
                    main_tsk.cfg_dat.m_vmax_set -= MIDDLE_CAL_STEP; 
                }
            }
        }else{
            break;
        }
    }
    // OUT_CTRL(main_tsk.out_ctrl);
    if(ret != -1){
        flash_w=1;
        USR_DBG_INFO("m_vmax_set succeffull\r\n");
    }else{
        main_tsk.cfg_dat.m_vmax_set = m_vmax_set;
        USR_DBG_INFO("m_vmax_set failed\r\n");
    }

    /*******************************height field voltage calibration handle *************************************/
    // OUT_CTRL(0);
    // MO_DELAY_TICK(100);
    // set_out_voltage(HEIGHT_CALIBRATION_VOL);
    // MO_DELAY_TICK(50);
    // OUT_CTRL(1);

    try_cnt = H_CAL_TRY_TIMES;
    main_tsk.cfg_dat.out_vol=HEIGHT_CALIBRATION_VOL;
     while(try_cnt--){
        set_out_voltage(HEIGHT_CALIBRATION_VOL);
        MO_DELAY_TICK(500);
        if(ina226_read_bus_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
            h_t = HEIGHT_CALIBRATION_VOL+75+10;
            if(vbus_vol < (h_t) && vbus_vol > (HEIGHT_CALIBRATION_VOL+75)){
                ret=0;
                break;
            }else{
                if(vbus_vol > h_t){
                    main_tsk.cfg_dat.h_vmax_set += HEIGHT_CAL_STEP;
                }else{
                    main_tsk.cfg_dat.h_vmax_set -= HEIGHT_CAL_STEP; 
                }
            }
        }else{
            break;
        }
    }
    // OUT_CTRL(main_tsk.out_ctrl);
    if(ret != -1){
        flash_w=1;
        USR_DBG_INFO("h_vmax_set succeffull\r\n");
    }else{
        main_tsk.cfg_dat.h_vmax_set = h_vmax_set;
        USR_DBG_INFO("h_vmax_set failed\r\n");
    }

    
    main_tsk.out_ctrl=0;
    OUT_CTRL(main_tsk.out_ctrl);
    sync_out_ctrl = (OUT_CTRL_T*)mo_malloc(sizeof(OUT_CTRL_T));
    if(sync_out_ctrl){
        *sync_out_ctrl = main_tsk.out_ctrl;
        mo_msg_send(get_usr_display_task(),USR_LCD_POWER_OUT_UI_UPDATE,sync_out_ctrl,0);
    }

    if(flash_w)usr_save_cfg_data_to_flash();

    cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));

    if(cdat){
        *cdat = main_tsk.cfg_dat;
        mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
    }

    return ret;

}


static void usr_save_cfg_data_to_flash(void){
    CfigDatsT cfg_datas;
    usr_flash_write(TEST_FLASH_ADDRESS_START,&main_tsk.cfg_dat,sizeof(main_tsk.cfg_dat)/sizeof(mo_u16));
    // flash_read(TEST_FLASH_ADDRESS_START,&cfg_datas,sizeof(cfg_datas)/sizeof(mo_u16));
    USR_DBG_INFO("h_vmax_set:%d \r\n m_vmax_set:%d\r\n l_vmax_set:%d\r\n limit_curr:%d\r\n out_vol:%d\r\n",\
        main_tsk.cfg_dat.h_vmax_set,main_tsk.cfg_dat.m_vmax_set,main_tsk.cfg_dat.l_vmax_set,main_tsk.cfg_dat.limit_curr,main_tsk.cfg_dat.out_vol
    );
}



static void usr_cfg_dat_init(void){

    CfigDatsT cfg_datas;

    flash_read(TEST_FLASH_ADDRESS_START,&cfg_datas,sizeof(cfg_datas)/sizeof(mo_u16));
    main_tsk.cfg_dat = cfg_datas;

    if(main_tsk.cfg_dat.h_vmax_set > (VOUT_MAX_SET+2000) || \
        main_tsk.cfg_dat.h_vmax_set < (VOUT_MAX_SET-2000)){

        main_tsk.cfg_dat.h_vmax_set = VOUT_MAX_SET;    //default value
    }

    if(main_tsk.cfg_dat.m_vmax_set > (VOUT_MAX_SET+2000) || \
        main_tsk.cfg_dat.m_vmax_set < (VOUT_MAX_SET-2000)){

        main_tsk.cfg_dat.m_vmax_set = VOUT_MAX_SET;    //default value
    }
    if(main_tsk.cfg_dat.l_vmax_set >(VOUT_MIMDU_SET+2000) || \
        main_tsk.cfg_dat.l_vmax_set < (VOUT_MIMDU_SET-2000)){

        main_tsk.cfg_dat.l_vmax_set = VOUT_MIMDU_SET;    //default value
    }

    if(main_tsk.cfg_dat.limit_curr > OC_MAX || main_tsk.cfg_dat.limit_curr < OC_MIN){
        main_tsk.cfg_dat.limit_curr = DEFAULT_OC_LIMIT; //default value
    }

    if(main_tsk.cfg_dat.out_vol > VOUT_MAX || main_tsk.cfg_dat.out_vol < VOUT_MIN){
        main_tsk.cfg_dat.out_vol = DEFAULT_OC_LIMIT; //default value
    }


}

static void main_task_handle(mo_task tsk, mo_msg_id msg_id, mo_msg msg) {
    

    switch (msg_id) {
        case SYS_KEY_EVENT: {
            if(msg!=NULL){
                mo_key_event_msg_t *k_event = (mo_key_event_msg_t *)msg;
                // MO_LOG("vk_code:%d,vk_patt:%d,click:%d\r\n", k_event->vk_code, k_event->vk_patt, k_event->click_num);
                key_event_handle(k_event);
            }
        } break;
        case USR_CFG_UP_CONTINUOUS:{
            cfg_increase_handle(50);
            mo_msg_cancel_all(&main_tsk.tsk,USR_CFG_UP_CONTINUOUS);
            mo_msg_send(&main_tsk.tsk,USR_CFG_UP_CONTINUOUS,NULL,400);
        }break;
        case USR_CFG_DW_CONTINUOUS:{
            cfg_decrease_handle(50);
            mo_msg_cancel_all(&main_tsk.tsk,USR_CFG_DW_CONTINUOUS);
            mo_msg_send(&main_tsk.tsk,USR_CFG_DW_CONTINUOUS,NULL,400);
        }break;
        case USR_EVENT_TEST_POLLING:{
            int vbus_vol;
            u8 buf[6];
            MonitorDatsT *p_dat;

            if(ina226_read_bus_voltage(ina226_INPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
                main_tsk.m_dat.in_vol = vbus_vol;
                // USR_DBG_INFO("in_vol:%d,%d\r\n",(vbus_vol/1000),(vbus_vol%1000));
            }else{
                // USR_DBG_INFO("vbus_vol error\r\n");
            }
            if(ina226_read_shunt_voltage(ina226_INPUT_I2C_DEV_ADD,ina226_CH1_SHUNT_V_REG,&vbus_vol)==0){
                main_tsk.m_dat.in_curr = vbus_vol*10;
            }else{
            }

            if(ina226_read_bus_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
                main_tsk.m_dat.out_vol = vbus_vol;
                // USR_DBG_INFO("out_vol:%d,%d\r\n",(vbus_vol/1000),(vbus_vol%1000));
            }else{
            }
            if(ina226_read_shunt_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_SHUNT_V_REG,&vbus_vol)==0){
                main_tsk.m_dat.out_curr = vbus_vol*10;
            }else{
            }
           

            mo_msg_send(&main_tsk.tsk,USR_EVENT_TEST_POLLING,NULL,100); 

            p_dat = (MonitorDatsT*)mo_malloc(sizeof(MonitorDatsT));
            if(p_dat){
                *p_dat = main_tsk.m_dat;
                mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_MONITOR_DAT,p_dat,0);
            }
            
        }
            break;
        case USR_EVENT_DELAY_TO_INIT:{
                CfigDatsT *cdat;
                mo_u8 *p_vol;
                mo_u8 req_vol;
                
//                get_sc8721_init();
                // ina3221_init();
                ina226_init(ina226_INPUT_I2C_DEV_ADD);
                ina226_init(ina226_OUTPUT_I2C_DEV_ADD);
                
                cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
                if(cdat){
                    *cdat = main_tsk.cfg_dat;
                    mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
                }
                mo_msg_send(tsk,USR_UPDATE_CFG_VOLTAGE_TO_IC,0,0);

                
                // req_vol = pd_req_vol_inc_handle();

                // p_vol = (mo_u8*)mo_malloc(sizeof(mo_u8));
                // if(p_vol){
                //     *p_vol = req_vol;
                //     mo_msg_send(get_usr_display_task(),USR_LCD_PD_REQ_UI_UPDATE,p_vol,0);
                // }
//                sc8721_out_vol_cfg(main_tsk.curr_set.out_vol);
            }
                break;

        case USR_UPDATE_CFG_VOLTAGE_TO_IC:{
            #if 1
            if(main_tsk.curr_set.out_vol != main_tsk.cfg_dat.out_vol){
                if(main_tsk.curr_set.out_vol < main_tsk.cfg_dat.out_vol){
                    main_tsk.curr_set.out_vol+=100; /*mV*/
                    if(main_tsk.curr_set.out_vol>main_tsk.cfg_dat.out_vol)main_tsk.curr_set.out_vol=main_tsk.cfg_dat.out_vol;
                }else{
                     main_tsk.curr_set.out_vol-=100; /*mV*/
                    if(main_tsk.curr_set.out_vol<main_tsk.cfg_dat.out_vol)main_tsk.curr_set.out_vol=main_tsk.cfg_dat.out_vol;
                }
//                sc8721_out_vol_cfg(main_tsk.curr_set.out_vol);
              set_out_voltage(main_tsk.curr_set.out_vol);  
            //   set_out_current(main_tsk.curr_set.out_vol);
                
            }else{
                //mo_msg_cancel_all(tsk,USR_UPDATE_CFG_VOLTAGE_TO_IC);
                main_tsk.app_set_flg=0;
            }
            mo_msg_cancel_all(tsk,USR_UPDATE_CFG_VOLTAGE_TO_IC);
            mo_msg_send(tsk,USR_UPDATE_CFG_VOLTAGE_TO_IC,NULL,20);
            #endif
        }
            break;
        case USR_CFG_TIMEOUT_HANDLE:
            main_tsk.cfg_mode = CFG_MODE_NULL;
            mo_msg_send(get_usr_display_task(),USR_LCD_CFG_UI_INACTIVE,0,0);
            break;
        case USR_UART_RX_RECEIVED_DATAS:
            if(msg!=NULL){
                uart_rx_msg_handle(msg);
            }
            break;
        case USR_INC_50_MSG:
            cfg_increase_handle(50);
            break;
        case USR_INC_100_MSG:
            cfg_increase_handle(100);
            break;
        case USR_INC_1000_MSG:
            cfg_increase_handle(1000);
            break;
        case USR_DEC_50_MSG:
            cfg_decrease_handle(50);
            break;
        case USR_DEC_100_MSG:
            cfg_decrease_handle(100);
            break;
        case USR_DEC_1000_MSG:
            cfg_decrease_handle(1000);
            break;
        case USR_OUT_V_CALIBRATION:
            out_voltage_calibration_handle();
            break;

        case USR_SAVE_CFG_DAT_TO_FLASHE:
            usr_save_cfg_data_to_flash();
            break;
        default:
            break;
    }
}


mo_task get_usr_main_task(void){
    return &main_tsk.tsk;
}


void usr_main_app_task_init(void){

    memset(&main_tsk,0,sizeof(main_tsk));
    main_tsk.tsk.handler = main_task_handle;

    usr_cfg_dat_init();

    main_tsk.curr_set = main_tsk.cfg_dat;

    main_tsk.pd_req_vol = PD_REQ_9V;
    
   // mo_msg_cancel_all(&main_tsk.tsk,USR_EVENT_TEST_POLLING);
    mo_msg_send(&main_tsk.tsk,USR_EVENT_TEST_POLLING,NULL,70);

    // gpio_bits_set(GPIOA, GPIO_PINS_6);
    set_out_current(main_tsk.cfg_dat.limit_curr);
    set_out_voltage(main_tsk.cfg_dat.out_vol);

    mo_msg_send(&main_tsk.tsk,USR_EVENT_DELAY_TO_INIT,NULL,60);
    
}
