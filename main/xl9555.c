#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "myiic.h"
#include "string.h"
#include "xl9555.h"
#include "esp_log.h"

static const char *xl9555_tag = "xl9555";
i2c_master_dev_handle_t xl9555_handle = NULL;

/**
 * @brief       读取XL9555的IO值
 * @param       data:读取数据的存储区
 * @param       len:读取数据的大小
 * @retval      ESP_OK:读取成功; 其他:读取失败
 */
esp_err_t xl9555_read_byte(uint8_t *data, size_t len)
{
    uint8_t reg_addr = XL9555_INPUT_PORT0_REG;
    
    return i2c_master_transmit_receive(xl9555_handle, &reg_addr, 1, data, len, -1);
}

/**
 * @brief       向XL9555寄存器写入数据
 * @param       reg:寄存器地址
 * @param       data:要写入数据的存储区
 * @param       len:要写入数据的大小
 * @retval      ESP_OK:读取成功; 其他:读取失败
 */
esp_err_t xl9555_write_byte(uint8_t reg, uint8_t *data, size_t len)
{
    esp_err_t ret;

    uint8_t *buf = malloc(1 + len);
    if (buf == NULL)
    {
        ESP_LOGE(xl9555_tag, "%s memory failed", __func__);
        return ESP_ERR_NO_MEM;      /* 分配内存失败 */
    }

    buf[0] = reg;                   /* 0号元素为寄存器数值 */
    memcpy(buf + 1, data, len);     /* 拷贝数据至存储区中 */

    ret = i2c_master_transmit(xl9555_handle, buf, len + 1, -1);

    free(buf);                      /* 发送完成释放内存 */

    return ret;
}

/**
 * @brief       控制某个IO的电平
 * @param       pin     : 控制的IO
 * @param       val     : 电平
 * @retval      返回所有IO状态
 */
uint16_t xl9555_pin_write(uint16_t pin, int val)
{
    uint8_t w_data[2];
    uint16_t temp = 0x0000;

    xl9555_read_byte(w_data, 2);

    if (pin <= 0x0080)
    {
        if (val)
        {
            w_data[0] |= (uint8_t)(0xFF & pin);
        }
        else
        {
            w_data[0] &= ~(uint8_t)(0xFF & pin);
        }
    }
    else
    {
        if (val)
        {
            w_data[1] |= (uint8_t)(0xFF & (pin >> 8));
        }
        else
        {
            w_data[1] &= ~(uint8_t)(0xFF & (pin >> 8));
        }
    }

    temp = ((uint16_t)w_data[1] << 8) | w_data[0]; 

    xl9555_write_byte(XL9555_OUTPUT_PORT0_REG, w_data, 2);
    
    return temp;
}

/**
 * @brief       XL9555的IO配置
 * @param       config_value：IO配置输入或者输出
 * @retval      返回设置的数值
 */
void xl9555_ioconfig(uint16_t config_value)
{
    /* 从机地址 + CMD + data1(P0) + data2(P1) */
    /* P10、P11、P12、P13和P14为输入，其他引脚为输出 -->0001 1111 0000 0000 注意：0为输出，1为输入*/
    uint8_t data[2];
    esp_err_t err;

    data[0] = (uint8_t)(0xFF & config_value);
    data[1] = (uint8_t)(0xFF & (config_value >> 8));

    do
    {
        err = xl9555_write_byte(XL9555_CONFIG_PORT0_REG, data, 2);
        if (err != ESP_OK)
        {
            ESP_LOGE(xl9555_tag, "%s configure %X failed, ret: %d", __func__, config_value, err);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
        
    } while (err != ESP_OK);
}

esp_err_t xl9555_init()
{
    uint8_t r_data[2];

    i2c_device_config_t xl9555_i2c_dev_conf = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,  /* 从机地址长度 */
        .scl_speed_hz    = IIC_SPEED_CLK,       /* 传输速率 */
        .device_address  = XL9555_ADDR,         /* 从机7位的地址 */
    };
    /* I2C总线上添加XL9555设备 */
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &xl9555_i2c_dev_conf, &xl9555_handle));

    /* 输入模式下，中断才有效（读取IO电平） */
    // xl9555_int_init();

    /* 上电先读取一次清除中断标志 */
    xl9555_read_byte(r_data, 2);
    /* 配置那些扩展管脚为输入输出模式 */
    xl9555_ioconfig(0xF003);
    /* 关闭蜂鸣器 */
    xl9555_pin_write(BEEP_IO, 0);
    /* 关闭喇叭 */
    // xl9555_pin_write(SPK_EN_IO, 1);

    return ESP_OK;
}