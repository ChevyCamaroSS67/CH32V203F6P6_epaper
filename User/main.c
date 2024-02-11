/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART2_Tx(PA2).
 This example demonstrates using USART2_Tx(PA2) as a print debug port output.

*/

#include "debug.h"
#include "epaper.h"
#include "bmp.h"
#include "msp.h"

/* Global typedef */

/* Global define */

/* Global Variable */

extern struct EpdInstance epd1;

uint8_t image_bw[EPD_154_W_BUFF_SIZE * EPD_154_H];

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("Epaper Module Test\r\n");

    bool err = epd_pre_init(&epd1);
    err = !err && epd_init(&epd1);
    if (err)
        while(1);

    epd_paint_newimage(image_bw, EPD_154_W, EPD_154_H, EPD_ROTATE_0, EPD_COLOR_WHITE);
    epd_paint_clear(EPD_COLOR_WHITE);

    epd_paint_showString(10, 29, (uint8_t *)&"Designed By WeAct Studio", EPD_FONT_SIZE16x8, EPD_COLOR_BLACK);

    uint8_t text[20];
    sprintf((char *)&text, ">> Hello World.");
    epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_RED);
    epd_paint_showString(10,100,(uint8_t *)&"CH32V203F6P6 Example",EPD_FONT_SIZE16x8,EPD_COLOR_RED);

    epd_displayBW(&epd1, image_bw);
    epd_enter_deepsleepmode(&epd1, EPD_DEEPSLEEP_MODE1);
    Delay_Ms(5000);
for(;;);

    epd_paint_clear(EPD_COLOR_WHITE);
    epd_paint_showPicture((EPD_154_H - 200) / 2,(EPD_154_W - 64) / 2,200,64,gImage_5,EPD_COLOR_WHITE);
    epd_displayBW(&epd1, image_bw);
    epd_enter_deepsleepmode(&epd1, EPD_DEEPSLEEP_MODE1);

    Delay_Ms(5000);

    epd_init_partial(&epd1);
    epd_paint_selectimage(image_bw);
    epd_paint_clear(EPD_COLOR_WHITE);
    epd_paint_showString(10, 0, (uint8_t *)&"1.54 Inch", EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_paint_showString(10, 23, (uint8_t *)&"Epaper Module", EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_paint_showString(10, 48, (uint8_t *)&"Designed By WeAct Studio", EPD_FONT_SIZE12x6, EPD_COLOR_BLACK);
    epd_paint_showString(10, 60, (uint8_t *)&"with 200 x 200 resolution", EPD_FONT_SIZE12x6, EPD_COLOR_BLACK);
    epd_paint_showPicture((EPD_154_H - 200) / 2,130,200,64,gImage_5,EPD_COLOR_WHITE);
    epd_paint_showString(10,100,(uint8_t *)&"CH32V203F6P6 Example",EPD_FONT_SIZE16x8,EPD_COLOR_BLACK);

    sprintf((char *)&text, ">> Partial Mode");
    epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_displayBW_partial(&epd1, image_bw);
    Delay_Ms(1000);

    for (uint32_t i = 123; i < 8 * 123; i += 123)
    {
        sprintf((char *)&text, ">> Num=%d     ", i);
        epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);

        epd_displayBW_partial(&epd1, image_bw);

        Delay_Ms(100);
    }

    sprintf((char *)&text, ">> Hello World.");
    epd_paint_showString(10, 71, text, EPD_FONT_SIZE24x12, EPD_COLOR_BLACK);
    epd_displayBW_partial(&epd1, image_bw);
    Delay_Ms(1000);
    epd_update(&epd1);
    epd_enter_deepsleepmode(&epd1, EPD_DEEPSLEEP_MODE1);

    while(1)
    {
    }
}
