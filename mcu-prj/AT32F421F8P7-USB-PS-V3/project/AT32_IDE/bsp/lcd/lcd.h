/**************************************************************************
 * @file lcd.h
 * @author BryanHe(贺璧) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/

#ifndef LCD__H_H
#define LCD__H_H
#include<stdint.h>
#include<stdio.h>

#ifdef __cplusplus
extern "C"{
#endif
#include "at32f421.h"
#include "at32f421_dma.h"
#include "at32f421_spi.h"
#include "at32f421_wk_config.h"
#include "mo_config.h"

void usr_LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//ָ�����������ɫ
void LCD_DrawPoint(u16 x,u16 y,u16 color);//��ָ��λ�û�һ����
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//��ָ��λ�û�һ����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//��ָ��λ�û�һ������
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//��ָ��λ�û�һ��Բ

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ���ִ�
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����12x12����
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����16x16����
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����24x24����
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ����32x32����

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾһ���ַ�
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//��ʾ�ַ���
u32 mypow(u8 m,u8 n);//����
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��������
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//��ʾ��λС������

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const uint16_t pic[]);//��ʾͼƬ


//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //��ɫ
#define BRRED 			     0XFC07 //�غ�ɫ
#define GRAY  			     0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			     0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

#define RGB_to_565(r,g,b) ((((r*31/255)&0x1f)<<11)|(((g*63/255)&0x3f)<<5)|((b*31/255)&0x1f))

#define DIS_BACKGROUND   (BLACK)
#define USB_PS_COLOR     RGB_to_565(255, 192, 0)
#define GRAY_LABEL_COLOR RGB_to_565(190, 90, 17)
#define RUN_NUM_COLOR    RGB_to_565(242, 242, 242)
#define IN_VOL_COLOR     RGB_to_565(98, 249, 251)
#define IN_CURR_COLOR    RGB_to_565(237, 125, 48)
#define OUT_VOL_COLOR    RGB_to_565(112, 173, 71)  // RGB_to_565(169, 209, 142)
#define OUT_CURR_COLOR   RGB_to_565(212, 148, 201)
#define OUT_PW_COLOR     RGB_to_565(255, 255, 0)
#define CFG_VOL_COLOR    RGB_to_565(207, 21, 219)
#define CFG_CURR_COLOR   RGB_to_565(56, 216, 109)
#define LINE_COLOR       RGB_to_565(118, 113, 113)
#define IN_FILL_COLOR    RGB_to_565(237, 125, 48)

#define USB_PS_OUT_ENABLE_COLOR  RGB_to_565(91, 155, 213)
#define USB_PS_OUT_DISABLE_COLOR RGB_to_565(192, 0, 0)
#define USB_PS_O_ON_TXT_COLOR    RGB_to_565(242, 242, 242)
#define USB_PS_O_OFF_TXT_COLOR   RGB_to_565(242, 242, 242)

#define M_VOLTAGE_COLOR  RGB_to_565(108, 236, 79)

#ifdef __cplusplus
}
#endif

#endif //LCD__H_H






