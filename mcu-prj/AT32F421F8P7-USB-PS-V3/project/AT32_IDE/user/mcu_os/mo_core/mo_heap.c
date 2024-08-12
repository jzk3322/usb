/**
 * @file mo_heap.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "mo_heap.h"

static MCU_HEAP_T mcu_heap_t;

/*!
  @brief allocate a block memory from mcu_heap_t .
        NOTE:
  @param m_size defined how many bytes memory to allocate for.
  @return if the memory has been allocated successfully, that will return the
  memory address, else that will return NULL
*/

void *mo_malloc(mo_u32 m_size) { /**/
    mo_u32 i, free_slot_cnt, temp;
    mo_bool ret_it;
    mo_u8 *ret_add = NULL;
    mo_u32 slot = m_size / MCU_SLOT_SIZE;

    if (m_size == 0) return NULL;

    if (slot) {
        i = m_size % MCU_SLOT_SIZE;
        if (i) {
            slot++;
        }
    } else {
        slot = 1;
    }

    free_slot_cnt = 0;

    ret_it = MO_CRITICAL_ENTER();

    for (i = 0; i < MAX_MCU_SLOT; i++) {
        if (mcu_heap_t.used_map[i] == 0) { /*the slot is free?*/
            free_slot_cnt++;

            if (free_slot_cnt >= slot) {
                temp = (i + 1) - free_slot_cnt;
                mcu_heap_t.used_map[temp] = slot;
                ret_add = &mcu_heap_t.mcu_heap[temp * MCU_SLOT_SIZE];
                mcu_heap_t.memory_used_in_slot += slot; /**Bryan added on 2020-4-16***/
                break;
            }
        } else {
            i += mcu_heap_t.used_map[i] - 1;
            free_slot_cnt = 0;
        }
    }
    MO_CRITICAL_EXIT(ret_it);

    if (ret_add == NULL) /*Error occurred**/
    {
        // while(1);
        MO_FORCE_SYS_RESET(); /**/
    }
    return ret_add;
}

/*!
  @brief release a block memory from mcu_heap_t .
        NOTE:
  @param paddr point to memory address that has to release.

*/
void mo_free(const void *paddr) { /*NOTE: */
    mo_u32 i;
    mo_bool ret_it;

    if (paddr == NULL) return;

    ret_it = MO_CRITICAL_ENTER();

    if ((mo_u8 *)paddr > &mcu_heap_t.mcu_heap[(MAX_MCU_HEAP - 1)] || (mo_u8 *)paddr < mcu_heap_t.mcu_heap) return;

    for (i = 0; i < MAX_MCU_SLOT; i++) {
        if ((mo_u8 *)paddr == &mcu_heap_t.mcu_heap[i * MCU_SLOT_SIZE]) {
            mcu_heap_t.memory_used_in_slot -= mcu_heap_t.used_map[i]; /**Bryan added on 2020-4-16***/
            mcu_heap_t.used_map[i] = 0x00;
            break;
        }
    }
    MO_CRITICAL_EXIT(ret_it);
}

mo_u32 mo_memory_used_in_slot(void) { return mcu_heap_t.memory_used_in_slot; }

void mo_malloc_init(void) {
    mo_bool ret_it;
    ret_it = MO_CRITICAL_ENTER();
    memset(&mcu_heap_t, 0x00, sizeof(MCU_HEAP_T));
    MO_CRITICAL_EXIT(ret_it);
}
