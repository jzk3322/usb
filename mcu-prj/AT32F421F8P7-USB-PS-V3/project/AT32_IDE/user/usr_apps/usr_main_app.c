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
#include "MCP4728.h"
#include "usr_main_app.h"

#define FB_CTL_DAC_CH         DAC_CH_A
#define CURR_CTL_DAC_CH       DAC_CH_B
#define FB_CTL_DAC_CH_INDEX   0
#define CURR_CTL_DAC_CH_INDEX 1

#define SC8701_OUT_READY  2
#define SC8701_FIRST_OUT  1 

typedef struct usr_main_app_tsk {
    mo_task_data tsk;
    CFG_MODE_T cfg_mode;
    CfigDatsT cfg_dat;
    CfigDatsT curr_set;
    MonitorDatsT m_dat;
    PD_REQ_VOL_T pd_req_vol;
    OUT_CTRL_T out_ctrl;
    mo_u8 sc_8701_en;
    // mo_u8 app_set_flg;
    DAC_CFG_D_SET_T dac_cfg_sets[USR_USED_CH_MAX];
} UsrMainAppTskT;

static UsrMainAppTskT main_tsk;
static void set_out_voltage(mo_u16 vol,mo_u8 update_flg);
static void set_out_current(mo_u16 curr);
static void usr_save_cfg_data_to_flash(void);

CfigDatsT main_app_get_cfg_dat(void){
    return main_tsk.cfg_dat;
}

CFG_MODE_T main_app_get_cfg_mode(void){
    return main_tsk.cfg_mode;
}

// #define FB_CTRL_(v) gpio_bits_write(GPIOA,GPIO_PINS_4,v)

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

        set_out_voltage(main_tsk.cfg_dat.out_vol,0);
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

        set_out_voltage(main_tsk.cfg_dat.out_vol,0);
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

    set_out_voltage(main_tsk.cfg_dat.out_vol,0);
    
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

    set_out_current(main_tsk.cfg_dat.limit_curr);
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
        out_limit_curr_increase_handle(step/10);
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

static void power_out_set(OUT_CTRL_T out_ctrl){
    MOS_CTL(out_ctrl);
}

static void togle_power_out_handle(void){
    #define NEED_SLOW_INC_VOL 10000ul
    OUT_CTRL_T *sync_out_ctrl=NULL;

    if(main_tsk.sc_8701_en != SC8701_OUT_READY)return;

    main_tsk.out_ctrl = !main_tsk.out_ctrl;
    power_out_set(main_tsk.out_ctrl);
    USR_DBG_INFO("OUT_CTRL:%d\r\n",main_tsk.out_ctrl);

    sync_out_ctrl = (OUT_CTRL_T*)mo_malloc(sizeof(OUT_CTRL_T));
    if(sync_out_ctrl){
        *sync_out_ctrl = main_tsk.out_ctrl;
        mo_msg_send(get_usr_display_task(),USR_LCD_POWER_OUT_UI_UPDATE,sync_out_ctrl,0);
    }

}

static void set_power_out_handle(OUT_CTRL_T en,MO_EVENTS_T notify_ev){

    OUT_CTRL_T *sync_out_ctrl=NULL;
    main_tsk.out_ctrl = en;
    power_out_set(main_tsk.out_ctrl);

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
           if(str_vol_to_s32(dat_buf,&int_dat) != -1) {
                out_voltage_set_handle(int_dat);
           }
        }else if(strcmp("sync_cfg",buf)==0){
            // to_sync_all_cfg_to_app(set_pd_req_vol_handle(main_tsk.pd_req_vol),main_tsk.out_ctrl,main_tsk.cfg_dat.out_vol);
        }else if(strcmp("o_ctr",buf)==0){
            set_power_out_handle((OUT_CTRL_T) (dat_buf[0]-'0'),USR_LCD_POWER_OUT_UI_UPDATE_BY_APP);
        }
        else{
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

#define SC8701_Vref 1220ul /*1.22V*/
#define Pb1_ 0.02963465f
// #define Pb1_ 0.029f
#define Pb2_ 0.56989711f
// #define Pb2_ 0.5781f

static void set_out_voltage(mo_u16 vol,mo_u8 update_flg){
    mo_u32 m_voltage,tmp;
    
    if(vol <= OUT_VOL_SW_THRESHOLD){
        vol+=main_tsk.cfg_dat.out_vol_offset;
    }else if(vol <= OUT_VOL_SW_M_THRESHOLD){
        vol+=main_tsk.cfg_dat.out_vol_m_offset;
    }else{
        vol+=main_tsk.cfg_dat.out_vol_h_offset;
    }
    

    m_voltage = (SC8701_Vref-vol*Pb1_)/Pb2_;
    tmp = m_voltage;
    
    m_voltage=m_voltage*(DAC_RESOLUTION/INTERNAL_VREF);
    main_tsk.cfg_dat.out_dac_value = m_voltage;
    main_tsk.dac_cfg_sets[FB_CTL_DAC_CH_INDEX].dac_value=m_voltage;

    USR_DBG_DBUG("Vol:%f,%x",tmp,m_voltage);

    if(update_flg){
        main_tsk.curr_set.out_dac_value = main_tsk.cfg_dat.out_dac_value;
        mcp4728_multi_write_only_reg(main_tsk.dac_cfg_sets,USR_USED_CH_MAX);
    }

}

static void set_out_current(mo_u16 curr){

    mo_u32 m_voltage;
    //  mo_u32 int_flg=mo_critical_enter();

    /** V=R*I R=0.01R ,here the gain of the curr. amp is 20 */
    m_voltage = (curr*0.01f*20);
    m_voltage=m_voltage*(DAC_RESOLUTION/INTERNAL_VREF);
    main_tsk.dac_cfg_sets[CURR_CTL_DAC_CH_INDEX].dac_value=m_voltage;
    mcp4728_multi_write_only_reg(main_tsk.dac_cfg_sets,USR_USED_CH_MAX);

    // mo_critical_exit(int_flg);
}

#define CAL_TOLERANCE 10u// tolerance
#define CHANGE_STEP 5ul

static int out_voltage_calibration_handle(void){
    CfigDatsT cfg_datas;
    mo_s16 *offset;
    mo_u16 out_vol;
    int ret=-1;
    int timeout;
    int vbus_vol,h_t;
    
    main_tsk.cfg_dat.out_vol_offset=0;
    main_tsk.cfg_dat.out_vol_h_offset=0;
    main_tsk.cfg_dat.out_vol_m_offset=0;

    offset = &main_tsk.cfg_dat.out_vol_offset;
    out_vol = LOW_CALIBRATION_VOL;
    for(int i=0; i<3; i++){
        main_tsk.cfg_dat.out_vol = out_vol;

        set_out_voltage(out_vol,0);
        MO_DELAY_TICK(2000);
        power_out_set(OUT_CTRL_ENABLE);
        timeout=400;
        while (timeout){

            set_out_voltage(out_vol,1);
            MO_DELAY_TICK(50);
            timeout--;

            if(ina226_read_bus_voltage(ina226_OUTPUT_I2C_DEV_ADD,ina226_CH1_BUS_V_REG,&vbus_vol)==0){
                h_t = out_vol+CAL_TOLERANCE;
                if(vbus_vol < (h_t+CAL_TOLERANCE) && vbus_vol > (h_t)){
                    ret = 0;
                    break;
                }else{
                    if(vbus_vol > h_t){
                        (*offset) -= CHANGE_STEP;
                    }else{
                        (*offset) += CHANGE_STEP;
                    }
                }
            }else{
                break;
            }
        }
        if(i==0){
            offset = &main_tsk.cfg_dat.out_vol_h_offset;
            out_vol = HEIGHT_CALIBRATION_VOL;
        }else{
            offset = &main_tsk.cfg_dat.out_vol_m_offset;
            out_vol = MIDDLE_CALIBRATION_VOL;
        }
        
       
    }
    
    main_tsk.cfg_dat.out_vol = 3300ul;

    set_out_voltage(main_tsk.cfg_dat.out_vol,0);
    if(timeout){
        usr_save_cfg_data_to_flash();
    }
    CfigDatsT *cdat;
    cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));

    if(cdat){
        *cdat = main_tsk.cfg_dat;
        mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
    }

    // power_out_set(OUT_CTRL_DISABLE);
    set_power_out_handle(OUT_CTRL_DISABLE,USR_LCD_POWER_OUT_UI_UPDATE);
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

    if(main_tsk.cfg_dat.out_vol_offset > (2000ul) || \
        main_tsk.cfg_dat.out_vol_offset < (-2000)){
        main_tsk.cfg_dat.out_vol_offset = 0;    //default value
    }

    if(main_tsk.cfg_dat.out_vol_h_offset > (2000ul) || \
        main_tsk.cfg_dat.out_vol_h_offset < (-2000)){
        main_tsk.cfg_dat.out_vol_h_offset = 0;    //default value
    }

    if(main_tsk.cfg_dat.limit_curr > OC_MAX || main_tsk.cfg_dat.limit_curr < OC_MIN){
        main_tsk.cfg_dat.limit_curr = DEFAULT_OC_LIMIT; //default value
    }

    if(main_tsk.cfg_dat.out_vol > VOUT_MAX || main_tsk.cfg_dat.out_vol < VOUT_MIN){
        main_tsk.cfg_dat.out_vol = DEFAULT_O_VOL; //default value
    }


    main_tsk.dac_cfg_sets[FB_CTL_DAC_CH_INDEX].dac_ch_id = FB_CTL_DAC_CH;
    main_tsk.dac_cfg_sets[CURR_CTL_DAC_CH_INDEX].dac_ch_id = CURR_CTL_DAC_CH;

    main_tsk.curr_set = main_tsk.cfg_dat;
    /*TODO Config the DAC here*/
    main_tsk.curr_set.out_vol = VOUT_MIN;
    set_out_voltage(main_tsk.curr_set.out_vol,1);
    set_out_voltage(main_tsk.cfg_dat.out_vol,0);
    set_out_current(main_tsk.cfg_dat.limit_curr);
    
    /*enable SC8701*/
    // SC8701_CE_CTRL(1);
}
void out_vol_pudate_handle(void){
    static mo_u8 cnt=0;
    mo_u8 step=10;

    if(main_tsk.sc_8701_en){
        cnt++;
        if(cnt > 4){
            cnt=0;

            if(main_tsk.sc_8701_en == SC8701_FIRST_OUT){
                step=5;
            }
            
            if(main_tsk.curr_set.out_dac_value != main_tsk.cfg_dat.out_dac_value){
                if(main_tsk.curr_set.out_dac_value < main_tsk.cfg_dat.out_dac_value){
                    main_tsk.curr_set.out_dac_value +=step; /*mV*/
                    if(main_tsk.curr_set.out_dac_value>main_tsk.cfg_dat.out_dac_value)main_tsk.curr_set.out_dac_value=main_tsk.cfg_dat.out_dac_value;
                }else{
                        main_tsk.curr_set.out_dac_value -=step; /*mV*/
                    if(main_tsk.curr_set.out_dac_value<main_tsk.cfg_dat.out_dac_value)main_tsk.curr_set.out_dac_value=main_tsk.cfg_dat.out_dac_value;
                }
                main_tsk.dac_cfg_sets[FB_CTL_DAC_CH_INDEX].dac_value=main_tsk.curr_set.out_dac_value;
                mcp4728_multi_write_only_reg(main_tsk.dac_cfg_sets,USR_USED_CH_MAX);
            }else{
                main_tsk.sc_8701_en = SC8701_OUT_READY;
            }
        }
        
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

                ina226_init(ina226_INPUT_I2C_DEV_ADD);
                ina226_init(ina226_OUTPUT_I2C_DEV_ADD);
                
                cdat = (CfigDatsT*)mo_malloc(sizeof(CfigDatsT));
                if(cdat){
                    *cdat = main_tsk.cfg_dat;
                    mo_msg_send(get_usr_display_task(),USR_LCD_UPDATE_OUT_CFG_DAT,cdat,0);
                }
                mo_msg_send(tsk,USR_UPDATE_CFG_VOLTAGE_TO_IC,0,0);

            }
                break;

        case USR_UPDATE_CFG_VOLTAGE_TO_IC:{
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
        case USR_EVENT_DELAY_TO_EN_SC8701:
            SC8701_CE_CTRL(1);
            main_tsk.sc_8701_en = SC8701_FIRST_OUT;
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

    main_tsk.pd_req_vol = PD_REQ_9V;
    
    mo_msg_send(&main_tsk.tsk,USR_EVENT_TEST_POLLING,NULL,70);

    set_out_current(main_tsk.cfg_dat.limit_curr);

    mo_msg_send(&main_tsk.tsk,USR_EVENT_DELAY_TO_INIT,NULL,60);
    mo_msg_send(&main_tsk.tsk,USR_EVENT_DELAY_TO_EN_SC8701,NULL,1000);
    power_out_set(0);
    
}
