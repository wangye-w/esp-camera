#pragma once

extern i2c_master_bus_handle_t bus_handle;  /* 总线句柄 */

#define IIC_NUM_PORT       I2C_NUM_0
#define IIC_SPEED_CLK      400000           /* 速率400K */
#define IIC_SDA_GPIO_PIN   GPIO_NUM_41
#define IIC_SCL_GPIO_PIN   GPIO_NUM_42

#define CAM_PIN_SIOD        GPIO_NUM_39     /* IIC1_SDA引脚 */
#define CAM_PIN_SIOC        GPIO_NUM_38     /* IIC1_SCL引脚 */

/* 函数声明 */
esp_err_t myiic_init(void);                 /* 初始化MYIIC */