/*
 * msp.h
 *
 *  Created on: Feb 10, 2024
 *      Author: Chevy
 */

#ifndef USER_MSP_H_
#define USER_MSP_H_

#include <stdint.h>
#include <stdbool.h>

void epd_delay_ms(uint16_t ms);
void epd_msp_init(void);
void epd_res_set(void);
void epd_res_reset(void);
void epd_dc_set(void);
void epd_dc_reset(void);
void epd_cs_set(void);
void epd_cs_reset(void);
bool epd_is_busy(void);
void epd_write_byte(uint8_t data);
void epd_write_data(const uint8_t *data, uint32_t size);
void epd_write_data_inverted(const uint8_t *data, uint32_t size);

#endif /* USER_MSP_H_ */
