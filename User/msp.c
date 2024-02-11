/*
 * msp.c
 *
 *  Created on: Feb 10, 2024
 *      Author: Chevy
 */

#include "ch32v20x.h"
#include "msp.h"

// epaper module
// res pin  -> PA0
#define GPIO_PIN_RES        GPIO_Pin_0
#define GPIO_PORT_RES       GPIOA
// busy pin -> PA1
#define GPIO_PIN_BUSY       GPIO_Pin_1
#define GPIO_PORT_BUSY      GPIOA
// d/c pin  -> PA12
#define GPIO_PIN_DC         GPIO_Pin_12
#define GPIO_PORT_DC        GPIOA
// cs pin   -> PA4
#define GPIO_PIN_CS         GPIO_Pin_4
#define GPIO_PORT_CS        GPIOA
// sck pin  -> PA5
#define GPIO_PIN_SPI_SCK    GPIO_Pin_5
#define GPIO_PORT_SPI_SCK   GPIOA
// mosi pin -> PA7
#define GPIO_PIN_SPI_MOSI   GPIO_Pin_7
#define GPIO_PORT_SPI_MOSI  GPIOA
// SPI interface SPI1
#define SPIX                SPI1

void epd_delay_ms(uint16_t ms)
{
    Delay_Ms(ms);
}

void epd_msp_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_RES;
    GPIO_SetBits(GPIO_PORT_RES, GPIO_PIN_RES);
    GPIO_Init(GPIO_PORT_RES, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_DC;
    GPIO_SetBits(GPIO_PORT_DC, GPIO_PIN_DC);
    GPIO_Init(GPIO_PORT_DC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_CS;
    GPIO_SetBits(GPIO_PORT_CS, GPIO_PIN_CS);
    GPIO_Init(GPIO_PORT_CS, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_BUSY;
    GPIO_Init(GPIO_PORT_BUSY, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_SCK;
    GPIO_Init(GPIO_PORT_SPI_SCK, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SPI_MOSI;
    GPIO_Init(GPIO_PORT_SPI_MOSI, &GPIO_InitStructure);

    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_Init(SPIX, &SPI_InitStructure);
    SPI_Cmd(SPIX, ENABLE);
}

void epd_res_set(void)
{
    GPIO_SetBits(GPIO_PORT_RES, GPIO_PIN_RES);
}

void epd_res_reset(void)
{
    GPIO_ResetBits(GPIO_PORT_RES, GPIO_PIN_RES);
}

void epd_dc_set(void)
{
    GPIO_SetBits(GPIO_PORT_DC, GPIO_PIN_DC);
}

void epd_dc_reset(void)
{
    GPIO_ResetBits(GPIO_PORT_DC, GPIO_PIN_DC);
}

void epd_cs_set(void)
{
    GPIO_SetBits(GPIO_PORT_CS, GPIO_PIN_CS);
}

void epd_cs_reset(void)
{
    GPIO_ResetBits(GPIO_PORT_CS, GPIO_PIN_CS);
}

bool epd_is_busy(void)
{
  return GPIO_ReadInputDataBit(GPIO_PORT_BUSY, GPIO_PIN_BUSY) == Bit_RESET ? false : true;
}

void epd_write_byte(uint8_t data)
{
    SPI_I2S_SendData(SPIX, data);
    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_BSY) != RESET);
}

void epd_write_data(const uint8_t *data, uint32_t size)
{
    while(size--)
    {
        while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPIX, *data++);
    }
    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_BSY) != RESET);
}

void epd_write_data_inverted(const uint8_t *data, uint32_t size)
{
    while(size--)
    {
        while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET);
        SPI_I2S_SendData(SPIX, ~(*data++));
    }
    while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_BSY) != RESET);
}
