/**************************************************************************
 * @file INA226.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-02
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/

#include "INA226.h"


int16_t ina226_convert_dat_to_int16(uint8_t *tmp) {
    uint16_t dat;
    int16_t m=1;
    int16_t ret;
    uint16_t signed_bit;

    dat = (uint16_t)tmp[0] << 8;
    dat |= tmp[1];

    // USR_DBG_DBUG("curr:%x\r\n",dat);

    signed_bit = (dat & 0x8000);
    if(signed_bit){//negative number in twos complement format
        // dat = (~dat)+1; // 
        dat = 0;
    }else{
    }
    // ret = dat*m;

    return dat;
}
/**************************************************************************
 * @brief Those register stores the bus voltage reading, VBUS, for channel x. Full-scale range = 32.76 V (decimal = 7FF8);
LSB (BD0) = 8 mV. Although the input range is 26 V, the full-scale range of the ADC scaling is 32.76 V. Do not
apply more than 26 V.
 * 
 * @param bus_reg The Register of Channel Bus Voltage
 * @param p_vol point to the int data, that is delivered the bus voltage, the unit in mV
 * @return int 0: successful; -1:error
 *************************************************************************/
int ina226_read_bus_voltage(u8 dev_addr,u8 bus_reg,int *p_vol){
    int ret=-1;
    uint8_t tmp[32];
    int16_t int16dat;
    i2c_status_type i2c_status;

    tmp[0] = bus_reg;
    i2c_status = usr_i2c_master_transmit(dev_addr, tmp, 1, 1000);

    if (i2c_status == I2C_OK) {
    } else {
        USR_DBG_INFO("ina3221 write error:%d\r\n", i2c_status);
    }

    i2c_status = usr_i2c_master_receive(dev_addr, tmp, 2,1000);

    if(i2c_status == I2C_OK){
        int16dat=ina226_convert_dat_to_int16(tmp);
        /*LSB (BD0) = 1.25 mV*/
        *p_vol = int16dat*1.25f;
        // USR_DBG_INFO("int16dat:%d\r\n", int16dat);
        ret=0;
    }else{
        USR_DBG_INFO("ina3221 read error:%d,ch_reg:0x%x\r\n",i2c_status,bus_reg);
    }

    return ret;
}
/**************************************************************************
 * @brief This register contains the averaged shunt-voltage measurement for channel 1. This register stores the current
shunt-voltage reading, VSHUNT, for channel 1. Negative numbers are represented in twos complement format.
Generate the twos complement of a negative number by complementing the absolute value binary number and
adding 1. Extend the sign, denoting a negative number by setting MSB = 1.
Full-scale range = 163.8 mV (decimal = 7FF8); LSB (SD0): 40 μV.
 * 
 * @param shunt_reg 
 * @param p_vol point to the int data, that is delivered the bus voltage, the unit in mV
 * @return int 0: successful; -1:error
 *************************************************************************/
int ina226_read_shunt_voltage(u8 dev_addr,u8 shunt_reg,int *p_vol){
    int ret=-1;
    uint8_t tmp[32];
    int16_t int16dat;
    i2c_status_type i2c_status;

    tmp[0] = shunt_reg;
    i2c_status = usr_i2c_master_transmit(dev_addr, tmp, 1, 1000);

    if (i2c_status == I2C_OK) {
    } else {
        USR_DBG_INFO("ina3221 write error:%d,ch_reg:0x%x\r\n", i2c_status, ina226_CFG_REG);
    }

    i2c_status = usr_i2c_master_receive(dev_addr, tmp, 2,1000);

    if(i2c_status == I2C_OK){
        int16dat=ina226_convert_dat_to_int16(tmp);
        /*LSB (SD0): 2.5 μV*/
        *p_vol = int16dat*25;
        ret=0;
    }else{
        USR_DBG_INFO("ina3221 read error:%d,ch_reg:0x%x\r\n",i2c_status,shunt_reg);
    }

    return ret;
}

/**************************************************************************
 * @brief 
 * 
 *************************************************************************/
void ina226_init(uint8_t dev_addr) {
    // Configuration Register (address = 00h) [reset = 7127h]
    //  0x7127

    USR_DBG_INFO("ina226 init\r\n");

    uint8_t tmp[10] = {0x71, 0x72};
    uint16_t default_val = 0x7127;
    i2c_status_type i2c_status;

    default_val &= ~(AVERAGE_MASKER_BITS << AVERAGE_FILED_START);

    default_val |= (NUM_16 << AVERAGE_FILED_START);
    tmp[0] = ina226_CFG_REG;
    tmp[1] = (default_val>>8)&0x00ff;
    tmp[2] = (default_val)&0x00ff;
    i2c_status = usr_i2c_master_transmit(dev_addr, tmp, 3, 1000);

    if (i2c_status == I2C_OK) {
    } else {
        USR_DBG_INFO("init write error:%d,ch_reg:0x%x\r\n", i2c_status, ina226_CFG_REG);
    }
}