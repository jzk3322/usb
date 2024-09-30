/**************************************************************************
 * @file MCP4728.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-05
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/

#include "MCP4728.h"

void mcp4728_multi_write_only_reg(DAC_CFG_D_SET_T *p_dac_d_set,mo_u8 len){
    mo_u8 send_buf[USR_USED_CH_MAX*3+1];
    mo_u8 i=0,k;
    i2c_status_type i2c_status;

    for(k=0;k<USR_USED_CH_MAX;k++){   

        if(p_dac_d_set[k].dac_value>MAX_DAC_VALUE)p_dac_d_set[k].dac_value=MAX_DAC_VALUE;

        send_buf[i++]=MULTI_WRITE_CMD_MARK|(p_dac_d_set[k].dac_ch_id<<1);
        send_buf[i++]=((p_dac_d_set[k].dac_value>>8)&0x0f)|DAC_CFG_DAT;
        send_buf[i++]=((p_dac_d_set[k].dac_value)&0xff);
    }
    
    i2c_status = usr_i2c_master_transmit(DEFAULT_MCP4728_DEV_ADDR, send_buf, i, 1000);

    if (i2c_status == I2C_OK) {
    } else {
        USR_DBG_INFO("mcp4728 write error:%d\r\n", i2c_status);
    }
}

void mcp4728_single_write_reg_and_eeprom(DAC_CFG_D_SET_T p_dac_d_set){
    mo_u8 send_buf[4];
    i2c_status_type i2c_status;

    if(p_dac_d_set.dac_value>MAX_DAC_VALUE)p_dac_d_set.dac_value=MAX_DAC_VALUE;
    
    send_buf[0]=SINGLE_W_REG_AND_EEPROM_CMD_MARK|(p_dac_d_set.dac_ch_id<<1);
    send_buf[1]=((p_dac_d_set.dac_value>>8)&0x0f)|DAC_CFG_DAT;
    send_buf[2]=((p_dac_d_set.dac_value)&0xff);

    i2c_status = usr_i2c_master_transmit(DEFAULT_MCP4728_DEV_ADDR, send_buf, 3, 1000);

    if (i2c_status == I2C_OK) {
    } else {
        USR_DBG_INFO("mcp4728 write error:%d\r\n", i2c_status);
    }

}
