/**************************************************************************
 * @file lcd_init.c
 * @author BryanHe(���) (532352406@qq.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-21
 * 
 * @copyright Copyright (c) 2024
 * 
 *************************************************************************/

#include "lcd_init.h"
void LCD_GPIO_Init(void) {
    // GPIO_InitTypeDef  GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
    // GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA
    // GPIO_SetBits(GPIOA,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
}

void delay(int t) {
    while (t--)
        ;
}
/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
******************************************************************************/
void LCD_Writ_Bus(u8 dat) {
    LCD_CS_Clr();
    ///MO_LOG("KKKKK\r\n");
    while (spi_i2s_flag_get(SPI1, SPI_I2S_TDBE_FLAG) == RESET);  // �����ձ�־λ
    //MO_LOG("KKKKK7\r\n");
    spi_i2s_data_transmit(SPI1, dat);
    // delay(3);
    LCD_CS_Set();
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat) { 
    while (spi_i2s_flag_get(SPI1, SPI_I2S_BF_FLAG) == SET);  // spi or i2s busy flag
    LCD_DC_Set();  // д����
    LCD_Writ_Bus(dat); 
    }

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat) {
    while (spi_i2s_flag_get(SPI1, SPI_I2S_BF_FLAG) == SET);  // spi or i2s busy flag
    LCD_DC_Set();  // д����
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
}

/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat) {
    while (spi_i2s_flag_get(SPI1, SPI_I2S_BF_FLAG) == SET);  // spi or i2s busy flag
    LCD_DC_Clr();  // д����
    LCD_Writ_Bus(dat);
    
}

/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2) {
    if (USE_HORIZONTAL == 0) {
        LCD_WR_REG(0x2a);  // �е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);  // �е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);  // ������д
    } else if (USE_HORIZONTAL == 1) {
        LCD_WR_REG(0x2a);  // �е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);  // �е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);  // ������д
    } else if (USE_HORIZONTAL == 2) {
        LCD_WR_REG(0x2a);  // �е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);  // �е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);  // ������д
    } else {
        LCD_WR_REG(0x2a);  // �е�ַ����
        LCD_WR_DATA(x1);
        LCD_WR_DATA(x2);
        LCD_WR_REG(0x2b);  // �е�ַ����
        LCD_WR_DATA(y1);
        LCD_WR_DATA(y2);
        LCD_WR_REG(0x2c);  // ������д
    }
}

void LCD_Init(void) {
    USR_DBG_INFO("init start\r\n");
   // SPI1_Init();
    // LCD_GPIO_Init();  // ��ʼ��GPIO
    LCD_DC_Set();  // д����
    LCD_RES_Clr();  // ��λ
    // USR_DBG_INFO("1\r\n");
    MO_DELAY_TICK(200);
    // USR_DBG_INFO("2\r\n");
    LCD_RES_Set();
    MO_DELAY_TICK(100);
    // USR_DBG_INFO("3\r\n");
    LCD_BLK_Set();  // �򿪱���
    // MO_DELAY_TICK(100);
    USR_DBG_INFO("init reg\r\n");
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11);  // Sleep out
    MO_DELAY_TICK(120);     // Delay 120ms
    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x36);
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x32);  // Vcom=1.35V

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x15);  // GVDD=4.8V  ��ɫ���

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);  // VDV, 0x20:0v

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);  // 0x0F:60Hz

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x08);
    LCD_WR_DATA8(0x0E);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x09);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x48);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x15);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x34);
    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
    USR_DBG_INFO("led init end \r\n");

}
