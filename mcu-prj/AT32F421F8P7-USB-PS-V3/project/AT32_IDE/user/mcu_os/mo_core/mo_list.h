/**
 * @file mo_list.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef MO_LIST_HHHHHHHHH_
#define MO_LIST_HHHHHHHHH_
#ifdef __cplusplus
extern "C" {
#endif

#include "../mcu/mo_config.h"

/**Define a structure for linked list elements**/
typedef struct list_elmt_ {
    void *dat;
    struct list_elmt_ *next;
} LIST_ELMT_T;
/*Define a structure for linked lists*/
typedef struct list_ {
    mo_u32 size;
    LIST_ELMT_T *head;
    LIST_ELMT_T *tail;
} LIST_T;
/*******************/

/**Define a structure for doubly-linked list elements**/
typedef struct d_list_elmt_ {
    void *dat;
    struct d_list_elmt_ *prev;
    struct d_list_elmt_ *next;
} D_LIST_ELMT_T;
/*Define a structure for doubly-linked lists**/
typedef struct DList_ {
    mo_u32 size;
    D_LIST_ELMT_T *head;
    D_LIST_ELMT_T *tail;
} D_LIST_T;
/******************/

D_LIST_ELMT_T *dlist_remove(D_LIST_T *list, D_LIST_ELMT_T *element);
mo_u8 dlist_ins_next(D_LIST_T *dlist, D_LIST_ELMT_T *element, D_LIST_ELMT_T *new_element, void *dat);
#ifdef __cplusplus
}
#endif
#endif
