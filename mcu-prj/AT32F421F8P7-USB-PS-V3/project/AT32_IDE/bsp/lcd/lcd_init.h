/**************************************************************************
 * @file lcd_init.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/
#ifndef LCD_INIT__H_H
#define LCD_INIT__H_H
#include<stdint.h>
#include<stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
#include "at32f421.h"
#include "mo_config.h"

#define USE_HORIZONTAL 3  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 320
#else
#define LCD_W 320
#define LCD_H 240
#endif



//-----------------LCD�˿ڶ���----------------

#define LCD_RES_Clr() gpio_bits_reset(GPIOA, GPIO_PINS_13)  // RES
#define LCD_RES_Set() gpio_bits_set(GPIOA, GPIO_PINS_13)

#define LCD_DC_Clr() gpio_bits_reset(GPIOA, GPIO_PINS_6)  // DC
#define LCD_DC_Set() gpio_bits_set(GPIOA, GPIO_PINS_6)

#define LCD_CS_Clr() //gpio_bits_reset(GPIOB, GPIO_PINS_4)  // CS
#define LCD_CS_Set() //gpio_bits_set(GPIOB, GPIO_PINS_4)

#define LCD_BLK_Clr() //gpio_bits_reset(GPIOA, GPIO_PINS_14)  // BLK
#define LCD_BLK_Set() //gpio_bits_set(GPIOA, GPIO_PINS_14)

void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��


#ifdef __cplusplus
}
#endif

#endif //LCD_INIT__H_H



