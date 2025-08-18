/* Fecha: 18/08/2025 - 08:25  */
/* Fichero: components/bsp/include/bsp_api.h */
/* Último cambio: Añadida la declaración de bsp_wifi_deinit para permitir la desactivación del stack WiFi. */
/* Descripción: Interfaz pública del BSP. Se añade una nueva función para de-inicializar completamente el stack WiFi. Esto es crucial para poder activar y desactivar la red bajo demanda desde el modo de configuración, liberando memoria y recursos cuando no se necesita. */
#ifndef BSP_API_H
#define BSP_API_H

#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include <stdint.h> // Para uint16_t

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
esp_err_t bsp_battery_init(void);

// --- FUNCIONES DE WIFI ---
void bsp_wifi_init_stack(void);
void bsp_wifi_init_sta(const char *ssid, const char *pass);
void bsp_wifi_start_ap(void);
void bsp_wifi_init_sta_from_nvs(void);
bool bsp_wifi_wait_for_ip(uint32_t timeout_ms);
void bsp_wifi_get_ip(char *ip);
void bsp_wifi_erase_credentials(void);
void bsp_wifi_deinit(void); // <-- NUEVA FUNCIÓN

// --- FUNCIONES DE CONTROL ---
void bsp_display_set_brightness(int percentage);
void bsp_display_turn_on(void);
void bsp_display_turn_off(void);

// --- FUNCIONES DEL IMU (SENSOR DE MOVIMIENTO) ---
void bsp_imu_read(float acc[3], float gyro[3]);

// --- FUNCIONES DE BATERÍA ---
void bsp_battery_get_voltage(float *voltage, uint16_t *adc_value);

// --- GETTERS DE HANDLES Y CONFIGURACIÓN ---
i2c_master_bus_handle_t bsp_get_i2c_bus_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_touch_handle_t bsp_get_touch_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#endif // BSP_API_H
