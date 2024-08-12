/**************************************************************************
 * @file INA226.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/
#ifndef ina226__H_H
#define ina226__H_H

#include<stdint.h>
#include<stdio.h>


#ifdef __cplusplus
extern "C"{
#endif

#include "mo_config.h"
#include "usr_debug.h"
#include "usr_i2c.h"

#define ina226_INPUT_I2C_DEV_ADD (0x8a)
#define ina226_OUTPUT_I2C_DEV_ADD (0x80)

#define ina226_CFG_REG         0x00
#define ina226_CH1_SHUNT_V_REG 0X01
#define ina226_CH1_BUS_V_REG   0X02

#define ina226_CH2_SHUNT_V_REG 0X03
#define ina226_CH2_BUS_V_REG   0X04
#define ina226_CH3_SHUNT_V_REG 0X05
#define ina226_CH3_BUS_V_REG   0X06

/*Averaging mode. These bits set the number of samples that are
collected and averaged together.*/
typedef enum {
   NUM_1=0,// = 1 (default)
   NUM_4,// = 4
   NUM_16,// = 16
   NUM_64,// = 64
   NUM_128,// = 128
   NUM_256,// = 256
   NUM_512,// = 512
   NUM_1024,// = 1024
}AveragedTimes_T;

#define AVERAGE_MASKER_BITS 0x07
#define AVERAGE_FILED_START 9ul

void ina226_init(uint8_t dev_addr);
int ina226_read_shunt_voltage(uint8_t dev_addr,u8 shunt_reg,int *p_vol);
int ina226_read_bus_voltage(uint8_t dev_addr,u8 bus_reg,int *p_vol);

#ifdef __cplusplus
}
#endif

#endif //POWER_MONITOR__H_H


