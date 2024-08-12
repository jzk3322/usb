/**
 * @file mo_mcu.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef MO_MCU_______HHHH_
#define MO_MCU_______HHHH_
#ifdef __cplusplus
extern "C" {
#endif

typedef mo_u32 mo_bool;

void mo_reset_mcu(void);
void mo_1ms_timer_nit(void);
mo_u32 mo_get_tick(void);
mo_u32 mo_critical_enter(void);
void mo_critical_exit(mo_u32 prev_int_state);
void mo_tick_inc(void);
void mo_delay_tick(mo_u32 delay_tick);

#ifdef __cplusplus
}
#endif

#endif
