/*
  Fichero: ./components/diymon_bsp/include/bsp_api.h
  Fecha: 12/08/2025 - 09:00
  Último cambio: Restauradas todas las declaraciones de funciones públicas necesarias.
  Descripción: Interfaz pública del BSP. Se restauran las declaraciones de funciones
               esenciales (`bsp_get_i2c_bus_handle`, `bsp_display_turn_on/off`)
               para asegurar que todos los componentes que dependen del BSP compilen.
*/
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "driver/i2c_master.h"
#include "esp_wifi.h"

// --- INICIALIZADORES DE HARDWARE ---
esp_err_t bsp_init(void);
esp_err_t bsp_init_service_mode(void);
esp_err_t bsp_init_minimal_headless(void);

// --- INICIALIZADORES DE PERIFÉRICOS INDIVIDUALES ---
esp_err_t bsp_i2c_init(void);
esp_err_t bsp_spi_init(void);
esp_err_t bsp_display_init(void);
esp_err_t bsp_touch_init(void);
esp_err_t bsp_sdcard_init(void);
esp_err_t bsp_imu_init(void);

// --- FUNCIONES DE WIFI ---
void bsp_wifi_init_stack(void);
void bsp_wifi_start_ap(void);
void bsp_wifi_init_sta_from_nvs(void);
bool bsp_wifi_wait_for_ip(uint32_t timeout_ms);
void bsp_wifi_get_ip(char *ip);

// --- FUNCIONES DE CONTROL ---
void bsp_display_set_brightness(int percentage);
void bsp_display_turn_on(void);
void bsp_display_turn_off(void);

// --- GETTERS DE HANDLES Y CONFIGURACIÓN ---
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#endif // BSP_API_H