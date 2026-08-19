#pragma once

#define XL9555_ADDR                 0X20 

/* XL9555寄存器宏 */
#define XL9555_INPUT_PORT0_REG      0                               /* 输入寄存器0地址 */
#define XL9555_INPUT_PORT1_REG      1                               /* 输入寄存器1地址 */
#define XL9555_OUTPUT_PORT0_REG     2                               /* 输出寄存器0地址 */
#define XL9555_OUTPUT_PORT1_REG     3                               /* 输出寄存器1地址 */
#define XL9555_INVERSION_PORT0_REG  4                               /* 极性反转寄存器0地址 */
#define XL9555_INVERSION_PORT1_REG  5                               /* 极性反转寄存器1地址 */
#define XL9555_CONFIG_PORT0_REG     6                               /* 方向配置寄存器0地址 */
#define XL9555_CONFIG_PORT1_REG     7                               /* 方向配置寄存器1地址 */

/* XL9555各个IO的功能 */
#define AP_INT_IO                   0x0001
#define QMA_INT_IO                  0x0002
#define SPK_EN_IO                   0x0004
#define BEEP_IO                     0x0008
#define OV_PWDN_IO                  0x0010
#define OV_RESET_IO                 0x0020
#define GBC_LED_IO                  0x0040
#define GBC_KEY_IO                  0x0080
#define LCD_BL_IO                   0x0100
#define CT_RST_IO                   0x0200
#define SLCD_RST_IO                 0x0400
#define SLCD_PWR_IO                 0x0800
#define KEY3_IO                     0x1000
#define KEY2_IO                     0x2000
#define KEY1_IO                     0x4000
#define KEY0_IO                     0x8000

esp_err_t xl9555_init();
uint16_t xl9555_pin_write(uint16_t pin, int val);