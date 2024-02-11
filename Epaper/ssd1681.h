/*
 * ssd1681.h
 *
 *  Created on: Feb 11, 2024
 *      Author: Chevy
 */

#ifndef EPAPER_SSD1681_H_
#define EPAPER_SSD1681_H_

#define SSD1681_CMD_DRIVER_OUTPUT_CONTROL   0x01

#define SSD1681_CMD_DATA_ENTRY_MODE         0x11
#define SSD1681_DATA_ENTRY_MODE_YDEC_XDEC   0x00
#define SSD1681_DATA_ENTRY_MODE_YDEC_XINC   0x01
#define SSD1681_DATA_ENTRY_MODE_YINC_XDEC   0x02
#define SSD1681_DATA_ENTRY_MODE_YINC_XINC   0x03
#define SSD1681_DATA_ENTRY_MODE_AM_XDIR     0x00
#define SSD1681_DATA_ENTRY_MODE_AM_YDIR     0x04

#define SSD1681_CMD_SW_RESET                0x12

#endif /* EPAPER_SSD1681_H_ */