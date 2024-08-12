/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f421_wk_config.h"
#include "mo_config.h"
#include "mo_heap.h"
#include "mo_msg.h"
#include "usr_i2c.h"
#include "ina226.h"
#include "usr_main_app.h"
#include "lcd_init.h"
#include "lcd.h"
#include "usr_desplay_task.h"
#include "usr_input_task.h"
#include "usr_debug.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */

/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */
static void uart_put_char(char dat) {
    while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(USART1, (uint16_t)(dat));
    while (usart_flag_get(USART1, USART_TDC_FLAG) == RESET);
}

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void) {
    /* add user code begin 1 */

    /* add user code end 1 */

    /* system clock config. */
    wk_system_clock_config();

    /* config periph clock. */
    wk_periph_clock_config();

    /* config systick clock source */
    systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8);
    /* system tick config */
    /**
     * use systick as time base source and configure 1ms tick.
     * users need add interrupt handler code into the below function in the at32f421_int.c file.
     *  --void SystTick_IRQHandler(void)
     */
    systick_interrupt_config(system_core_clock / 8 / 1000);
    // SysTick_Config(SystemCoreClock / 1000);

    /* nvic config. */
    wk_nvic_config();

      /* init dma1 channel3 */
    wk_dma1_channel3_init(TRUE);
        /* config dma channel transfer parameter */
    /* user need to modify parameters memory_base_addr and buffer_size */
    wk_dma_channel_config(USA_DMA_CHANNEL, (uint32_t)&SPI1->dt, 0, 0);
    dma_channel_enable(USA_DMA_CHANNEL, TRUE);
    /* init usart1 function. */
    wk_usart1_init();

    /* init spi1 function. */
    wk_spi1_init(SPI_FRAME_8BIT);

    /* init i2c1 function. */
    // wk_i2c1_init();
    usr_i2c_init();

    /* init exint function. */
    wk_exint_config();

    /* init gpio function. */
    wk_gpio_config();
    OUT_CTRL(0);

    /* init tmr3 function. */
    wk_tmr3_init();

    /* init tmr15 function. */
    wk_tmr15_init();

    /* add user code begin 2 */

    /* add user code end 2 */
    usr_debug_print_register(uart_put_char);

    USR_DBG_INFO("enter main\r\n");

    mo_msg_init();
    /*Tasks init start here*/
    usr_main_app_task_init();
    usr_display_task_init();
    usr_input_task_init(get_usr_main_task());
    /*Tasks init end here*/
    mo_msg_loop(NULL);
    /******************************/
}
