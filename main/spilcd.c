#include "driver/gpio.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "esp_log.h"
#include "esp_lcd_panel_st7789.h"
#include "spilcd.h"

static const char* TAG = "spilcd";

#define PIN_NUM_BK_LIGHT
#define SPI_SCLK_PIN    GPIO_NUM_12
#define SPI_MOSI_PIN    GPIO_NUM_11
#define SPI_MISO_PIN    GPIO_NUM_13
#define MY_SPI_HOST         SPI2_HOST
#define LCD_DC_PIN      GPIO_NUM_40 
#define LCD_CS_PIN      GPIO_NUM_21
#define LCD_CLK_HZ      (60 * 1000 * 1000)

esp_lcd_panel_handle_t panel_handle = NULL;

typedef struct
{
    uint32_t pwidth;
    uint32_t pheight;
    uint8_t dir;
    uint16_t width;
    uint16_t height;
} _spilcd_dev;

_spilcd_dev spilcddev;

DRAM_ATTR uint8_t refresh_done_flag = 0;

static bool notify_lcd_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    bool temp = (void *)user_ctx;
    refresh_done_flag = 1;
    return false;
}

esp_err_t spilcd_init()
{
    ESP_LOGI(TAG, "Turn off LCD backlight");
    LCD_RST(0);
    vTaskDelay(pdMS_TO_TICKS(100));
    LCD_RST(1);
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = SPI_SCLK_PIN,
        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 320 * 240 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(MY_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spilcddev.pheight = 320;  /* 高度 */
    spilcddev.pwidth  = 240;   /* 宽度 */

    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_DC_PIN,
        .cs_gpio_num = LCD_CS_PIN,
        .pclk_hz = LCD_CLK_HZ,
        .lcd_cmd_bits = 8,                          /* 命令位宽 */
        .lcd_param_bits = 8,                        /* LCD 参数位宽 */
        .spi_mode = 0,                              /* SPI 模式 */
        .trans_queue_depth = 7,                     /* 传输队列 */
    };
    /* 将 LCD 设备挂在到 SPI 总线上*/
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)MY_SPI_HOST, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,                       /* 颜色深度 */
        .data_endian = LCD_RGB_DATA_ENDIAN_BIG      /* 大端顺序 */
    };

    /* 为 ST7789 创建 LCD 面板句柄 */
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    /* 反显 */
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    /* 初始化 LCD 句柄 */
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    /* 打开屏幕 */
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lcd_flush_ready,
    };
    /* 注册屏幕刷新完成回调函数 */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, NULL));

    spilcd_display_dir(1);

    spilcd_clear(RED);
    LCD_PWR(1);

    return ESP_OK;
}

/**
 * @brief           设置屏幕方向       
 * @param           dir：0 为竖屏， 1 为横屏
 * @retval          无
 */
void spilcd_display_dir(uint8_t dir)
{
    do {
        if (dir > 1) {
            break;
        }

        spilcddev.dir = dir;
        spilcddev.width = spilcddev.pwidth;
        spilcddev.height = spilcddev.pheight;

        if (spilcddev.dir == 0) {
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, false, false);
        } else {
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, false, false);
        }
    } while (0);
}

/**
 * @brief           清屏       
 * @param           color：颜色值
 * @retval          无
 */
#define nums 20
void spilcd_clear(uint16_t color)
{
    /* 以 40 行作为缓冲，提高速率，若出现内存不足，可以减少缓冲行数 */
    uint16_t *buffer = heap_caps_malloc(spilcddev.width * sizeof(uint16_t) * nums, MALLOC_CAP_DMA);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
        return;
    }

    uint16_t color_tmp = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);
    for (uint32_t i = 0; i < spilcddev.width * nums; ++i) {
        buffer[i] = color_tmp;
    }
    for (uint16_t i = 0; i < spilcddev.height; i+=nums) {
        esp_lcd_panel_draw_bitmap(panel_handle, 0, i, spilcddev.width, i + nums, buffer);
    }

    refresh_done_flag = 0;

    do {
        vTaskDelay(1);
    } while (refresh_done_flag != 1);

    heap_caps_free(buffer);
}