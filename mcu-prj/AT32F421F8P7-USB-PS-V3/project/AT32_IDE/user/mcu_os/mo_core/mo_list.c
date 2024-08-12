/**
 * @file mo_list.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-31
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "mo_list.h"

mo_u8 dlist_ins_next(D_LIST_T *dlist, D_LIST_ELMT_T *element, D_LIST_ELMT_T *new_element, void *dat) {
    // D_LIST_ELMT_T *new_element;

    /*Do not allow a NULL element unless the list is empty. **/
    if (element == NULL && dlist->size != 0) return MO_FALSE;

    /*Allocate storage for the element. */
    if (new_element == NULL) return MO_FALSE;

    /**Insert the new element into the list. **/
    new_element->dat = dat;

    if (dlist->size == 0) {
        /*Handle insertion when the list is empty. **/
        dlist->head = new_element;
        dlist->head->prev = NULL;
        dlist->head->next = NULL;
        dlist->tail = new_element;
    } else {
        /*Handle insertion when the list is not empty. */
        new_element->next = element->next;
        new_element->prev = element;

        if (element->next == NULL)
            dlist->tail = new_element;
        else
            element->next->prev = new_element;

        element->next = new_element;
    }
    /**Adjust the size of the list to account for the inserted element. */
    dlist->size++;

    return MO_TRUE;
}
D_LIST_ELMT_T *dlist_remove(D_LIST_T *list, D_LIST_ELMT_T *element) {
    D_LIST_ELMT_T *old_element = element;

    if (element == NULL || list->size == 0) return NULL;

    /*Remove the element from the list. */
    if (element == list->head) {
        /*Handle removal from the head of the list. */
        list->head = element->next;

        if (list->head == NULL)
            list->tail = NULL;
        else
            element->next->prev = NULL;
    } else {
        /*Handle removal from other than the head of the list. */
        element->prev->next = element->next;
        if (element->next == NULL)
            list->tail = element->prev;
        else
            element->next->prev = element->prev;
    }
    /*free the storage allocated by the abstract datatype. */

    /*Adjust the size of the list to account for the removed element. */
    list->size--;

    return old_element;
}
