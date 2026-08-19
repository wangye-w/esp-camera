#pragma once

#include "esp_log.h"
#include "xl9555.h"

#define LCD_PWR(x)          do { x ?                                \
                                 xl9555_pin_write(SLCD_PWR_IO, 1):  \
                                 xl9555_pin_write(SLCD_PWR_IO, 0);  \
                            } while(0)

#define LCD_RST(x)          do { x ?                                 \
                                 xl9555_pin_write(SLCD_RST_IO, 1):   \
                                 xl9555_pin_write(SLCD_RST_IO, 0);   \
                            } while(0)

/* 常用颜色值 */
#define WHITE               0xFFFF      /* 白色 */
#define BLACK               0x0000      /* 黑色 */
#define RED                 0xF800      /* 红色 */
#define GREEN               0x07E0      /* 绿色 */
#define BLUE                0x001F      /* 蓝色 */ 
#define MAGENTA             0XF81F      /* 洋红色 */
#define YELLOW              0XFFE0      /* 黄色 */
#define CYAN                0X07FF      /* 蓝绿色 */

esp_err_t spilcd_init();
void spilcd_display_dir(uint8_t dir);
void spilcd_clear(uint16_t color);