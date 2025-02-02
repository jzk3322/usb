/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f421_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f421_int.h"
#include "mo_input.h"
#include "mo_msg.h"
#include "usr_input_task.h"
#include "usr_main_app.h"
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

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

  /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */

    /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
  /* add user code begin PendSV_IRQ 0 */

  /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */

  /* add user code end PendSV_IRQ 1 */
}

/**
 * @brief  this function handles systick handler.
 * @param  none
 * @retval none
 */
extern void mo_tick_inc(void);
extern void out_vol_pudate_handle(void);

void SysTick_Handler(void) { 
    mo_tick_inc(); 
    out_vol_pudate_handle();
}

/**
 * @brief  this function handles EXINT Line 1 & 0 handler.
 * @param  none
 * @retval none
 */
void EXINT1_0_IRQHandler(void) {
    /* add user code begin EXINT1_0_IRQ 0 */

    /* add user code end EXINT1_0_IRQ 0 */
    /* add user code begin EXINT1_0_IRQ 1 */
    if (exint_interrupt_flag_get(EXINT_LINE_0) != RESET) {
        mo_msg_cancel_all(get_usr_input_task(), USR_ROTATE_INT_TOGGLE);
        mo_msg_send(get_usr_input_task(), USR_ROTATE_INT_TOGGLE, 0, 0);
        exint_flag_clear(EXINT_LINE_0);
    }
    /* add user code end EXINT1_0_IRQ 1 */
}

/**
 * @brief  this function handles USART1 handler.
 * @param  none
 * @retval none
 */
#define USART_RX_BUFFER_SIZE 256
static uint8_t usart2_rx_buffer[USART_RX_BUFFER_SIZE];
static uint16_t usart2_rx_counter = 0;
void USART1_IRQHandler(void) {
    /* add user code begin USART1_IRQ 0 */

    /* add user code end USART1_IRQ 0 */
    /* add user code begin USART1_IRQ 1 */
    uint8_t dat;
    uint8_t *p_dat;

    if (usart_interrupt_flag_get(USART1, USART_RDBF_FLAG) != RESET) {
        /* read one byte from the receive data register */
        dat = usart_data_receive(USART1);
        usart2_rx_buffer[usart2_rx_counter++] = dat;
        if (dat == '\n') {
            p_dat = mo_malloc(usart2_rx_counter + 1);
            if (p_dat) {
                memcpy(p_dat, usart2_rx_buffer, usart2_rx_counter);
                p_dat[usart2_rx_counter] = '\0';
                mo_msg_send(get_usr_main_task(), USR_UART_RX_RECEIVED_DATAS, p_dat, 0);
            }
            usart2_rx_counter = 0;
        }
        if (usart2_rx_counter >= USART_RX_BUFFER_SIZE) {
            usart2_rx_counter = 0;
        }
    }

    /* add user code end USART1_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
