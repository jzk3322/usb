/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f421_wk_config.c
  * @brief    work bench config program
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
#include "at32f421_dma.h"
#include "at32f421_wk_config.h"

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

/**
  * @brief  system clock config program
  * @note   the system clock is configured as follow:
  *         system clock (sclk)   = hick / 12 * pll_mult
  *         system clock source   = HICK_VALUE
  *         - sclk                = 120000000
  *         - ahbdiv              = 1
  *         - ahbclk              = 120000000
  *         - apb1div             = 1
  *         - apb1clk             = 120000000
  *         - apb2div             = 1
  *         - apb2clk             = 120000000
  *         - pll_mult            = 30
  *         - flash_wtcyc         = 3 cycle
  * @param  none
  * @retval none
  */
void wk_system_clock_config(void)
{
  /* reset crm */
  crm_reset();

  /* config flash psr register */
  flash_psr_set(FLASH_WAIT_CYCLE_3);

  /* enable lick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LICK, TRUE);

  /* wait till lick is ready */
  while(crm_flag_get(CRM_LICK_STABLE_FLAG) != SET)
  {
  }

  /* enable hick */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

  /* wait till hick is ready */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* config pll clock resource */
  crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* config ahbclk */
  crm_ahb_div_set(CRM_AHB_DIV_1);

  /* config apb2clk */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* config apb1clk */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);

  /* update system_core_clock global variable */
  system_core_clock_update();
}

/**
  * @brief  config systick and enable interrupt.
  * @param  none
  * @retval none
  */
uint32_t systick_interrupt_config(uint32_t ticks)
{
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);
  }

  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
  SysTick->VAL   = 0UL;
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return (0UL);
}

/**
  * @brief  config periph clock
  * @param  none
  * @retval none
  */
void wk_periph_clock_config(void)
{
      /* enable dma1 periph clock */
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  /* enable gpioa periph clock */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* enable gpiob periph clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* enable gpiof periph clock */
  crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);

  /* enable scfg periph clock */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

  /* enable spi1 periph clock */
  crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);

  /* enable usart1 periph clock */
  crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);

  /* enable tmr15 periph clock */
  crm_periph_clock_enable(CRM_TMR15_PERIPH_CLOCK, TRUE);

  /* enable tmr3 periph clock */
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);

  
  /* enable tmr14 periph clock */
  crm_periph_clock_enable(CRM_TMR14_PERIPH_CLOCK, TRUE);

  /* enable i2c1 periph clock */
  crm_periph_clock_enable(CRM_I2C1_PERIPH_CLOCK, TRUE);
}

/**
  * @brief  nvic config
  * @param  none
  * @retval none
  */
void wk_nvic_config(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  nvic_irq_enable(SysTick_IRQn, 0, 0);
  nvic_irq_enable(EXINT1_0_IRQn, 0, 0);
  nvic_irq_enable(USART1_IRQn, 0, 0);
}

/**
  * @brief  init gpio_input/gpio_output/gpio_analog/eventout function.
  * @param  none
  * @retval none
  */
void wk_gpio_config(void)
{
  /* add user code begin gpio_config 0 */

  /* add user code end gpio_config 0 */

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin gpio_config 1 */

  /* add user code end gpio_config 1 */

  /* gpio input config */
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_2;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);

  /* gpio output config */
  gpio_bits_reset(GPIOA, GPIO_PINS_6 | GPIO_PINS_13 | GPIO_PINS_14 | GPIO_PINS_4);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_13 | GPIO_PINS_14|GPIO_PINS_4;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  /* add user code begin gpio_config 2 */
#if !EN_TMR3__USR
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PINS_1;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);
    gpio_bits_write(GPIOB,GPIO_PINS_1,1);
#endif
  /* add user code end gpio_config 2 */
}

/**
  * @brief  init exint function.
  * @param  none
  * @retval none
  */
void wk_exint_config(void)
{
  /* add user code begin exint_config 0 */

  /* add user code end exint_config 0 */

  gpio_init_type gpio_init_struct;
  exint_init_type exint_init_struct;

  /* add user code begin exint_config 1 */

  /* add user code end exint_config 1 */

  /* configure the EXINT0 */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);

  scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
  exint_init(&exint_init_struct);
  
  /**
   * Users need to configure EXINT0 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding EXINT0 interrupt.
   *     --exint_interrupt_enable(EXINT_LINE_0, TRUE);
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void EXINT1_0_IRQHandler(void)
   */

  /* add user code begin exint_config 2 */

  /* add user code end exint_config 2 */
}

/**
  * @brief  init i2c1 function.
  * @param  none
  * @retval none
  */
void wk_i2c1_init(void)
{
  /* add user code begin i2c1_init 0 */

  /* add user code end i2c1_init 0 */

  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin i2c1_init 1 */

  /* add user code end i2c1_init 1 */

  /* configure the SCL pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init(GPIOF, &gpio_init_struct);

  gpio_pin_mux_config(GPIOF, GPIO_PINS_SOURCE1, GPIO_MUX_1);

  /* configure the SDA pin */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOF, &gpio_init_struct);

  gpio_pin_mux_config(GPIOF, GPIO_PINS_SOURCE0, GPIO_MUX_1);

  i2c_init(I2C1, I2C_FSMODE_DUTY_2_1, 100000);
  i2c_own_address1_set(I2C1, I2C_ADDRESS_MODE_7BIT, 0x5a);
  i2c_ack_enable(I2C1, TRUE);
  i2c_clock_stretch_enable(I2C1, TRUE);
  i2c_general_call_enable(I2C1, FALSE);

  i2c_enable(I2C1, TRUE);

  /* add user code begin i2c1_init 2 */

  /* add user code end i2c1_init 2 */
}

/**
  * @brief  init spi1 function
  * @param  none
  * @retval none
  */
void wk_spi1_init(spi_frame_bit_num_type w_width)
{
  /* add user code begin spi1_init 0 */

  /* add user code end spi1_init 0 */

  gpio_init_type gpio_init_struct;
  spi_init_type spi_init_struct;

  gpio_default_para_init(&gpio_init_struct);
  spi_default_para_init(&spi_init_struct);

  /* add user code begin spi1_init 1 */

  /* add user code end spi1_init 1 */

  /* configure the SCK pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_5;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE5, GPIO_MUX_0);

  /* configure the MOSI pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_0);

  /* configure the CS pin */
//   gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
//   gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
//   gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
//   gpio_init_struct.gpio_pins = GPIO_PINS_4;
//   gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
//   gpio_init(GPIOA, &gpio_init_struct);

//   gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_0);

  /* configure param */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.frame_bit_num = w_width;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_4;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI1, &spi_init_struct);

  /* configure the cs pin output */
  spi_hardware_cs_output_enable(SPI1, TRUE);

  spi_i2s_dma_transmitter_enable(SPI1, TRUE);

  spi_enable(SPI1, TRUE);

  /* add user code begin spi1_init 2 */

  /* add user code end spi1_init 2 */
}
/**
  * @brief  init dma1 channel1 for "spi1_tx"
  * @param  none
  * @retval none
  */
void wk_dma1_channel3_init(confirm_state mem_inc)
{
  dma_init_type dma_init_struct;

  dma_reset(DMA1_CHANNEL3);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = mem_inc;
  dma_init_struct.peripheral_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL3, &dma_init_struct);

  /* flexible function enable */
//   dma_flexible_config(DMA1, FLEX_CHANNEL1, DMA_FLEXIBLE_SPI1_TX);
}

/**
  * @brief  config dma channel transfer parameter
  * @param  none
  * @retval none
  */
void wk_dma_channel_config(dma_channel_type* dmax_channely, uint32_t peripheral_base_addr, uint32_t memory_base_addr, uint16_t buffer_size)
{
  dmax_channely->dtcnt = buffer_size;
  dmax_channely->paddr = peripheral_base_addr;
  dmax_channely->maddr = memory_base_addr;
}
/**
  * @brief  init tmr3 function.
  * @param  none
  * @retval none
  */
void wk_tmr3_init(void)
{
  /* add user code begin tmr3_init 0 */

  /* add user code end tmr3_init 0 */
#if EN_TMR3__USR
  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr3_init 1 */

  /* add user code end tmr3_init 1 */

  /* configure the tmr3 CH4 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE1, GPIO_MUX_1);

  /* configure counter settings */
  tmr_base_init(TMR3, 1000, 0);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR3, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR3, FALSE);
  tmr_primary_mode_select(TMR3, TMR_PRIMARY_SEL_RESET);

  /* configure channel 4 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, 500);
  tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_4, FALSE);

  tmr_output_channel_immediately_set(TMR3, TMR_SELECT_CHANNEL_4, FALSE);


  tmr_counter_enable(TMR3, TRUE);

  /* add user code begin tmr3_init 2 */

  /* add user code end tmr3_init 2 */
  #endif

}

/**
  * @brief  init tmr14 function.
  * @param  none
  * @retval none
  */
void wk_tmr14_init(void)
{
  /* add user code begin tmr14_init 0 */

  /* add user code end tmr14_init 0 */
#if !EN_TMR3__USR
  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr14_init 1 */

  /* add user code end tmr14_init 1 */

  /* configure the tmr14 CH1 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_4;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_4);

  /* configure counter settings */
  tmr_base_init(TMR14, VOUT_PWM_MAX, 0);
  tmr_cnt_dir_set(TMR14, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR14, TMR_CLOCK_DIV1);
  tmr_period_buffer_enable(TMR14, FALSE);

  /* configure channel 1 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;

  tmr_output_channel_config(TMR14, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR14, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(TMR14, TMR_SELECT_CHANNEL_1, FALSE);

  tmr_output_channel_immediately_set(TMR14, TMR_SELECT_CHANNEL_1, TRUE);

  tmr_counter_enable(TMR14, TRUE);
#endif
  /* add user code begin tmr14_init 2 */

  /* add user code end tmr14_init 2 */
}

/**
  * @brief  init tmr15 function.
  * @param  none
  * @retval none
  */
void wk_tmr15_init(void)
{
    #if 0
  /* add user code begin tmr15_init 0 */

  /* add user code end tmr15_init 0 */

  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_struct;

  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin tmr15_init 1 */

  /* add user code end tmr15_init 1 */

  /* configure the tmr15 CH2 pin */
  gpio_init_struct.gpio_pins = GPIO_PINS_3;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_0);

  /* configure counter settings */
  tmr_base_init(TMR15, PWM_MAX, 0);
  tmr_cnt_dir_set(TMR15, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR15, TMR_CLOCK_DIV2);
  tmr_repetition_counter_set(TMR15, 0);
  tmr_period_buffer_enable(TMR15, FALSE);

  /* configure primary mode settings */
  tmr_sub_sync_mode_set(TMR15, FALSE);
  tmr_primary_mode_select(TMR15, TMR_PRIMARY_SEL_RESET);

  /* configure channel 2 output settings */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_channel_config(TMR15, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(TMR15, TMR_SELECT_CHANNEL_2, 200);
  tmr_output_channel_buffer_enable(TMR15, TMR_SELECT_CHANNEL_2, FALSE);

  tmr_output_channel_immediately_set(TMR15, TMR_SELECT_CHANNEL_2, FALSE);

  /* configure break and dead-time settings */
  tmr_brkdt_struct.brk_enable = FALSE;
  tmr_brkdt_struct.auto_output_enable = FALSE;
  tmr_brkdt_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_LOW;
  tmr_brkdt_struct.fcsoen_state = FALSE;
  tmr_brkdt_struct.fcsodis_state = FALSE;
  tmr_brkdt_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_struct.deadtime = 0;
  tmr_brkdt_config(TMR15, &tmr_brkdt_struct);

  tmr_output_enable(TMR15, TRUE);

  tmr_counter_enable(TMR15, TRUE);

  /* add user code begin tmr15_init 2 */

  /* add user code end tmr15_init 2 */
  #endif
  
}

/**
  * @brief  init usart1 function
  * @param  none
  * @retval none
  */
void wk_usart1_init(void)
{
  /* add user code begin usart1_init 0 */

  /* add user code end usart1_init 0 */

  gpio_init_type gpio_init_struct;
  gpio_default_para_init(&gpio_init_struct);

  /* add user code begin usart1_init 1 */

  /* add user code end usart1_init 1 */

  /* configure the TX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_1);

  /* configure the RX pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_10;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE10, GPIO_MUX_1);

  /* configure param */
  usart_init(USART1, 1000000, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(USART1, TRUE);
  usart_receiver_enable(USART1, TRUE);
  usart_parity_selection_config(USART1, USART_PARITY_NONE);

  usart_hardware_flow_control_set(USART1, USART_HARDWARE_FLOW_NONE);

  /**
   * Users need to configure USART1 interrupt functions according to the actual application.
   * 1. Call the below function to enable the corresponding USART1 interrupt.
   *     --usart_interrupt_enable(...)
   * 2. Add the user's interrupt handler code into the below function in the at32f421_int.c file.
   *     --void USART1_IRQHandler(void)
   */
  /* enable usart2 and usart3 interrupt */
  usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);

  usart_enable(USART1, TRUE);

  /* add user code begin usart1_init 2 */


  /* add user code end usart1_init 2 */
}

/* add user code begin 1 */

/* add user code end 1 */
