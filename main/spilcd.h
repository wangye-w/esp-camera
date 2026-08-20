#pragma once

#include "esp_log.h"
#include "xl9555.h"

#define BUFFER_LINE         40

#define PIN_NUM_BK_LIGHT
#define SPI_SCLK_PIN    GPIO_NUM_12
#define SPI_MOSI_PIN    GPIO_NUM_11
#define SPI_MISO_PIN    GPIO_NUM_13
#define MY_SPI_HOST         SPI2_HOST
#define LCD_DC_PIN      GPIO_NUM_40 
#define LCD_CS_PIN      GPIO_NUM_21
#define LCD_CLK_HZ      (60 * 1000 * 1000)

/* 常用颜色值 */
#define WHITE               0xFFFF      /* 白色 */
#define BLACK               0x0000      /* 黑色 */
#define RED                 0xF800      /* 红色 */
#define GREEN               0x07E0      /* 绿色 */
#define BLUE                0x001F      /* 蓝色 */ 
#define MAGENTA             0XF81F      /* 洋红色 */
#define YELLOW              0XFFE0      /* 黄色 */
#define CYAN                0X07FF      /* 蓝绿色 */

#define LCD_PWR(x)          do { x ?                                \
                                 xl9555_pin_write(SLCD_PWR_IO, 1):  \
                                 xl9555_pin_write(SLCD_PWR_IO, 0);  \
                            } while(0)

#define LCD_RST(x)          do { x ?                                 \
                                 xl9555_pin_write(SLCD_RST_IO, 1):   \
                                 xl9555_pin_write(SLCD_RST_IO, 0);   \
                            } while(0)


extern esp_lcd_panel_handle_t panel_handle;

esp_err_t spilcd_init();
void spilcd_display_dir(uint8_t dir);
void spilcd_clear(uint16_t color);