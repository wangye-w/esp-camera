idf_component_register(SRCS "camera.c" "spilcd.c" "main.c" "myiic.c" "xl9555.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd nvs_flash
                    )
