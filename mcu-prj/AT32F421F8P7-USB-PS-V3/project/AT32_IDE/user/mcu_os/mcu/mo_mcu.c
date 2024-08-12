/**
 * @file mo_mcu.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "at32f421_misc.h"
#include "cmsis_gcc.h"
#include "mo_config.h"

#ifdef WINDOWS_DEMO_EN
#include <Windows.h>
#endif

static volatile mo_u32 moTick = 0;
#define mo_tick moTick

/* #TODOThe os_tick_inc should be invoked in timer IRQ
 * function*********************/
void mo_tick_inc(void) { 
    mo_tick++; 
    //  USR_DBG_DBUG("%d\n",mo_tick);
}

void mo_reset_mcu(void) {
    /*#TODO: here to reset the MCU you own***/
    /*for at32f4xx*/
    nvic_system_reset(); /**/

}
void mo_delay_tick(mo_u32 delay_tick){
	mo_u32 t = mo_tick;
    mo_u32 delay=0;

    while(1){
        if(t!=mo_tick){
            t=mo_tick;
            delay++;
            // USR_DBG_DBUG("%d\r\n",delay);
            if(delay>=delay_tick)break;
        }
    }
    
}

/*#TODO: place mo_tick_timer_irq_handle() into the timer interrupt service of
the MCU you own *void mo_tick_timer_irq_handle(void){	mo_tick_inc();
}*/
/***********************************************/


/*********************************************************************
 * @fn      mo_1ms_timer_nit
 *
 * @brief   Initializes Systick.
 *
 * @return  none
 */
/*#TODO need to init a 1mS timer for mcu os tick*/
void mo_1ms_timer_nit(void) {
#ifdef MO_CHV32V30x
    SysTick->SR = 0;
    SysTick->CNT = 0;
    SysTick->CMP = SystemCoreClock / 8 / 1000;
    SysTick->CTLR = 0x3;
    Interrupt_VTF_Init();
#endif

#ifdef MO_CH58x
    TMR0_TimerInit(FREQ_SYS / 1000);        // 璁剧疆瀹氭椂鏃堕棿 1ms
    TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END);  // 寮�鍚腑鏂�
    PFIC_EnableIRQ(TMR0_IRQn);
#endif
}

#ifdef WINDOWS_DEMO_EN
void CALLBACK mo_tick_timer_irq_handle(HWND t, UINT s, UINT_PTR w, DWORD d) {
    // printf("timer callback \n");
    mo_tick_inc();
}

/*******************************Windows thread*******************************/
DWORD dwtimerID = 0;

DWORD CALLBACK thread(PVOID pvoid) {
    MSG msg;

    dwtimerID = SetTimer(NULL, 1, 1, mo_tick_timer_irq_handle);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TIMER) {
            // printf("i get a word: \n");
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    KillTimer(NULL, dwtimerID);

    return 0;
}
/****************************************************************************/

void mo_1ms_timer_nit(void) { /*for 1ms systick timer*/

    /*@TODO: here to init your own MCU timer as 1mS***/
    DWORD dwThreadId = 0;
    HANDLE hthread = CreateThread(NULL, 0, thread, 0, 0, &dwThreadId);
}

#endif

mo_u32 mo_get_tick(void) { return mo_tick; }

/**************
return val: TRUE indecats that the global interrupt is enabled
                   FALSE indecats that it is disabled
**************/
mo_u32 mo_get_global_int_state(void) {
    /**#TODO: Here to get the global interrupt status of the MCU you own****/
    return 1;
}

/**************
return ret: TRUE indecats that the global interrupt is enabled before
                   FALSE indecats that it is disabled before
**************/
mo_u32 mo_critical_enter(void) {
    mo_u32 ret;
    ret = mo_get_global_int_state();

    /**#TODO: here to disable the global interrupt of the MCU you own***/
    /*for stm32f0xx*/
    __disable_irq();
    return ret;
}

/**************
parameter prev_int_state: the previous int state that is returned by call
critical_enter() function
**************/
void mo_critical_exit(mo_u32 prev_int_state) {

    if (prev_int_state) {
        /**#TODO: here to enable the global interrupt of the MCU you own***/
        /*for stm32f0xx*/
        __enable_irq();
  }
}


#if MO_DEBUG_EN
// /*@TODO binding printf fun. here below*/
// extern UART_HandleTypeDef huart2;
// /*this is for Keil&IAR IDE*/

// int fputc(int ch, FILE *f){
//   HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
//   return ch;
// }

// /*This is for other IDE such as: GCC*/
// // int __io_putchar(int ch){
// //   HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
// //   return ch;
// // }
#endif

#if 0 /*here below code is used to bind the uart to printf() function to print log information via uart*/
        
#if MO_DEBUG_EN
#include "stdio.h"
//#include "Usart.h"
#ifdef OS_FOR_STM32
//#include "cmsis_gcc.h"
#include "stm32f072xb.h"
#endif

#pragma import(__use_no_semihosting)
/* 鏍囧噯搴撻渶瑕佺殑鏀寔鍑芥暟 */
struct __FILE {
  int handle;
};

FILE __stdout;
/* 瀹氫箟_sys_exit()浠ラ伩鍏嶄娇鐢ㄥ崐涓绘満妯″紡     */
void _sys_exit(int x) { x = x; }
/* 閲嶅畾涔塮putc鍑芥暟 */
int fputc(int ch, FILE *f) {
  /*Hear to provide the UART send data function
    for example as below is for stm32f072xb*/
  /* 閫夋嫨USART1鍙戦�� */
  while ((USART1->ISR & 0X40) == 0);  // 寰幆鍙戦��,鐩村埌鍙戦�佸畬姣�
  USART1->TDR = (uint8_t)ch;
  
  return ch;
}
#endif
#endif 
