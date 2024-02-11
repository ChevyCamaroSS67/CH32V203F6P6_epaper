#ifndef __EPAPER_H
#define __EPAPER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define EPD_OK      false
#define EPD_ERROR   true

#define EPD_COLOR_WHITE 0xFF
#define EPD_COLOR_BLACK 0x00
#define EPD_COLOR_RED   EPD_COLOR_BLACK

#define EPD_FONT_SIZE8x6    (8)
#define EPD_FONT_SIZE12x6   (12)
#define EPD_FONT_SIZE16x8   (16)
#define EPD_FONT_SIZE24x12  (24)

#define EPD_DEEPSLEEP_MODE1 (0x01)
#define EPD_DEEPSLEEP_MODE2 (0x03)

struct EpdPaint
{
    uint8_t *Image;
    uint16_t Width;
    uint16_t Height;
    uint16_t WidthMemory;
    uint16_t HeightMemory;
    uint16_t Color;
    uint16_t Rotate;
    uint16_t WidthByte;
    uint16_t HeightByte;
};

extern struct EpdPaint EPD_Paint;

struct EpdInterface
{
    void (*delay_ms)(uint16_t ms);
    void (*msp_init)(void);
    void (*res_set)(void);
    void (*res_reset)(void);
    void (*dc_set)(void);
    void (*dc_reset)(void);
    void (*cs_set)(void);
    void (*cs_reset)(void);
    bool (*is_busy)(void);
    void (*write_byte)(uint8_t data);
    void (*write_data)(const uint8_t *data, uint32_t size);
    void (*write_data_inverted)(const uint8_t *data, uint32_t size);
};

enum EpdType {EPD_154, EPD_213, EPD_29};

enum EpdTypeDimension {
    EPD_154_W = 200, EPD_154_H = 200,
    EPD_213_W = 122, EPD_213_H = 250,
    EPD_29_W = 128, EPD_29_H = 296
};

enum EpdTypeWBuffSize {
    EPD_154_W_BUFF_SIZE = ((EPD_154_W % 8 == 0) ? (EPD_154_W / 8) : (EPD_154_W / 8 + 1)),
    EPD_213_W_BUFF_SIZE = ((EPD_213_W % 8 == 0) ? (EPD_213_W / 8) : (EPD_213_W / 8 + 1)),
    EPD_29_W_BUFF_SIZE = ((EPD_29_W % 8 == 0) ? (EPD_29_W / 8) : (EPD_29_W / 8 + 1)),
};

enum EpdRotate {EPD_ROTATE_0 = 0, EPD_ROTATE_90 = 90, EPD_ROTATE_180 = 180, EPD_ROTATE_270 = 270};

struct EpdInstance
{
    bool hibernating;
    enum EpdType type;
    struct EpdInterface *intf;
};

bool epd_pre_init(struct EpdInstance *instance);
bool epd_init(struct EpdInstance *instance);
bool epd_init_partial(struct EpdInstance *instance);
void epd_enter_deepsleepmode(struct EpdInstance *instance, uint8_t mode);
bool epd_power_on(struct EpdInstance *instance);
bool epd_power_off(struct EpdInstance *instance);
void epd_init_internalTempSensor(struct EpdInstance *instance);
void epd_update(struct EpdInstance *instance);
void epd_update_partial(struct EpdInstance *instance);
void epd_setpos(struct EpdInstance *instance, uint16_t x, uint16_t y);
void epd_display(struct EpdInstance *instance, uint8_t *image1, uint8_t *image2);
void epd_displayBW(struct EpdInstance *instance, uint8_t *image);
void epd_displayBW_partial(struct EpdInstance *instance, uint8_t *image);
void epd_displayRED(struct EpdInstance *instance, uint8_t *image);
    
void epd_paint_newimage(uint8_t *image, uint16_t Width, uint16_t Height, uint16_t Rotate, uint16_t Color);
void epd_paint_setpixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);
void epd_paint_selectimage(uint8_t *image);
void epd_paint_clear(uint16_t color);
void epd_paint_drawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);
void epd_paint_drawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color);
void epd_paint_drawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t Color, uint8_t mode);
void epd_paint_drawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint16_t Color, uint8_t mode);
void epd_paint_showChar(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color);
void epd_paint_showString(uint16_t x, uint16_t y, uint8_t *chr, uint16_t size1, uint16_t color);
void epd_paint_showNum(uint16_t x, uint16_t y, uint32_t num, uint16_t len, uint16_t size1, uint16_t color);
void epd_paint_showChinese(uint16_t x, uint16_t y, uint16_t num, uint16_t size1, uint16_t color);
void epd_paint_showPicture(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t BMP[], uint16_t Color);

#ifdef __cplusplus
}
#endif

#endif
