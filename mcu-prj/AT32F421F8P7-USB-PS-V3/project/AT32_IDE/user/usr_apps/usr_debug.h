/**************************************************************************
 * @file usr_debug.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-10
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/

#ifndef USR_DEBUG__H_H
#define USR_DEBUG__H_H
#include<stdint.h>
#include<stdio.h>
#include<string.h>

#ifdef __cplusplus
extern "C"{
#endif


typedef void (*usr_dbg_char_out)(char dat);

#define USR_DBG_PRINT_EN_INFOR     0
#define USR_DBG_PRINT_EN_ERROR     0
#define USR_DBG_PRINT_EN_WARNING   0
#define USR_DBG_PRINT_EN_DEBUG     0
#define USR_DBG_PRINT_EN_PRINT_BUF 0

#define USR_DBG_PRINT_EN  (USR_DBG_PRINT_EN_INFOR|USR_DBG_PRINT_EN_ERROR|USR_DBG_PRINT_EN_WARNING|USR_DBG_PRINT_EN_DEBUG|USR_DBG_PRINT_EN_PRINT_BUF)

#if USR_DBG_PRINT_EN_INFOR
#define USR_DBG_INFO(format, ...) printf(format, ##__VA_ARGS__)
#else
#define USR_DBG_INFO(format, ...)
#endif //USR_DBG_PRINT_EN_INFOR

#if USR_DBG_PRINT_EN_ERROR
#define USR_DBG_ERROR(format, ...) printf("[error][%s:%d]" format, __func__, __LINE__, ##__VA_ARGS__)
#else
#define USR_DBG_ERROR(format, ...)
#endif //USR_DBG_PRINT_EN_ERROR

#if USR_DBG_PRINT_EN_WARNING
#define USR_DBG_WARNING(format, ...) printf("[waring][%s:%d]" format, __func__, __LINE__, ##__VA_ARGS__)
#else
#define USR_DBG_WARNING(format, ...)
#endif //USR_DBG_PRINT_EN_WARNING

#if USR_DBG_PRINT_EN_DEBUG
#define USR_DBG_DBUG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define USR_DBG_DBUG(format, ...)
#endif //USR_DBG_PRINT_EN_DEBUG

#if USR_DBG_PRINT_EN_PRINT_BUF
#define USR_DBG_PRING_BUF(buf,len) do{\
  int i;\
  for(i=0;i<(len);i++){\
      printf("%x ",(buf)[i]);\
  }\
  printf("\r\n");\
}while(0);
#else
#define USR_DBG_PRING_BUF(buf,len)
#endif //USR_DBG_PRINT_EN_PRINT_BUF


void usr_debug_print_register(usr_dbg_char_out char_out);


#ifdef __cplusplus
}
#endif

#endif //USR_DEBUG__H_H


