/**************************************************************************
 * @file usr_display_task.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/
#include "usr_main_app.h"
// #include "con_bmp.h"
#include "usr_desplay_task.h"

typedef struct usr_dis_tsk {
    mo_task_data tsk;
    mo_u8 cfg_ui_blink;
    OutVolCurrT vc[INPUT_DAT_MAX];
    mo_u16 cycle_buf_end;
    mo_u16 cycle_buf_start;
    VolScaleInforT vscale_infor;
    CurScaleInforT cscale_infor;
    mo_u32 v_y_position;
    mo_u32 monitor_table_en;
    mo_u8 measure_en;
    mo_u8 dis_mini_program_icon;
} UsrDisTskT;

static UsrDisTskT dis_tsk;
static void out_state_ui_handle(OUT_CTRL_T ctrl);
static void draw_monitor_fixed_ui(void);
static void display_draw_main_ui(void);
static void draw_monitor_run_table_ui(mo_u32 v,mo_u32 c);
static void draw_rectangle_and_text_inside(u16 x,u16 y,u16 width,u16 heigh,REC_STYLE_T style);
static void dis_output_infor(uint8_t *v_str,uint8_t *c_str,uint8_t *pw_str);
static  void dis_input_infor(uint8_t *v,uint8_t *c);



#if EN_UPLOAD_DAT_TO_APP 
#define upload_dat_to_app(format, ...)  printf(format,##__VA_ARGS__)
#else
#define upload_dat_to_app(format, ...)
#endif
const mo_task get_usr_display_task(void){
    return &dis_tsk.tsk;
}

const VolScaleInforT v_scale_table[]={
{
    .vol_dis_scale=10, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=50, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=100, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=150, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=200, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=250, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=300, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=350, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=400, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=450, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=500, /*Unit is mV*/
    .p_Vunit_str="mV",
},
{
    .vol_dis_scale=600, /*Unit is mV*/
    .p_Vunit_str="mV",
},
};

const CurScaleInforT c_scale_table[]={
{    .curr_dis_scale=4,/*Unit is uA*/
    .p_Cunit_str="uA",
    .curr_actual_scale = 4,
},
{    .curr_dis_scale=10,/*Unit is uA*/
    .p_Cunit_str="uA",
    .curr_actual_scale = 10,
},
{    .curr_dis_scale=50,/*Unit is uA*/
    .p_Cunit_str="uA",
    .curr_actual_scale = 50,
},
{    .curr_dis_scale=1,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 1000,
},
{    .curr_dis_scale=5,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 5000,
},
{    .curr_dis_scale=10,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 10000,
},
{    .curr_dis_scale=20,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 20000,
},
{    .curr_dis_scale=40,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 40000,
},
{    .curr_dis_scale=60,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 60000,
},
{    .curr_dis_scale=80,/*Unit is uA*/
    .p_Cunit_str="mA",
    .curr_actual_scale = 80000,
},
};


static void monitor_vol_curr_input(mo_u32 vol,mo_u32 curr){
        
        dis_tsk.vc[dis_tsk.cycle_buf_end].out_vol = vol;
        dis_tsk.vc[dis_tsk.cycle_buf_end].out_curr = curr;
        dis_tsk.cycle_buf_end++;
        dis_tsk.cycle_buf_end %= INPUT_DAT_MAX;

        if(dis_tsk.cycle_buf_end == dis_tsk.cycle_buf_start){
            dis_tsk.cycle_buf_start++;
            dis_tsk.cycle_buf_start %= INPUT_DAT_MAX;
        }
}

static mo_u32 get_max_curr_by_input_buf(void){
    mo_u32 dat_index;
    mo_u32 curr,max_curr=0;
    mo_u32 i=0;

    dat_index=dis_tsk.cycle_buf_start;

     while (dat_index != dis_tsk.cycle_buf_end){

        curr = dis_tsk.vc[dat_index].out_curr;
        if(curr > max_curr) max_curr = curr;
        i++;
        dat_index = (dis_tsk.cycle_buf_start+i)%INPUT_DAT_MAX;
    }

    return max_curr;
}

static mo_u32 get_max_voltage_by_input_buf(void){
    mo_u32 dat_index;
    mo_u32 curr,max=0;
    mo_u32 i=0;

    dat_index=dis_tsk.cycle_buf_start;

     while (dat_index != dis_tsk.cycle_buf_end){

        curr = dis_tsk.vc[dat_index].out_vol;
        if(curr > max) max = curr;
        i++;
        dat_index = (dis_tsk.cycle_buf_start+i)%INPUT_DAT_MAX;
    }

    return max;
}

static void covert_to_str(char *p_str_des,mo_u32 t_dat,mo_u32 t_dat2,u8 str_len){
    mo_u32 i;

    if(t_dat2<10){
        sprintf(p_str_des,"%d.00%d",t_dat,t_dat2);
    }else if(t_dat2<100){
        sprintf(p_str_des,"%d.0%d",t_dat,t_dat2);
    }else{
       sprintf(p_str_des,"%d.%d",t_dat,t_dat2); 
    }
    
    for(i=0; i<str_len; i++){
        if(p_str_des[i]=='\0'){
            p_str_des[i] = '0';
        }
    }

    p_str_des[i]='\0';

}
static void vol_dat_to_string(u8 str_len,char *p_str_des,mo_u32 int_dat/*mV*/){
    
    mo_u32 t_dat,t_dat2;

    if(p_str_des==NULL)return;

    t_dat = int_dat/1000;
    t_dat2 = int_dat%1000;
    covert_to_str(p_str_des,t_dat,t_dat2,str_len);
}
/**************************************************************************
 * @brief 
 * 
 * @param str_len 
 * @param p_str_des 
 * @param curr_dat  in uA
 * @return char* return the display unit string
 *************************************************************************/
static char *curr_dat_to_string(u8 str_len,char *p_str_des,mo_u32 curr_dat){
    mo_u32 i;
    char *curr_uint=0;
    mo_u32 t_dat,t_dat2;
    char point=0;

    if(p_str_des==NULL)return "\0";

    t_dat = curr_dat/1000;

    if(t_dat > 999){
        curr_uint = CURR_A;
        t_dat2 = t_dat%1000;
        t_dat = t_dat/1000;
       // sprintf(p_str_des,"%d.%d",t_dat,t_dat2);
       covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }else if(t_dat){
        curr_uint = CURR_mA;
        t_dat2 = curr_dat%1000;
       // sprintf(p_str_des,"%d.%d",t_dat,t_dat2);
       covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }else{
        curr_uint = CURR_uA;
        t_dat=curr_dat;
        t_dat2 = 0;
        covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }

    return curr_uint;
}
static char *power_to_string(u8 str_len,char *p_str_des,mo_u32 v,mo_u32 c){
    mo_u32 i;
    char *pw_uint=0;
    mo_u32 pw;
    mo_u32 t_dat,t_dat2;

    pw = (v/10)*(c/100); /*unit uW*/

    if(p_str_des==NULL)return "\0";

    t_dat = pw/1000;

    if(t_dat > 999){
        pw_uint = POWER_W;
        t_dat2 = t_dat%1000;
        t_dat = t_dat/1000;
       covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }else if(t_dat){
        pw_uint = POWER_mW;
        t_dat2 = pw%1000;
       covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }else{
        pw_uint = POWER_uW;
        t_dat = pw;
        t_dat2 = 0;
        covert_to_str(p_str_des,t_dat,t_dat2,str_len);
    }

    return pw_uint;
}


static void display_measure_vol_handle(MonitorDatsT dats){
    char tmp[32]={0};
    char send_to_buf[128];
    mo_u32 pw;
    char *curr_uint;
    #define FONT_WIDTH       32
    #define FONT_HEIGH       64
    #define START_POSITION_X 40
    #define START_POSITION_Y 130

    /******input monitor ui*/
    memset(tmp,0,sizeof(tmp));
    memset(send_to_buf,0,sizeof(send_to_buf));

    vol_dat_to_string(5,tmp,dats.measured_in_vl);
    LCD_ShowString(START_POSITION_X,START_POSITION_Y,tmp,M_VOLTAGE_COLOR,DIS_BACKGROUND,FONT_HEIGH,0);
    LCD_ShowString(FONT_WIDTH*5+START_POSITION_X,START_POSITION_Y,"V",IN_VOL_COLOR,DIS_BACKGROUND,64,0);
    sprintf(send_to_buf, "m_vol=%s@V,",tmp);

    upload_dat_to_app("%s",send_to_buf);

}

static void display_monitor_dat_handle(MonitorDatsT dats){
    char tmp[32];
    char tmp2[32];
    char tmp1[32];
    char send_to_buf[128];
    mo_u32 pw;
    char *curr_uint;

    /******input monitor ui*/
    memset(tmp,0,sizeof(tmp));
    memset(send_to_buf,0,sizeof(send_to_buf));
    vol_dat_to_string(5,tmp,dats.in_vol);
    sprintf(send_to_buf, "i_vol=%s@V,",tmp);
    strcat(tmp," V");
    
    memset(tmp2,0,sizeof(tmp2));
    curr_uint=curr_dat_to_string(5,tmp2,dats.in_curr);
    strcat(send_to_buf, "i_cur=");
    strcat(send_to_buf, tmp2);
    strcat(send_to_buf, "@");
    strcat(send_to_buf, curr_uint);

    strcat(tmp2,curr_uint);
    dis_input_infor(tmp,tmp2);
    /************************/

   /*******out monitor ui*/
    memset(tmp,0,sizeof(tmp));
    vol_dat_to_string(5,tmp,dats.out_vol);
    strcat(send_to_buf, ",o_vol=");
    strcat(send_to_buf, tmp);
    strcat(send_to_buf, "@V,");

    strcat(tmp," V");
    memset(tmp2,0,sizeof(tmp2));
    curr_uint = power_to_string(5,tmp2,dats.out_vol,dats.out_curr);
    strcat(send_to_buf, "o_pw=");
    strcat(send_to_buf, tmp2);
    strcat(send_to_buf, "@");
    strcat(send_to_buf, curr_uint);
    strcat(tmp2,curr_uint);

    memset(tmp1,0,sizeof(tmp1));
    curr_uint=curr_dat_to_string(5,tmp1,dats.out_curr);
    strcat(send_to_buf, ",o_cur=");
    strcat(send_to_buf, tmp1);
    strcat(send_to_buf, "@");
    strcat(send_to_buf, curr_uint);
    strcat(send_to_buf, "\n");
    strcat(tmp1,curr_uint);
    dis_output_infor(tmp,tmp1,tmp2);

    upload_dat_to_app("%s",send_to_buf);
/*************************/

}


static void display_cfg_dat_handle(CfigDatsT dats,CV_dis_t v_dis,CV_dis_t c_dis){
    char tmp[32]={0};
    REC_STYLE_T style={.font_size=24,
    .rec_fill_color=RGB_to_565(112, 173, 71),
    .txt=0,
    .txt_color=RUN_NUM_COLOR,
    .txt_x=8,
    .txt_y=0};
    #define V_REC_X_START 226
    #define V_REC_Y_START 49
    #define C_REC_X_START 226
    #define C_REC_Y_START 81
    #define REC_WIIDTH    93
    #define REC_HEIGHT    28

    switch (v_dis) {
        case CV_DIS_EN: {
            memset(tmp, 0, sizeof(tmp));
            vol_dat_to_string(5, tmp, dats.out_vol);
            // LCD_ShowString(142,5,tmp,RUN_NUM_COLOR,DIS_BACKGROUND,24,0);
            // USR_DBG_DBUG("cfg-v:%d\r\n",dats.out_vol);
            style.txt = tmp;
            draw_rectangle_and_text_inside(V_REC_X_START, V_REC_Y_START, REC_WIIDTH, REC_HEIGHT, style);
            LCD_ShowString(V_REC_X_START+(style.font_size/2)*5+style.txt_x, 
                            V_REC_Y_START+style.txt_y,
                            " V", 
                            RUN_NUM_COLOR, 
                            style.rec_fill_color, 
                            24, 
                            0);
        }
        break;
        case CV_DIS_DISABLE: {
            style.txt = "\0";
            draw_rectangle_and_text_inside(V_REC_X_START, V_REC_Y_START, REC_WIIDTH, REC_HEIGHT, style);
            // LCD_ShowString(285, 43, "V", CFG_VOL_COLOR, style.rec_fill_color, 24, 0);
        } break;
        default:
            break;
    }

    switch (c_dis) {
        case CV_DIS_EN: {
            char *curr_uint;
            memset(tmp, 0, sizeof(tmp));
            curr_uint = curr_dat_to_string(5, tmp, dats.limit_curr * 1000);
            style.txt = tmp;
            style.rec_fill_color = RGB_to_565(112, 48, 160);
            // USR_DBG_DBUG("cfg-c:%d\r\n",dats.limit_curr);
            draw_rectangle_and_text_inside(C_REC_X_START, C_REC_Y_START, REC_WIIDTH, REC_HEIGHT, style);
            // LCD_ShowString(285, 78, curr_uint, CFG_CURR_COLOR, style.rec_fill_color, 24, 0);
            LCD_ShowString(C_REC_X_START+(style.font_size/2)*5+style.txt_x, 
                            C_REC_Y_START+style.txt_y,
                            curr_uint, 
                            CFG_CURR_COLOR, 
                            style.rec_fill_color, 
                            24, 
                            0);
        } break;
        case CV_DIS_DISABLE: {
            style.txt = "\0";
            style.rec_fill_color = RGB_to_565(112, 48, 160);
            draw_rectangle_and_text_inside(C_REC_X_START, C_REC_Y_START, REC_WIIDTH, REC_HEIGHT, style);
        } break;
        default:
            break;
    }
}

static void cfg_dat_send_to_host_handle(CfigDatsT dats){
    char tmp[32]={0};

    char *curr_uint;
    memset(tmp,0,sizeof(tmp));
    vol_dat_to_string(5,tmp,dats.out_vol);
    
    upload_dat_to_app("o_cfg=%s@V\n",tmp);
    
}

static void display_pd_req_cfg_ui(u8 vol){
    LCD_ShowIntNum(24,(38),vol,2,RUN_NUM_COLOR,DIS_BACKGROUND,12);
    LCD_ShowString((24+12),(38),"V",IN_VOL_COLOR,DIS_BACKGROUND,12,0);
}

static void cfg_pd_req_send_to_host(u8 vol){
    upload_dat_to_app("i_cfg=%d@V\n",vol);
}

static void out_pw_ctrl_send_to_host(u8 en){
    upload_dat_to_app("o_ctr=%d@V\n",en);
}

void to_sync_all_cfg_to_app(mo_u8 in_req_vol, mo_u8 octl, mo_u32 out_cfg){
    char tmp[32]={0};

    char *curr_uint;
    memset(tmp,0,sizeof(tmp));
    vol_dat_to_string(5,tmp,out_cfg);

    upload_dat_to_app("o_cfg=%s@V,o_ctr=%d@V,i_cfg=%d@V\n",tmp,octl,in_req_vol);

    if (dis_tsk.dis_mini_program_icon) {
        dis_tsk.dis_mini_program_icon = 0;
        LCD_Fill(0, 0, LCD_W, LCD_H, DIS_BACKGROUND);
        display_draw_main_ui();
        draw_monitor_run_table_ui(0, 0);
        draw_monitor_fixed_ui();
    } 

}

#if 0
#else 

static  void dis_input_infor(uint8_t *v,uint8_t *c){
    LCD_ShowString(12,50,v,IN_VOL_COLOR,DIS_BACKGROUND,24,0);
    LCD_ShowString(12,82,c,IN_CURR_COLOR,DIS_BACKGROUND,24,0);
}
static void dis_output_infor(uint8_t *v_str,uint8_t *c_str,uint8_t *pw_str){
    LCD_ShowString(210,6,pw_str,OUT_PW_COLOR,DIS_BACKGROUND,24,0);
    LCD_ShowString(110,46,v_str,OUT_VOL_COLOR,DIS_BACKGROUND,32,0);
    LCD_ShowString(110,78,c_str,OUT_CURR_COLOR,DIS_BACKGROUND,32,0);
}
static void display_draw_main_ui(void){
    CfigDatsT cfg_dat={.out_vol=2700,.limit_curr=3000};
/*********USB IN PUT*****************/
    REC_STYLE_T style={.font_size=24,
    .rec_fill_color=IN_FILL_COLOR,
    .txt="IN",
    .txt_color=RUN_NUM_COLOR,
    .txt_x=25,
    .txt_y=7};
    #define REC__W 80
    #define REC__H 40

    draw_rectangle_and_text_inside(10,3,REC__W,REC__H,style);
    dis_input_infor("5.000 V","5.000 A");
/*******out monitor ui*/
    dis_output_infor("5.000 V","5.000 A","25.00W");
/*************************/
    usr_LCD_Fill(100,0,102,113,LINE_COLOR);
    usr_LCD_Fill(0,113,319,113,LINE_COLOR);
/***********/
    out_state_ui_handle(OUT_CTRL_DISABLE);
/****************cfg ui**************/
    display_cfg_dat_handle(cfg_dat,CV_DIS_EN,CV_DIS_EN);
/*****************/

/************Show version***********/
    LCD_ShowString(290,230,USB_PS_SW_VER,GRAY,DIS_BACKGROUND,12,0);
}

#endif 

static void display_draw_a_table(PointT start_point,TableInforT table_inf){
    mo_u32 i,x_s,y_s,tabl_draw_end;

    //LCD_DrawLine(0,113,319,113,LINE_COLOR);
    tabl_draw_end = table_inf.row_distance*table_inf.row_num+start_point.y;
    for(i=0; i<(table_inf.column_num+1); i++){
        x_s = start_point.x+i*table_inf.column_distance;
        usr_LCD_Fill(x_s,start_point.y,x_s,tabl_draw_end,LINE_COLOR);
    }

    tabl_draw_end = table_inf.column_distance*table_inf.column_num+start_point.x;
    for(i=0; i<(table_inf.row_num+1); i++){
        y_s = start_point.y+i*table_inf.row_distance;
        usr_LCD_Fill(start_point.x,y_s,tabl_draw_end,y_s,LINE_COLOR);
    }

}
static void update_scale_ui_infor(VolScaleInforT vscale_infor,CurScaleInforT cscale_infor){
   
    mo_u32 str_start_x;
    str_start_x = TABLE_START_X+(COLUMN_DES*COLUMN_NUM)+3;

    LCD_ShowIntNum(str_start_x+24,160,vscale_infor.vol_dis_scale,4,RUN_NUM_COLOR,DIS_BACKGROUND,12);
    LCD_ShowString(str_start_x+24+24,160,vscale_infor.p_Vunit_str,OUT_VOL_COLOR,DIS_BACKGROUND,12,0);

    LCD_ShowIntNum(str_start_x+24,180,cscale_infor.curr_dis_scale,4,RUN_NUM_COLOR,DIS_BACKGROUND,12);
    LCD_ShowString(str_start_x+24+24,180,cscale_infor.p_Cunit_str,OUT_CURR_COLOR,DIS_BACKGROUND,12,0);

}
static void draw_monitor_fixed_ui(void){
    mo_u32 i;
    mo_u32 str_start_y,str_start_x;

    str_start_y = TABLE_START_Y+(ROW_DISTANCE*ROW_NUMBER)+3;
    for(i=0; i<COLUMN_NUM; i++){
        if(!(i%2)){
            LCD_ShowIntNum(TABLE_START_X+i*COLUMN_DES,str_start_y,i,(1+i/10),GRAY_LABEL_COLOR,DIS_BACKGROUND,12);
        }  
    }
    LCD_ShowIntNum(TABLE_START_X+i*COLUMN_DES-12,str_start_y,i,(1+i/10),GRAY_LABEL_COLOR,DIS_BACKGROUND,12);
   // LCD_ShowString(start_point.x+(i-2)*table_inf.column_distance+12,str_start_y,"S",GRAY_LABEL_COLOR,DIS_BACKGROUND,12,0);

    str_start_x = TABLE_START_X+(COLUMN_DES*COLUMN_NUM)+3;
    str_start_y=TABLE_START_Y+ROW_DISTANCE*ROW_NUMBER-5;

    for(i=0; i<ROW_NUMBER+1; i++){
        if(!(i%2))
            LCD_ShowIntNum(str_start_x,str_start_y,i*10,3,GRAY_LABEL_COLOR,DIS_BACKGROUND,12);
        
        str_start_y-=ROW_DISTANCE;
    }
    update_scale_ui_infor(dis_tsk.vscale_infor,dis_tsk.cscale_infor);
}

static VolScaleInforT get_v_scale_infor_by_input_vol(mo_u32 voltage){
    mo_u32 i;
    mo_u32 table_size = sizeof(v_scale_table)/sizeof(VolScaleInforT);
    VolScaleInforT ret_infor = v_scale_table[(table_size-1)];

    for(i=0; i<table_size; i++){
        if((v_scale_table[i].vol_dis_scale*TABLE_HEIGHT) > voltage){
            ret_infor = v_scale_table[i];
            break;
        }
    }

    return ret_infor;
}

static CurScaleInforT get_curr_scale_infor_by_input_curr(mo_u32 current){
    mo_u32 i;
    mo_u32 table_size = sizeof(c_scale_table)/sizeof(CurScaleInforT);
    CurScaleInforT ret_infor = c_scale_table[(table_size-1)];

    for(i=0; i<table_size; i++){
        if((c_scale_table[i].curr_actual_scale*TABLE_HEIGHT) > current){
            ret_infor = c_scale_table[i];
            break;
        }
    }
    
    return ret_infor;
}
extern u16 *pBuff;
extern u8 f_dma_start;

#if 0 //加入小伙伴“音阙诗听”,提高屏幕刷新率的优化部分代码
static void draw_monitor_run_table_ui(mo_u32 v,mo_u32 c){
//     mo_u32 i;
//     mo_u32 str_start_y,str_start_x;
//     mo_u32 p_des;
//     mo_u32 y_end;
//     mo_u32 y_deta_start,y_start;
//     mo_u32 y_max_point;
//     mo_u32 dat_index;
//     VolScaleInforT vol_scale;
//     CurScaleInforT cur_scale;

//     PointT start_point={.x=TABLE_START_X,.y=TABLE_START_Y};
//     TableInforT table_inf={.row_distance = ROW_DISTANCE,.row_num=ROW_NUMBER,
//                             .column_distance=COLUMN_DES,.column_num=COLUMN_NUM};

//     if(dis_tsk.monitor_table_en==0)return;

//     monitor_vol_curr_input(v,c);

//     vol_scale = get_v_scale_infor_by_input_vol(get_max_voltage_by_input_buf());
//     cur_scale = get_curr_scale_infor_by_input_curr(get_max_curr_by_input_buf());

//     usr_LCD_Fill(TABLE_START_X,TABLE_START_Y,TABLE_START_X+TABLE_WIDTH,\
//                 TABLE_START_Y+TABLE_HEIGHT,DIS_BACKGROUND);

//     display_draw_a_table(start_point,table_inf);

//     if(vol_scale.vol_dis_scale != dis_tsk.vscale_infor.vol_dis_scale ||\
//      cur_scale.curr_actual_scale != dis_tsk.cscale_infor.curr_actual_scale){
//         dis_tsk.vscale_infor = vol_scale;
//         dis_tsk.cscale_infor = cur_scale;
//         update_scale_ui_infor(dis_tsk.vscale_infor,dis_tsk.cscale_infor);
//     }

//     y_start = start_point.y+TABLE_HEIGHT;
//     y_max_point = TABLE_HEIGHT;

//     i=0;
//     dat_index=dis_tsk.cycle_buf_start;

//     p_des = dis_tsk.vc[dat_index].out_vol/dis_tsk.vscale_infor.vol_dis_scale;
//     if(p_des>y_max_point)p_des=y_max_point;
//     y_deta_start = y_start-p_des;

//    // MO_LOG("e:%d,s:%d\r\n",dis_tsk.cycle_buf_end,dis_tsk.cycle_buf_start);
//     while (dat_index != dis_tsk.cycle_buf_end){

//         p_des = dis_tsk.vc[dat_index].out_vol/dis_tsk.vscale_infor.vol_dis_scale;
//         if(p_des>y_max_point)p_des=y_max_point;

//         y_end = y_start-p_des;
//         LCD_DrawLine(start_point.x+i,y_deta_start,start_point.x+i,y_end,OUT_VOL_COLOR);
//         y_deta_start = y_end;

//         i++;
//         dat_index = (dis_tsk.cycle_buf_start+i)%INPUT_DAT_MAX;
//     }
    
//     //MO_LOG("i:%d e:%d\r\n",i,dis_tsk.cycle_buf_end);

// #if 1
//     i=0;
//     dat_index=dis_tsk.cycle_buf_start;

//     p_des = dis_tsk.vc[dat_index].out_curr /dis_tsk.cscale_infor.curr_actual_scale;
//     if(p_des>y_max_point)p_des=y_max_point;
//     y_deta_start = y_start-p_des;

//    // MO_LOG("e:%d,s:%d\r\n",dis_tsk.cycle_buf_end,dis_tsk.cycle_buf_start);
//     while (dat_index != dis_tsk.cycle_buf_end){

//         p_des = dis_tsk.vc[dat_index].out_curr /dis_tsk.cscale_infor.curr_actual_scale;
//         if(p_des>y_max_point)p_des=y_max_point;

//         y_end = y_start-p_des;
//         LCD_DrawLine(start_point.x+i,y_deta_start,start_point.x+i,y_end,OUT_CURR_COLOR);
//         y_deta_start = y_end;

//         i++;
//         dat_index = (dis_tsk.cycle_buf_start+i)%INPUT_DAT_MAX;
//     }
// #endif

    mo_u32 x,y,temp;
    mo_u32 i;
    mo_u32 str_start_y,str_start_x;
    mo_u32 p_des;
    mo_u32 V_y_end,I_y_end;
    mo_u32 V_y_deta_start,y_start,I_y_deta_start;
    mo_u32 y_max_point;
    mo_u32 dat_index;
    VolScaleInforT vol_scale;
    CurScaleInforT cur_scale;
    static u8 f_data_full_scr = 0;

    PointT start_point={.x=TABLE_START_X,.y=TABLE_START_Y};
    TableInforT table_inf={.row_distance = ROW_DISTANCE,.row_num=ROW_NUMBER,
                            .column_distance=COLUMN_DES,.column_num=COLUMN_NUM};
    if(dis_tsk.monitor_table_en==0)return;
    monitor_vol_curr_input(v,c);

    vol_scale = get_v_scale_infor_by_input_vol(get_max_voltage_by_input_buf());
    cur_scale = get_curr_scale_infor_by_input_curr(get_max_curr_by_input_buf());

    if(vol_scale.vol_dis_scale != dis_tsk.vscale_infor.vol_dis_scale ||\
     cur_scale.curr_actual_scale != dis_tsk.cscale_infor.curr_actual_scale){
        dis_tsk.vscale_infor = vol_scale;
        dis_tsk.cscale_infor = cur_scale;
        update_scale_ui_infor(dis_tsk.vscale_infor,dis_tsk.cscale_infor);
    }

    y_start = start_point.y+TABLE_HEIGHT;
    y_max_point = TABLE_HEIGHT;

    i=0;
    dat_index=dis_tsk.cycle_buf_start;
    
    if(dis_tsk.cycle_buf_end > INPUT_DAT_MAX-1)//数据是否够满屏显示
        f_data_full_scr = 1;

    p_des = dis_tsk.vc[dat_index].out_vol/dis_tsk.vscale_infor.vol_dis_scale;
    if(p_des>y_max_point)p_des=y_max_point;
    V_y_deta_start = y_start-p_des;
    
    for(x = 0;x < table_inf.column_distance * table_inf.column_num;x++)//每列发送一次DMA
    {
        p_des = dis_tsk.vc[dat_index].out_vol/dis_tsk.vscale_infor.vol_dis_scale;
        if(p_des>y_max_point)p_des=y_max_point;

        // y_end = y_start-p_des;
        V_y_end = table_inf.row_distance*table_inf.row_num-p_des;

        p_des = dis_tsk.vc[dat_index].out_curr /dis_tsk.cscale_infor.curr_actual_scale;
        if(p_des>y_max_point)p_des=y_max_point;
        I_y_end = table_inf.row_distance*table_inf.row_num-p_des;
        // I_y_end = 100-V_y_end;
        if(0 == (x % table_inf.column_distance) || (x == table_inf.column_distance * table_inf.column_num-1))//是否在纵向网格线上
        {
            //
            for(y = 0;y < table_inf.row_distance*table_inf.row_num;y++)
            {
                //voltage
                if(V_y_end > V_y_deta_start)//向下画线
                {
                    if((y >= V_y_deta_start) && (y <= V_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))//&& (f_data_full_scr || x < dis_tsk.cycle_buf_end)作用防止刚开始数据不够画线会有尾巴
                        pBuff[y] = OUT_VOL_COLOR;
                    else
                        pBuff[y] = LINE_COLOR;
                }
                else
                {
                    if((y >= V_y_end) && (y <= V_y_deta_start) && (x != 0))
                        pBuff[y] = OUT_VOL_COLOR;
                    else
                        pBuff[y] = LINE_COLOR;
                }
                //current
                if(I_y_end > I_y_deta_start)//向下画线
                {
                    if((y >= I_y_deta_start) && (y <= I_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))
                        pBuff[y] = OUT_CURR_COLOR;
                    // else
                    //     pBuff[y] = LINE_COLOR;
                }
                else
                {
                    if((y >= I_y_end) && (y <= I_y_deta_start) && (x != 0))
                        pBuff[y] = OUT_CURR_COLOR;
                    // else
                    //     pBuff[y] = LINE_COLOR;
                }
            }
        }
        else
        {
            for(y = 0;y < table_inf.row_distance*table_inf.row_num;y++)
            {
                
                if(0 == (y % table_inf.row_distance) || (y == table_inf.row_distance*table_inf.row_num-1))//是否在横向网格线上
                {
                    //voltage
                    if(V_y_end > V_y_deta_start)//向下画线
                    {
                        if((y >= V_y_deta_start) && (y <= V_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))
                            pBuff[y] = OUT_VOL_COLOR;
                        else
                            pBuff[y] = LINE_COLOR;
                    }
                    else
                    {
                        if((y >= V_y_end) && (y <= V_y_deta_start) && (x != 0))
                            pBuff[y] = OUT_VOL_COLOR;
                        else
                            pBuff[y] = LINE_COLOR;
                    }
                    //current
                    if(I_y_end > I_y_deta_start)//向下画线
                    {
                        if((y >= I_y_deta_start) && (y <= I_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))
                            pBuff[y] = OUT_CURR_COLOR;
                        // else
                        //     pBuff[y] = LINE_COLOR;
                    }
                    else
                    {
                        if((y >= I_y_end) && (y <= I_y_deta_start) && (x != 0))
                            pBuff[y] = OUT_CURR_COLOR;
                        // else
                        //     pBuff[y] = LINE_COLOR;
                    }
                }
                else
                {
                    //voltage
                    if(V_y_end > V_y_deta_start)//向下画线
                    {
                        if((y >= V_y_deta_start) && (y <= V_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))
                            pBuff[y] = OUT_VOL_COLOR;
                        else
                            pBuff[y] = 0x0000;
                    }
                    else
                    {
                        if((y >= V_y_end) && (y <= V_y_deta_start) && (x != 0))
                            pBuff[y] = OUT_VOL_COLOR;
                        else
                            pBuff[y] = 0x0000;
                    }
                    // current
                    if(I_y_end > I_y_deta_start)//向下画线
                    {
                        if((y >= I_y_deta_start) && (y <= I_y_end) && (x != 0) && (f_data_full_scr || x < dis_tsk.cycle_buf_end))
                            pBuff[y] = OUT_CURR_COLOR;
                        // else
                        //     pBuff[y] = 0x0000;
                    }
                    else
                    {
                        if((y >= I_y_end) && (y <= I_y_deta_start) && (x != 0))
                            pBuff[y] = OUT_CURR_COLOR;
                        // else
                        //     pBuff[y] = 0x0000;
                    }
                }
            }
        }
        V_y_deta_start = V_y_end;
        I_y_deta_start = I_y_end;
        dat_index = (dis_tsk.cycle_buf_start+x)%INPUT_DAT_MAX;
        usr_LCD_Fill_1(x+start_point.x,start_point.y,x+start_point.x,start_point.y+table_inf.row_distance*table_inf.row_num,0);
    }
    //最后一次DMA传输需要等待传输完成
    while (1) {
        if (dma_flag_get(DMA1_FDT3_FLAG) != RESET)  // 绛夊緟閫氶亾4浼犺緭瀹屾垚
        {
            dma_flag_clear(DMA1_FDT3_FLAG);  // 娓呴櫎閫氶亾4浼犺緭瀹屾垚鏍囧織
            break;
        }
        
    }
    f_dma_start = 0;//本次DMA传输结束
    wk_spi1_init(SPI_FRAME_8BIT);
}

#else //小乌龟：优化画线算法，改为行扫描的方式
static void draw_monitor_run_table_ui(mo_u32 v, mo_u32 c) {
    mo_u32 x, y, temp;
    mo_u32 row_num, column_num;
    mo_u32 y_max_point, x_max_point, draw_y_end, draw_x_end;
    mo_u32 p_des, prev_des;
    mo_u32 dat_index, prev_idx;

    VolScaleInforT vol_scale;
    CurScaleInforT cur_scale;
    mo_u16 delta_y;

    PointT start_point = {.x = TABLE_START_X, .y = TABLE_START_Y};
    TableInforT table_inf = {.row_distance = ROW_DISTANCE, .row_num = ROW_NUMBER, .column_distance = COLUMN_DES, .column_num = COLUMN_NUM};
    if (dis_tsk.monitor_table_en == 0) return;
    monitor_vol_curr_input(v, c);

    vol_scale = get_v_scale_infor_by_input_vol(get_max_voltage_by_input_buf());
    cur_scale = get_curr_scale_infor_by_input_curr(get_max_curr_by_input_buf());

    if (vol_scale.vol_dis_scale != dis_tsk.vscale_infor.vol_dis_scale || cur_scale.curr_actual_scale != dis_tsk.cscale_infor.curr_actual_scale) {
        dis_tsk.vscale_infor = vol_scale;
        dis_tsk.cscale_infor = cur_scale;
        update_scale_ui_infor(dis_tsk.vscale_infor, dis_tsk.cscale_infor);
    }

    y_max_point = (table_inf.row_distance * table_inf.row_num);
    x_max_point = table_inf.column_distance * table_inf.column_num;

    for (row_num = 0; row_num < (y_max_point + 1); row_num++) {
        for (column_num = 0; column_num < (x_max_point + 1); column_num++) {
            pBuff[column_num] = 0;
            if (row_num % table_inf.row_distance == 0) { /*表格画行线 */
                pBuff[column_num] = LINE_COLOR;
            }
            if (column_num % table_inf.column_distance == 0) { /*表格画列线*/
                pBuff[column_num] = LINE_COLOR;
            }

            if (column_num >= x_max_point) {
                continue;
            }

            dat_index = (dis_tsk.cycle_buf_start + column_num) % INPUT_DAT_MAX;

            if (dat_index != dis_tsk.cycle_buf_end) {
                p_des = dis_tsk.vc[dat_index].out_vol / dis_tsk.vscale_infor.vol_dis_scale;
                if (p_des > y_max_point) p_des = y_max_point;
                if (p_des == row_num) {
                    pBuff[column_num] = OUT_VOL_COLOR; /*画电压点*/
                }

                if (column_num) {
                    prev_des = dis_tsk.vc[prev_idx].out_vol / dis_tsk.vscale_infor.vol_dis_scale;
                    if (prev_des > y_max_point) prev_des = y_max_point;
                    if (prev_des > p_des) { /*电压下降*/
                        if (row_num < prev_des && row_num > p_des) {
                            pBuff[column_num] = OUT_VOL_COLOR; /*画下降曲线*/
                        }
                    } else { /*电压上升或者保持*/
                        if (row_num > prev_des && row_num < p_des) {
                            pBuff[column_num] = OUT_VOL_COLOR; /*画上升曲线*/
                        }
                    }
                }

                p_des = dis_tsk.vc[dat_index].out_curr / dis_tsk.cscale_infor.curr_actual_scale;
                if (p_des > y_max_point) p_des = y_max_point;
                if (p_des == row_num) {
                    pBuff[column_num] = OUT_CURR_COLOR; /*画电流点*/
                }

                if (column_num) {
                    prev_des = dis_tsk.vc[prev_idx].out_curr / dis_tsk.cscale_infor.curr_actual_scale;
                    if (prev_des > y_max_point) prev_des = y_max_point;
                    if (prev_des > p_des) { /*电流下降*/
                        if (row_num < prev_des && row_num > p_des) {
                            pBuff[column_num] = OUT_CURR_COLOR; /*画下降曲线*/
                        }
                    } else { /*电流上升或者保持*/
                        if (row_num > prev_des && row_num < p_des) {
                            pBuff[column_num] = OUT_CURR_COLOR; /*画上升曲线*/
                        }
                    }
                }
            }
            prev_idx = dat_index;
        }
        draw_y_end = start_point.y + (y_max_point - row_num) - 1;
        draw_x_end = start_point.x + (table_inf.column_distance * table_inf.column_num) + 1;
        usr_LCD_Fill_1(start_point.x, draw_y_end, draw_x_end, draw_y_end, 0);
    }

    // 最后一次DMA传输需要等待传输完成
    while (1) {
        if (dma_flag_get(DMA1_FDT3_FLAG) != RESET)  // 绛夊緟閫氶亾4浼犺緭瀹屾垚
        {
            dma_flag_clear(DMA1_FDT3_FLAG);  // 娓呴櫎閫氶亾4浼犺緭瀹屾垚鏍囧織
            break;
        }
    }
    f_dma_start = 0;  // 本次DMA传输结束
    wk_spi1_init(SPI_FRAME_8BIT);
}
#endif

static void draw_rectangle_and_text_inside(u16 x,u16 y,u16 width,u16 heigh,REC_STYLE_T style){
    LCD_Fill(x,y,x+width,y+heigh,style.rec_fill_color);
    LCD_ShowString(x+style.txt_x,y+style.txt_y,style.txt,style.txt_color,style.rec_fill_color,style.font_size,0); 
}

static void out_state_ui_handle(OUT_CTRL_T ctrl){

    REC_STYLE_T style={.font_size=24,
    .rec_fill_color=USB_PS_OUT_DISABLE_COLOR,
    .txt="OFF",
    .txt_color=USB_PS_O_OFF_TXT_COLOR,
    .txt_x=25,
    .txt_y=7};

    if(ctrl == OUT_CTRL_DISABLE){
       draw_rectangle_and_text_inside(OUT_CTRL_BLOCK_POSSITION_X,OUT_CTRL_BLOCK_POSSITION_Y,OUT_CTRL_B_WIDTH,OUT_CTRL_B_HEIGHT,style);
    }else{
        style.txt="OUT";
        style.rec_fill_color = USB_PS_OUT_ENABLE_COLOR;
        style.txt_color = USB_PS_O_ON_TXT_COLOR;
        draw_rectangle_and_text_inside(OUT_CTRL_BLOCK_POSSITION_X,OUT_CTRL_BLOCK_POSSITION_Y,OUT_CTRL_B_WIDTH,OUT_CTRL_B_HEIGHT,style);
    } 
}





static void display_task_handle(mo_task tsk, mo_msg_id msg_id, mo_msg msg) {
    switch (msg_id) {
        case USR_LCD_INIT_START: {
            // LCD_Init();//LCD初始化
            USR_DBG_INFO("10 \r\n");
            LCD_Fill(0,0,LCD_W,LCD_H,DIS_BACKGROUND);
            USR_DBG_INFO("12 \r\n");

            display_draw_main_ui();
            draw_monitor_run_table_ui(0,0);
            draw_monitor_fixed_ui();
            USR_DBG_INFO("13\r\n");
        } break;
        case USR_LCD_UPDATE_MONITOR_DAT:{
            #if 1
            MonitorDatsT *dats = (MonitorDatsT*)msg;

            if (dis_tsk.dis_mini_program_icon) break;

            display_monitor_dat_handle(*dats);
            #if 1
            if(dis_tsk.measure_en){
                display_measure_vol_handle(*dats);
            }else{
                draw_monitor_run_table_ui(dats->out_vol,dats->out_curr);
            }
            #else
            draw_monitor_run_table_ui(dats->out_vol,dats->out_curr);
            #endif 
        #endif
        }
        break;
        case USR_LCD_MEASURE_VOL_TOGGLE:{
            dis_tsk.measure_en = !dis_tsk.measure_en;
            if(dis_tsk.measure_en){
                usr_LCD_Fill(TABLE_START_X,TABLE_START_Y,TABLE_START_X+TABLE_WIDTH,\
                TABLE_START_Y+TABLE_HEIGHT,DIS_BACKGROUND);
            }
        }
            break;

        case USR_LCD_UPDATE_OUT_CFG_DAT:{
            CfigDatsT *dats = (CfigDatsT*)msg;
            display_cfg_dat_handle(*dats,CV_DIS_EN,CV_DIS_EN);
        }
            break;

        case USR_LCD_CFG_UI_ACTIVE:
            //mo_msg_cancel_all(tsk,USR_LCD_CFG_UI_ACTIVE_UPDATE);
            mo_msg_send(tsk,USR_LCD_CFG_UI_ACTIVE_UPDATE,0,0);
            break;
        case USR_LCD_CFG_UI_INACTIVE:
            mo_msg_cancel_all(tsk,USR_LCD_CFG_UI_ACTIVE_UPDATE);
            display_cfg_dat_handle(main_app_get_cfg_dat(),CV_DIS_EN,CV_DIS_EN);
            dis_tsk.cfg_ui_blink=1;
            break;
        case USR_LCD_CFG_UI_ACTIVE_UPDATE:

            if(!dis_tsk.cfg_ui_blink){
                dis_tsk.cfg_ui_blink=1;
                display_cfg_dat_handle(main_app_get_cfg_dat(),CV_DIS_EN,CV_DIS_EN);
            }else{
                dis_tsk.cfg_ui_blink=0;
                if(main_app_get_cfg_mode() == VOL_CFG_M){
                    // LCD_ShowString(142,5,"     ",RUN_NUM_COLOR,DIS_BACKGROUND,24,0);
                    display_cfg_dat_handle(main_app_get_cfg_dat(),CV_DIS_DISABLE,CV_DIS_NONE);
                }else if(main_app_get_cfg_mode() == CURR_CFG_M){
                    // LCD_ShowString(220,5,"     ",RUN_NUM_COLOR,DIS_BACKGROUND,24,0);
                    display_cfg_dat_handle(main_app_get_cfg_dat(),CV_DIS_NONE,CV_DIS_DISABLE);
                }
            }
            mo_msg_cancel_all(tsk,USR_LCD_CFG_UI_ACTIVE_UPDATE);
            mo_msg_send(tsk,USR_LCD_CFG_UI_ACTIVE_UPDATE,0,400);
            break;
        case USR_LCD_TEST_MSG:
            if(dis_tsk.monitor_table_en)dis_tsk.monitor_table_en=0;
            else dis_tsk.monitor_table_en=1;
            break;
        case USR_LCD_PD_REQ_UI_UPDATE:
            display_pd_req_cfg_ui(*(mo_u8*)msg);
            break;
        case USR_SYNC_PD_REQ_VOL_TO_HOST:
            cfg_pd_req_send_to_host(*(mo_u8*)msg);
            break;
        case USR_SYNC_OVOL_TO_HOST:{
            CfigDatsT *dats = (CfigDatsT*)msg;
            cfg_dat_send_to_host_handle(*dats);
        }
            break;
        case USR_HOST_CONNECTED_TO_SYNC_DATS:
            break;
        case USR_LCD_POWER_OUT_UI_UPDATE:{
            OUT_CTRL_T *ctrl = (OUT_CTRL_T*)msg;
            out_state_ui_handle(*ctrl);
            out_pw_ctrl_send_to_host(*ctrl);
        }
            break;
        case USR_LCD_POWER_OUT_UI_UPDATE_BY_APP:{
            OUT_CTRL_T *ctrl = (OUT_CTRL_T*)msg;
            out_state_ui_handle(*ctrl);
        }
            break;
        case USR_LCD_TOGGLE_WEIXIN_MINI_PROGRAM_ICON:
            if (dis_tsk.dis_mini_program_icon) {
                dis_tsk.dis_mini_program_icon = 0;
                LCD_Fill(0, 0, LCD_W, LCD_H, DIS_BACKGROUND);
                display_draw_main_ui();
                draw_monitor_run_table_ui(0, 0);
                draw_monitor_fixed_ui();
            } else {
                dis_tsk.dis_mini_program_icon = 1;
                LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
                // LCD_ShowPicture(40, 1, 237ul, 239ul, con_bmp);
            }

            break;
        default:
            break;

    }
}

void usr_display_task_init(void){

    memset(&dis_tsk,0,sizeof(dis_tsk));
    #if 1
    dis_tsk.tsk.handler = display_task_handle;
    dis_tsk.cfg_ui_blink=1;

    dis_tsk.cscale_infor.curr_actual_scale = 1000;
    dis_tsk.cscale_infor.curr_dis_scale=1;
    dis_tsk.cscale_infor.p_Cunit_str = "mA";
    dis_tsk.vscale_infor.p_Vunit_str = "mV";
    dis_tsk.vscale_infor.vol_dis_scale = 100;
    // dis_tsk.v_y_position = TABLE_START_Y+ROW_DISTANCE*ROW_NUMBER;
     dis_tsk.monitor_table_en = 1;

    dis_tsk.vc[dis_tsk.cycle_buf_end].out_vol = 0;
    dis_tsk.vc[dis_tsk.cycle_buf_end].out_curr = 0;
    dis_tsk.cycle_buf_end++;

    mo_msg_cancel_all(&dis_tsk.tsk,USR_LCD_INIT_START);
    mo_msg_send(&dis_tsk.tsk,USR_LCD_INIT_START,NULL,40);
    LCD_Init();//LCD初始化
    
    #endif
    
}


