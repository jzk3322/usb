/**
 * @file mo_heap.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MCU_HEAP___HHH_
#define MCU_HEAP___HHH_

#ifdef __cplusplus
extern "C" {
#endif

#include "../mcu/mo_config.h"

typedef struct mcu_heap_ {
    mo_u8 used_map[MAX_MCU_SLOT];
    mo_u8 mcu_heap[MAX_MCU_HEAP];

    mo_u32 memory_used_in_slot;
} MCU_HEAP_T;

extern mo_u32 mo_memory_used_in_slot(void);
extern void mo_malloc_init(void);
extern void mo_free(const void *paddr); /*NOTE: DO NOT INVOKE THIS FUNCTION IN
                                           INTERRUPT SERVER*/
extern void *mo_malloc(mo_u32 m_size);  /*NOTE: DO NOT INVOKE THIS FUNCTION IN INTERRUPT SERVER*/

#ifdef __cplusplus
}
#endif

#endif
