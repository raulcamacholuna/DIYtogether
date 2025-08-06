/*
 * Archivo: components/diymon_bsp/include/bsp_api.h
 * Versión Corregida: Esta es la API pública y única fuente de verdad para tu BSP.
 */
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h" // Necesario para esp_lcd_panel_handle_t
#include "esp_lcd_touch.h"     // Necesario para esp_lcd_touch_handle_t
#include "driver/i2c_master.h" // Necesario para i2c_master_bus_handle_t

/*
 * NOTA DE DISEÑO:
 * Esta API es lo que el resto de tu aplicación (main, UI, etc.) "ve".
 * Está diseñada para ser simple y ocultar los detalles complejos del hardware.
 * Una única función, bsp_init(), se encargará de inicializar todo el hardware necesario.
 */

// --- Inicializador Principal ---
// Llama a esta ÚNICA función desde main.c para encender todo el hardware.
esp_err_t bsp_init(void);

// --- Funciones de Control ---
void bsp_display_set_brightness(int percentage);
void bsp_imu_read(float acc[3], float gyro[3]);

// --- Getters para los "Handles" (para que otros componentes puedan usarlos) ---
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);

// --- Getters para la configuración del display ---
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);

#endif // BSP_API_H