#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"

// --- El Contrato Universal para cualquier Hardware de DIYMON ---

// Inicializadores principales
esp_err_t bsp_i2c_init(void);
esp_err_t bsp_spi_init(void);
esp_err_t bsp_display_init(void);
esp_err_t bsp_touch_init(void);
esp_err_t bsp_sdcard_init(void);

// Funciones de control
void bsp_display_set_brightness(int percentage);

// ... y cualquier otra función bsp_... que uses en hardware_manager.c

#endif // BSP_API_H