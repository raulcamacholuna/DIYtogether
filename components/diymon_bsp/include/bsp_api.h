/*
 * Fichero: ./diymon_bsp/include/bsp_api.h
 * Fecha: 08/08/2025 - 03:45
 * Último cambio: Añadida la declaración de bsp_get_i2c_bus_handle.
 * Descripción: Interfaz pública y unificada para la capa de abstracción de hardware (BSP). Declara todas las funciones que la aplicación puede usar, incluyendo el manejador del bus I2C moderno para que otros drivers puedan compartirlo.
 */
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "driver/i2c_master.h" // Necesario para definir i2c_master_bus_handle_t

// --- INICIALIZADORES ---
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
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void); // <-- LA DECLARACIÓN QUE FALTABA
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#endif // BSP_API_H