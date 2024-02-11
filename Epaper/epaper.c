/*---------------------------------------
- WeAct Studio Official Link
- taobao: weactstudio.taobao.com
- aliexpress: weactstudio.aliexpress.com
- github: github.com/WeActTC
- gitee: gitee.com/WeAct-TC
- blog: www.weact-tc.cn
---------------------------------------*/

#include "epaper.h"
#include "epdfont.h"
#include "ssd1681.h"
#include "ch32v20x.h"

struct EpdPaint EPD_Paint;

static const unsigned char lut_partial[] =
{
  0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x80, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x40, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x0, 0x0, 0x0,
};

static void epd_write_reg(struct EpdInstance *instance, uint8_t reg)
{
    instance->intf->dc_reset();
    instance->intf->cs_reset();
    instance->intf->write_byte(reg);
    instance->intf->cs_set();
    instance->intf->dc_set();
}

static void epd_write_data(struct EpdInstance *instance, uint8_t data)
{
    instance->intf->cs_reset();
    instance->intf->write_byte(data);
    instance->intf->cs_set();
}

static bool epd_wait_busy(struct EpdInstance *instance)
{
    uint32_t timeout = 0;
    while (instance->intf->is_busy())
    {
        timeout++;
        if (timeout > 40000)
        {
            return EPD_ERROR;
        }
        instance->intf->delay_ms(1);
    }
    return EPD_OK;
}

static void epd_reset(struct EpdInstance *instance)
{
    instance->intf->res_reset();
    instance->intf->delay_ms(50);
    instance->intf->res_set();
    instance->intf->delay_ms(50);
    instance->hibernating = false;
}

bool epd_pre_init(struct EpdInstance *instance)
{
    if (instance == NULL ||
        instance->intf == NULL ||
        instance->intf->cs_reset == NULL ||
        instance->intf->cs_set == NULL ||
        instance->intf->dc_reset == NULL ||
        instance->intf->dc_set == NULL ||
        instance->intf->delay_ms == NULL ||
        instance->intf->is_busy == NULL ||
        instance->intf->msp_init == NULL ||
        instance->intf->res_reset == NULL ||
        instance->intf->res_set == NULL ||
        instance->intf->write_byte == NULL ||
        instance->intf->write_data == NULL ||
        instance->intf->write_data_inverted == NULL)
    {
        return EPD_ERROR;
    }
    else
    {
        instance->intf->msp_init();
        return EPD_OK;
    }
}

bool epd_init(struct EpdInstance *instance)
{
    if (instance->hibernating)
        epd_reset(instance);

    if (epd_wait_busy(instance))
        return EPD_ERROR;

    epd_write_reg(instance, SSD1681_CMD_SW_RESET);
    instance->intf->delay_ms(10);

    if (epd_wait_busy(instance))
        return EPD_ERROR;

    epd_write_reg(instance, SSD1681_CMD_DRIVER_OUTPUT_CONTROL);
    switch(instance->type)
    {
    case EPD_213:
    case EPD_29:
        epd_write_data(instance, 0x27);
        epd_write_data(instance, 0x01);
        epd_write_data(instance, 0x01);
        break;
    default: // EPD_154 as default
        epd_write_data(instance, 0xC7);
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x01);
    }

    epd_write_reg(instance, SSD1681_CMD_DATA_ENTRY_MODE);
    epd_write_data(instance, SSD1681_DATA_ENTRY_MODE_AM_XDIR|SSD1681_DATA_ENTRY_MODE_YDEC_XINC);

    switch(instance->type)
    {
    case EPD_154:
        epd_write_reg(instance, 0x44); // set Ram-X address start/end position
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x18);

        epd_write_reg(instance, 0x45);  // set Ram-Y address start/end position
        epd_write_data(instance, 0xC7);
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x00);
        break;
    default:
        epd_write_reg(instance, 0x44); // set Ram-X address start/end position
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x0F); // 0x0F-->(15+1)*8=128

        epd_write_reg(instance, 0x45);  // set Ram-Y address start/end position
        epd_write_data(instance, 0x27); // 0x127-->(295+1)=296
        epd_write_data(instance, 0x01);
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x00);
        break;
    }

    epd_write_reg(instance, 0x3C); // BorderWavefrom
    epd_write_data(instance, 0x05);

    switch(instance->type)
    {
    case EPD_213:
    case EPD_29:
        epd_write_reg(instance, 0x21); //  Display update control
        epd_write_data(instance, 0x00);
        epd_write_data(instance, 0x80);
        break;
    default:
        break;
    }

    epd_write_reg(instance, 0x18); // Read built-in temperature sensor
    epd_write_data(instance, 0x80);

    epd_setpos(instance, 0, 0);

    if (epd_power_on(instance))
        return EPD_ERROR;

    return EPD_OK;
}

bool epd_init_partial(struct EpdInstance *instance)
{
    if (epd_init(instance))
        return EPD_ERROR;

    switch(instance->type)
    {
    case EPD_213:
    case EPD_29:
        epd_write_reg(instance, 0x32);
        instance->intf->cs_reset();
        instance->intf->write_data(lut_partial, sizeof(lut_partial));
        instance->intf->cs_set();
        break;
    default:
        break;
    }

  return EPD_OK;
}

void epd_enter_deepsleepmode(struct EpdInstance *instance, uint8_t mode)
{
    epd_power_off(instance);
    epd_write_reg(instance, 0x10);
    epd_write_data(instance, mode);
    instance->hibernating = true;
}

bool epd_power_on(struct EpdInstance *instance)
{
    epd_write_reg(instance, 0x22); // Display Update Control
    epd_write_data(instance, 0xf8);
    epd_write_reg(instance, 0x20); // Activate Display Update Sequence

    return epd_wait_busy(instance);
}

bool epd_power_off(struct EpdInstance *instance)
{
    epd_write_reg(instance, 0x22); // Display Update Control
    epd_write_data(instance, 0x83);
    epd_write_reg(instance, 0x20); // Activate Display Update Sequence

    return epd_wait_busy(instance);
}

void epd_init_internalTempSensor(struct EpdInstance *instance)
{
    epd_write_reg(instance, 0x18);
    epd_write_data(instance, 0x80);

    epd_write_reg(instance, 0x1A);
    epd_write_data(instance, 0x7F);
    epd_write_data(instance, 0xF0);
}

void epd_update(struct EpdInstance *instance)
{
    epd_write_reg(instance, 0x22); // Display Update Control

    switch(instance->type)
    {
    case EPD_154:
        epd_write_data(instance, 0xF4);
        break;
    default:
        epd_write_data(instance, 0xF7);
        break;
    }

    epd_write_reg(instance, 0x20); // Activate Display Update Sequence
    epd_wait_busy(instance);
}

void epd_update_partial(struct EpdInstance *instance)
{
    epd_write_reg(instance, 0x22); // Display Update Control

    switch(instance->type)
    {
    case EPD_154:
        epd_write_data(instance, 0xFC);
        break;
    default:
        epd_write_data(instance, 0xCC);
        break;
    }

    epd_write_reg(instance, 0x20); // Activate Display Update Sequence
    epd_wait_busy(instance);
}

void epd_setpos(struct EpdInstance *instance, uint16_t x, uint16_t y)
{
    uint8_t _x;
    uint16_t _y;

    _x = x / 8;

    switch(instance->type)
    {
    case EPD_154:
        _y = 199 - y;
        break;
    default:
        _y = 295 - y;
    }

  epd_write_reg(instance, 0x4E); // set RAM x address count to 0;
  epd_write_data(instance, _x);
  epd_write_reg(instance, 0x4F); // set RAM y address count to 0x127;
  epd_write_data(instance, _y & 0xff);
  epd_write_data(instance, _y >> 8 & 0x01);
}

static void epd_writedata(struct EpdInstance *instance, uint8_t *image, uint32_t length)
{
    instance->intf->cs_reset();
    instance->intf->write_data(image, length);
    instance->intf->cs_set();
}

void epd_display(struct EpdInstance *instance, uint8_t *image1, uint8_t *image2)
{
    uint32_t width, height;

    switch(instance->type)
    {
    case EPD_154:
        width = EPD_154_H;
        height = EPD_154_W_BUFF_SIZE;
        break;
    case EPD_213:
        width = EPD_213_H;
        height = EPD_213_W_BUFF_SIZE;
        break;
    case EPD_29:
        width = EPD_29_H;
        height = EPD_29_W_BUFF_SIZE;
        break;
    default:
        return;
    }

    epd_setpos(instance, 0, 0);

    epd_write_reg(instance, 0x24);
    epd_writedata(instance, image1, width * height);

    epd_setpos(instance, 0, 0);

    epd_write_reg(instance, 0x26);

    instance->intf->cs_reset();
    instance->intf->write_data_inverted(image2, width * height);
    instance->intf->cs_reset();

    epd_update(instance);
}

void epd_displayBW(struct EpdInstance *instance, uint8_t *image)
{
    uint32_t width, height;

    switch(instance->type)
    {
    case EPD_154:
        width = EPD_154_H;
        height = EPD_154_W_BUFF_SIZE;
        break;
    case EPD_213:
        width = EPD_213_H;
        height = EPD_213_W_BUFF_SIZE;
        break;
    case EPD_29:
        width = EPD_29_H;
        height = EPD_29_W_BUFF_SIZE;
        break;
    default:
        return;
    }

    epd_setpos(instance, 0, 0);
    epd_write_reg(instance, 0x26);
    epd_writedata(instance, image, width * height);

    epd_setpos(instance, 0, 0);
    epd_write_reg(instance, 0x24);
    epd_writedata(instance, image, width * height);

    epd_update(instance);
}

void epd_displayBW_partial(struct EpdInstance *instance, uint8_t *image)
{
    uint32_t width, height;

    switch(instance->type)
    {
    case EPD_154:
        width = EPD_154_H;
        height = EPD_154_W_BUFF_SIZE;
        break;
    case EPD_213:
        width = EPD_213_H;
        height = EPD_213_W_BUFF_SIZE;
        break;
    case EPD_29:
        width = EPD_29_H;
        height = EPD_29_W_BUFF_SIZE;
        break;
    default:
        return;
    }

    epd_setpos(instance, 0, 0);
    epd_write_reg(instance, 0x24);
    epd_writedata(instance, image, width * height);

    epd_update_partial(instance);

    epd_setpos(instance, 0, 0);
    epd_write_reg(instance, 0x26);
    epd_writedata(instance, image, width * height);
}

void epd_displayRED(struct EpdInstance *instance, uint8_t *image)
{
    uint32_t width, height;

    switch(instance->type)
    {
    case EPD_154:
        width = EPD_154_H;
        height = EPD_154_W_BUFF_SIZE;
        break;
    case EPD_213:
        width = EPD_213_H;
        height = EPD_213_W_BUFF_SIZE;
        break;
    case EPD_29:
        width = EPD_29_H;
        height = EPD_29_W_BUFF_SIZE;
        break;
    default:
        return;
    }

    epd_setpos(instance, 0, 0);

    epd_write_reg(instance, 0x26);
    epd_writedata(instance, image, width * height);

    epd_update(instance);
}

void epd_paint_newimage(uint8_t *image, uint16_t width, uint16_t height, uint16_t rotate, uint16_t color)
{
    EPD_Paint.Image = image;

    EPD_Paint.WidthMemory = width;
    EPD_Paint.HeightMemory = height;
    EPD_Paint.Color = color;
    EPD_Paint.WidthByte = (width % 8 == 0) ? (width / 8) : (width / 8 + 1);
    EPD_Paint.HeightByte = height;
    EPD_Paint.Rotate = rotate;

    if (rotate == EPD_ROTATE_0 || rotate == EPD_ROTATE_180)
    {
        EPD_Paint.Width = height;
        EPD_Paint.Height = width;
    }
    else
    {
        EPD_Paint.Width = width;
        EPD_Paint.Height = height;
    }
}

void epd_paint_setpixel(uint16_t xpoint, uint16_t ypoint, uint16_t color)
{
    uint16_t x, y;

    switch (EPD_Paint.Rotate)
    {
    case 0:
        x = EPD_Paint.WidthMemory - ypoint - 1;
        y = xpoint;
        break;
    case 90:
        x = EPD_Paint.WidthMemory - xpoint - 1;
        y = EPD_Paint.HeightMemory - ypoint - 1;
        break;
    case 180:
        x = ypoint;
        y = EPD_Paint.HeightMemory - xpoint - 1;
        break;
    case 270:
        x = xpoint;
        y = ypoint;
        break;
    default:
        return;
    }

    uint32_t addr = x / 8 + y * EPD_Paint.WidthByte;
    uint8_t rdata = EPD_Paint.Image[addr];

    if (color == EPD_COLOR_BLACK)
        EPD_Paint.Image[addr] = rdata & ~(0x80 >> (x % 8));
    else
        EPD_Paint.Image[addr] = rdata | (0x80 >> (x % 8));
}

void epd_paint_clear(uint16_t color)
{
    uint16_t x, y;
    uint32_t addr;

    for (y = 0; y < EPD_Paint.HeightByte; y++)
    {
        for (x = 0; x < EPD_Paint.WidthByte; x++)
        { // 8 pixel =  1 byte
            addr = x + y * EPD_Paint.WidthByte;
            EPD_Paint.Image[addr] = color;
        }
    }
}

void epd_paint_selectimage(uint8_t *image)
{
    EPD_Paint.Image = image;
}

void epd_paint_drawPoint(uint16_t xpoint, uint16_t ypoint, uint16_t color)
{
    epd_paint_setpixel(xpoint - 1, ypoint - 1, color);
}

void epd_paint_drawLine(uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t xpoint = xstart;
    uint16_t ypoint = ystart;

    int32_t dx = (int32_t)xend - (int32_t)xstart >= 0 ? xend - xstart : xstart - xend;
    int32_t dy = (int32_t)yend - (int32_t)ystart <= 0 ? yend - ystart : ystart - yend;

    int32_t xaddway = xstart < xend ? 1 : -1;
    int32_t yaddway = ystart < yend ? 1 : -1;

    int32_t esp = dx + dy;

    for (;;)
    {
        epd_paint_drawPoint(xpoint, ypoint, color);

        if (2 * esp >= dy)
        {
            if (xpoint == xend)
                break;
            esp += dy;
            xpoint += xaddway;
        }
        if (2 * esp <= dx)
        {
            if (ypoint == yend)
                break;
            esp += dx;
            ypoint += yaddway;
        }
    }
}

void epd_paint_drawRectangle(uint16_t xstart, uint16_t ystart, uint16_t xend, uint16_t yend, uint16_t color, uint8_t mode)
{
    if (mode)
    {
        for (uint16_t i = ystart; i < yend; i++)
            epd_paint_drawLine(xstart, i, xend, i, color);
    }
    else
    {
        epd_paint_drawLine(xstart, ystart, xend, ystart, color);
        epd_paint_drawLine(xstart, ystart, xstart, yend, color);
        epd_paint_drawLine(xend, yend, xend, ystart, color);
        epd_paint_drawLine(xend, yend, xstart, yend, color);
    }
}

void epd_paint_drawCircle(uint16_t xcenter, uint16_t ycenter, uint16_t radius, uint16_t color, uint8_t mode)
{
    uint16_t xcurrent = 0;
    uint16_t ycurrent = radius;
    uint16_t esp = 3 - (radius << 1);

    if (mode)
    {
        while (xcurrent <= ycurrent)
        { // Realistic circles
            for (uint16_t sCountY = xcurrent; sCountY <= ycurrent; sCountY++)
            {
                epd_paint_drawPoint(xcenter + xcurrent, ycenter + sCountY, color); // 1
                epd_paint_drawPoint(xcenter - xcurrent, ycenter + sCountY, color); // 2
                epd_paint_drawPoint(xcenter - sCountY, ycenter + xcurrent, color); // 3
                epd_paint_drawPoint(xcenter - sCountY, ycenter - xcurrent, color); // 4
                epd_paint_drawPoint(xcenter - xcurrent, ycenter - sCountY, color); // 5
                epd_paint_drawPoint(xcenter + xcurrent, ycenter - sCountY, color); // 6
                epd_paint_drawPoint(xcenter + sCountY, ycenter - xcurrent, color); // 7
                epd_paint_drawPoint(xcenter + sCountY, ycenter + xcurrent, color);
            }
            if ((int)esp < 0)
                esp += 4 * xcurrent + 6;
            else
            {
                esp += 10 + 4 * (xcurrent - ycurrent);
                ycurrent--;
            }
            xcurrent++;
        }
    }
    else
    { // Draw a hollow circle
        while (xcurrent <= ycurrent)
        {
            epd_paint_drawPoint(xcenter + xcurrent, ycenter + ycurrent, color); // 1
            epd_paint_drawPoint(xcenter - xcurrent, ycenter + ycurrent, color); // 2
            epd_paint_drawPoint(xcenter - ycurrent, ycenter + xcurrent, color); // 3
            epd_paint_drawPoint(xcenter - ycurrent, ycenter - xcurrent, color); // 4
            epd_paint_drawPoint(xcenter - xcurrent, ycenter - ycurrent, color); // 5
            epd_paint_drawPoint(xcenter + xcurrent, ycenter - ycurrent, color); // 6
            epd_paint_drawPoint(xcenter + ycurrent, ycenter - xcurrent, color); // 7
            epd_paint_drawPoint(xcenter + ycurrent, ycenter + xcurrent, color); // 0
            if ((int)esp < 0)
                esp += 4 * xcurrent + 6;
            else
            {
                esp += 10 + 4 * (xcurrent - ycurrent);
                ycurrent--;
            }
            xcurrent++;
        }
    }
}

void epd_paint_showChar(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color)
{
  uint16_t i, m, temp, size2, chr1;
  uint16_t x0, y0;
  x += 1, y += 1, x0 = x, y0 = y;
  if (x - size1 > EPD_Paint.Height)
    return;
  if (size1 == 8)
    size2 = 6;
  else
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2);
  chr1 = chr - ' ';
  for (i = 0; i < size2; i++)
  {
    if (size1 == 8)
    {
      temp = asc2_0806[chr1][i];
    } // 0806
    else if (size1 == 12)
    {
      temp = asc2_1206[chr1][i];
    } // 1206
    else if (size1 == 16)
    {
      temp = asc2_1608[chr1][i];
    } // 1608
    else if (size1 == 24)
    {
      temp = asc2_2412[chr1][i];
    } // 2412
    else
      return;
    for (m = 0; m < 8; m++)
    {
      if (temp & 0x01)
        epd_paint_drawPoint(x, y, color);
      else
        epd_paint_drawPoint(x, y, !color);
      temp >>= 1;
      y++;
    }
    x++;
    if ((size1 != 8) && ((x - x0) == size1 / 2))
    {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}

void epd_paint_showString(uint16_t x, uint16_t y, uint8_t *chr, uint16_t size1, uint16_t color)
{
  while (*chr != '\0')
  {
    epd_paint_showChar(x, y, *chr, size1, color);
    chr++;
    if (size1 == 8)
    {
      x += 6;
      if(x > EPD_Paint.Height-6)
				break;
    }
    else
    {
      x += size1 / 2;
      if(x > EPD_Paint.Height-size1 / 2)
				break;
    }
  }
}

// m^n
static uint32_t _Pow(uint16_t m, uint16_t n)
{
  uint32_t result = 1;
  while (n--)
  {
    result *= m;
  }
  return result;
}

void epd_paint_showNum(uint16_t x, uint16_t y, uint32_t num, uint16_t len, uint16_t size1, uint16_t color)
{
  uint8_t t, temp, m = 0;
  if (size1 == 8)
    m = 2;
  for (t = 0; t < len; t++)
  {
    temp = (num / _Pow(10, len - t - 1)) % 10;
    if (temp == 0)
    {
      epd_paint_showChar(x + (size1 / 2 + m) * t, y, '0', size1, color);
    }
    else
    {
      epd_paint_showChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, color);
    }
  }
}

void epd_paint_showChinese(uint16_t x, uint16_t y, uint16_t num, uint16_t size1, uint16_t color)
{
  uint16_t m, temp;
  uint16_t x0, y0;
  uint16_t i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1;
  x += 1, y += 1, x0 = x, y0 = y;
  for (i = 0; i < size3; i++)
  {
    if (size1 == 16)
    {
      temp = Hzk1[num][i];
    } // 16*16
    else if (size1 == 24)
    {
      temp = Hzk2[num][i];
    } // 24*24
    else if (size1 == 32)
    {
      temp = Hzk3[num][i];
    } // 32*32
    else if (size1 == 64)
    {
      temp = Hzk4[num][i];
    } // 64*64
    else
      return;
    for (m = 0; m < 8; m++)
    {
      if (temp & 0x01)
        epd_paint_drawPoint(x, y, color);
      else
        epd_paint_drawPoint(x, y, !color);
      temp >>= 1;
      y++;
    }
    x++;
    if ((x - x0) == size1)
    {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}

void epd_paint_showPicture(uint16_t x, uint16_t y, uint16_t sizex, uint16_t sizey, const uint8_t BMP[], uint16_t Color)
{
  uint16_t j = 0;
  uint16_t i, n = 0, temp = 0, m = 0;
  uint16_t x0 = 0, y0 = 0;
  x += 1, y += 1, x0 = x, y0 = y;
  sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
  for (n = 0; n < sizey; n++)
  {
    for (i = 0; i < sizex; i++)
    {
      temp = BMP[j];
      j++;
      for (m = 0; m < 8; m++)
      {
        if (temp & 0x01)
          epd_paint_drawPoint(x, y, !Color);
        else
          epd_paint_drawPoint(x, y, Color);
        temp >>= 1;
        y++;
      }
      x++;
      if ((x - x0) == sizex)
      {
        x = x0;
        y0 = y0 + 8;
      }
      y = y0;
    }
  }
}
