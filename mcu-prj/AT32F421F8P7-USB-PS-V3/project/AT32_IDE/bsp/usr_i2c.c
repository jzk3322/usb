/**
 * @file usr_i2c.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mo_config.h"
#include "usr_i2c.h"

static i2c_handle_type hi2cx;

/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
 extern void wk_i2c1_init(void);

void i2c_lowlevel_init(i2c_handle_type* hi2c) {

    if (hi2c->i2cx == I2Cx_PORT) {
        wk_i2c1_init();
    }
}



void usr_i2c_init(void){

    hi2cx.i2cx = I2Cx_PORT;
    i2c_config(&hi2cx);

}

i2c_status_type usr_i2c_master_transmit(uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout){
    i2c_status_type i2c_status;
    mo_u32 int_flg=mo_critical_enter();
    i2c_status = i2c_master_transmit(&hi2cx, address, pdata, size, timeout);
    mo_critical_exit(int_flg);
    return i2c_status;

}


i2c_status_type usr_i2c_master_receive(uint16_t address, uint8_t* pdata, uint16_t size, uint32_t timeout){
    i2c_status_type i2c_status;
    mo_u32 int_flg=mo_critical_enter();
    i2c_status = i2c_master_receive(&hi2cx, address, pdata, size, timeout);
    mo_critical_exit(int_flg);
    return i2c_status;
}


#if 0
void usr_i2c_test(void) {
    i2c_status_type i2c_status;
    uint8_t tx_buf[BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t rx_buf[BUF_SIZE] = {0};

    /* start the request reception process */
    if ((i2c_status = i2c_master_transmit(&hi2cx, I2Cx_ADDRESS, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
        // error_handler(i2c_status);
    }

    // delay_ms(10);

    /* start the request reception process */
    if ((i2c_status = i2c_master_receive(&hi2cx, I2Cx_ADDRESS, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK) {
        // error_handler(i2c_status);
    }
}
#endif 

