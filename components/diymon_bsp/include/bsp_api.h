/*
 * Archivo: bsp_api.h
 * Versión: FINAL CORREGIDA Y COMPLETA
 */
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "driver/i2c_master.h"

// --- INICIALIZADORES ---
// [CORRECCIÓN] Todas las funciones _init deben devolver esp_err_t para poder usar ESP_ERROR_CHECK
esp_err_t bsp_init(void); 
esp_err_t bsp_i2c_init(void);
esp_err_t bsp_display_init(void);
esp_err_t bsp_touch_init(void);
esp_err_t bsp_sdcard_init(void);
esp_err_t bsp_imu_init(void);

// --- FUNCIONES DE CONTROL ---
void bsp_display_set_brightness(int percentage);
void bsp_imu_read(float acc[3], float gyro[3]);

// --- GETTERS DE HANDLES Y CONFIGURACIÓN ---
// [CORRECCIÓN] La función que faltaba para el IMU
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#endif // BSP_API_H