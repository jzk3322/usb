/**
 * @file usr_i2c.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef USR_I2C___C_HHH__HH_
#define USR_I2C___C_HHH__HH_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include "at32f421_conf.h"
#include "i2c_application.h"

#define I2Cx_PORT                        I2C1

void usr_i2c_init(void);
i2c_status_type usr_i2c_master_receive(uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout);
i2c_status_type usr_i2c_master_transmit(uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout);


#ifdef __cplusplus
}
#endif

#endif