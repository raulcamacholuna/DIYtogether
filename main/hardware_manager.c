/*
 * =====================================================================================
 *
 *       Filename:  hardware_manager.c
 *
 *    Description:  Implementación del gestor de hardware para DIYMON.
 *                  Centraliza la inicialización y configuración de todos los
 *                  periféricos del dispositivo.
 *
 *        Version:  1.0
 *        Created:  [Fecha de hoy]
 *
 *         Author:  Raúl Camacho Luna (con revisión de IA)
 *   Organization:  DIYMON Project
 *
 * =====================================================================================
 */

#include "hardware_manager.h"

// Includes generales del sistema y drivers
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Includes de la BSP (Board Support Package) para el hardware específico
#include "bsp_i2c.h"
#include "bsp_spi.h"
#include "bsp_display.h"
#include "bsp_touch.h"
#include "bsp_sdcard.h"
#include "bsp_battery.h"
#include "bsp_qmi8658.h"
#include "bsp_wifi.h"

// Includes de LVGL
#include "esp_lvgl_port.h"

// TAG para los logs de este fichero
static const char *TAG = "HW_MANAGER";

/* --- Configuración del Hardware --- */
// Es buena práctica definir aquí las constantes de configuración en lugar de
// tener "números mágicos" esparcidos por el código.

// Configuración de la pantalla
#define DISPLAY_ROTATION             0     // 0, 90, 180, 270
#define DISPLAY_BUFF_HEIGHT          50    // Altura del buffer de dibujado de LVGL
#define DISPLAY_BUFF_DOUBLE_BUFFER   1     // 1 para doble buffer, 0 para simple

// Determina la resolución de la pantalla según la rotación
#if DISPLAY_ROTATION == 90 || DISPLAY_ROTATION == 270
#define LCD_H_RES (320)
#define LCD_V_RES (172)
#else
#define LCD_H_RES (172)
#define LCD_V_RES (320)
#endif


/* --- Variables estáticas del módulo --- */
// Estas variables son "privadas" a este fichero. Nadie fuera de hardware_manager.c
// necesita acceder a ellas directamente.
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;


/* --- Declaraciones de funciones privadas --- */
// Prototipos de funciones que solo se usarán dentro de este fichero.
static esp_err_t lvgl_driver_init(void);

/* --- Implementación de funciones --- */

esp_err_t hardware_manager_init(void)
{
    i2c_master_bus_handle_t i2c_bus_handle;

    ESP_LOGI(TAG, "Initializing I2C and SPI buses...");
    i2c_bus_handle = bsp_i2c_init();
    bsp_spi_init();
    
    ESP_LOGI(TAG, "Initializing sensors and peripherals...");
    bsp_battery_init();
    bsp_qmi8658_init(i2c_bus_handle); // Sensor de movimiento (IMU)
    bsp_sdcard_init();
    
    // NOTA: La conexión WiFi se deja comentada. Idealmente, la UI debe ofrecer
    // una pantalla para que el usuario introduzca sus credenciales.
    // bsp_wifi_init("WSTEST", "waveshare0755");
    // ESP_LOGW(TAG, "WiFi not initialized. Implement WiFi provisioning screen.");

    ESP_LOGI(TAG, "Initializing display and touch panel...");
    bsp_display_init(&io_handle, &panel_handle, LCD_H_RES * DISPLAY_BUFF_HEIGHT);
    bsp_touch_init(&touch_handle, i2c_bus_handle, LCD_H_RES, LCD_V_RES, DISPLAY_ROTATION);

    ESP_LOGI(TAG, "Initializing LVGL graphics library driver...");
    ESP_ERROR_CHECK(lvgl_driver_init());

    ESP_LOGI(TAG, "Initializing display brightness control...");
    bsp_display_brightness_init();
    bsp_display_set_brightness(100); // Pone el brillo al 100%

    return ESP_OK;
}

static esp_err_t lvgl_driver_init(void)
{
    // --- Configuración del motor de LVGL ---
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 1024 * 10,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    // --- Configuración del driver de la pantalla para LVGL ---
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * DISPLAY_BUFF_HEIGHT,
        .double_buffer = DISPLAY_BUFF_DOUBLE_BUFFER,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,
#endif
        }};

    // --- Ajuste de la rotación y el 'gap' de la pantalla ---
    // Este panel LCD tiene un área no visible, por lo que es necesario
    // ajustar las coordenadas para que la imagen quede centrada.
    #if DISPLAY_ROTATION == 90
        disp_cfg.rotation.swap_xy = true;
        disp_cfg.rotation.mirror_x = true;
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 34));
    #elif DISPLAY_ROTATION == 180
        disp_cfg.rotation.mirror_x = true;
        disp_cfg.rotation.mirror_y = true;
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 34, 0));
    #elif DISPLAY_ROTATION == 270
        disp_cfg.rotation.swap_xy = true;
        disp_cfg.rotation.mirror_y = true;
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 34));
    #else // Rotación 0
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 34, 0));
    #endif

    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    // --- Configuración del driver del panel táctil para LVGL ---
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}