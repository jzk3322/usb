/**************************************************************************
 * @file usr_debug.c
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-06-10
 *
 * @copyright Copyright (c) 2024
 *
 *************************************************************************/
#include "usr_debug.h"

usr_dbg_char_out usr_char_out = NULL;

void usr_debug_print_register(usr_dbg_char_out char_out) { usr_char_out = char_out; }

#if defined(__GNUC__) && !defined(__CC_ARM)
/**
 *******************************************************************************
 ** \brief  Re-target _write function.
 **
 ** \param  [in] fd
 ** \param  [in] data
 ** \param  [in] size
 **
 ** \retval int32_t
 **
 ******************************************************************************/
int32_t _write(int fd, char data[], int32_t size) {
    int32_t i = -1;

    if (NULL != data) {
        (void)fd; /* Prevent unused argument compilation warning */

        for (i = 0; i < size; i++) {
            if (usr_char_out) {
                usr_char_out(data[i]);
            }else{
                break;
            }
        }
    }

    return i ? i : -1;
}

#else
/**
 *******************************************************************************
 ** \brief  Re-target fputc function.
 **
 ** \param  [in] ch
 ** \param  [in] f
 **
 ** \retval int32_t
 **
 ******************************************************************************/
int32_t fputc(int32_t ch, FILE *f) {
    (void)f; /* Prevent unused argument compilation warning */
    if (usr_char_out) {
        usr_char_out((char)ch);
    }else{
        return -1;
    }
    return ch;

    // return (Ok == UartPutChar(m_PrintfDevice, (char)ch)) ? ch: -1;
}
#endif
