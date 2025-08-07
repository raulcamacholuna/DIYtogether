/*
 * Archivo: bsp_api.h
 * Versión: 9.0 (FINAL - Contrato completo para el hardware manager)
 */
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h" // <-- Añadido por si acaso

// --- INICIALIZADORES ---

// Función de conveniencia para inicializar todo el hardware con una sola llamada.
esp_err_t bsp_init(void); 

// [AÑADIDO] Funciones de inicialización granular que hardware_manager necesita.
esp_err_t bsp_i2c_init(void);
esp_err_t bsp_display_init(void);
esp_err_t bsp_touch_init(void);
esp_err_t bsp_sdcard_init(void);
esp_err_t bsp_imu_init(void);

// --- FUNCIONES DE CONTROL ---
void bsp_display_set_brightness(int percentage);
void bsp_imu_read(float acc[3], float gyro[3]);

// --- GETTERS DE HANDLES Y CONFIGURACIÓN ---
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void); // <-- [AÑADIDO]
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);

int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void); // <-- [AÑADIDO]

#endif // BSP_API_H