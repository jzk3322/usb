/**************************************************************************
 * @file MCP4728.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-05
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/
#ifndef MCP4728__H_H
#define MCP4728__H_H
#include<stdint.h>
#include<stdio.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "mo_config.h"
#include "usr_debug.h"
#include "usr_i2c.h"

#define DEFAULT_MCP4728_DEV_ADDR 0xc0
#define INTERNAL_VREF            2048ul  // 2.048V
#define DAC_RESOLUTION           4096ul  /*DAC:12bit*/
#define MAX_DAC_VALUE            0X0FFF

#define GENERAL_CALL_CMD 0X00 /*in the first byte*/
typedef enum mcp_4728_GC_cmd_set{
    GC_RESET = 0X06,/*• Internal Reset similar to a Power-on Reset (POR). 
                    The contents of the EEPROM are loaded into 
                    each DAC input and output registers immediately
                    • VOUT will be available immediately regardless of 
                    the LDAC pin condition*/
    GC_WAKE_UP=0X09,/*the device will reset the Power-Down bits (PD1, PD0 = 0,0)*/
    GC_SW_UPDATE = 0X08,/*the device updates all DAC analog outputs (VOUT) at the same time*/
    GC_READ_ADDR_BITS = 0X0C,/*This command is used to read the I2C address bits of the device*/
    MCP_CMD_MAX____
}MCP4728_GC_CMD_SET_T;

/*
PD1 PD0 Function
0   0   Normal Mode
0   1   1 kΩ resistor to ground (Note 1)
1   0   100 kΩ resistor to ground (Note 1)
1   1   500 kΩ resistor to ground (Note 1)
*/
/*
DAC Channel Selection bits:
00 = Channel A
01 = Channel B
10 = Channel C
11 = Channel D
*/
/*
Gain selection bit:
0 = x1 (gain of 1) 
1 = x2 (gain of 2)
*/
/*
Voltage Reference Selection bit:
0 = VDD
1 = Internal voltage reference (2.048V)
*/
/*
UDAC
0 = Upload. Output (VOUT) is updated.
1 = Do not upload. 
*/

#define DAC_CFG_DAT   0x80
/*Multi-Write Command*/
#define MULTI_WRITE_CMD_MARK             0x40  // Multi-Write for DAC Input Registers
//S 1 1 0 0 A2 A1 A0 0 A|0 1 0 0 0 DAC1 DAC0 UDAC A| VREF PD1 PD0 Gx D11 D10 D9 D8 A D7 D6 D5 D4 D3 D2 D1 D0 A ....Repeat Bytes of the 2nd - 4th Bytes
#define SINGLE_W_REG_AND_EEPROM_CMD_MARK 0X58  // Single Write for DAC Input Register and EEPROM
//S 1 1 0 0 A2 A1 A0 0 A|0 1 0 1 1 DAC1 DAC0 UDAC A| VREF PD1 PD0 Gx D11 D10 D9 D8 A| D7 D6 D5 D4 D3 D2 D1 D0 A| P
typedef enum DAC_CH_SET {
    DAC_CH_A=0,
    DAC_CH_B,
    DAC_CH_C,
    DAC_CH_D
}DAC_CHANNEL_SET_T;

#define USR_USED_CH_MAX 2u
typedef struct dac_cfg_dat_set{
    mo_u8 dac_ch_id;
    mo_u16 dac_value;
}DAC_CFG_D_SET_T;





void mcp4728_single_write_reg_and_eeprom(DAC_CFG_D_SET_T p_dac_d_set);
void mcp4728_multi_write_only_reg(DAC_CFG_D_SET_T *p_dac_d_set,mo_u8 len);


#ifdef __cplusplus
}
#endif

#endif //MCP4728__H_H


