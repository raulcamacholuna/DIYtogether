/*
 * =====================================================================================
 *
 *       Filename:  hardware_manager.c
 *
 *    Description:  Implementación del gestor de hardware para DIYMON.
 *                  Centraliza la inicialización y configuración de todos los
 *                  periféricos del dispositivo.
 *
 *        Version:  1.1 (Con brillo configurable)
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
#include "lv_fs_if.h" 
#include "lvgl.h"
#include "lvgl_helpers.h"

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

// [NUEVO] Configuración del Brillo
#define DEFAULT_BRIGHTNESS           50    // Brillo inicial en porcentaje (0-100)

// Determina la resolución de la pantalla según la rotación
#if DISPLAY_ROTATION == 90 || DISPLAY_ROTATION == 270
#define LCD_H_RES (320)
#define LCD_V_RES (172)
#else
#define LCD_H_RES (172)
#define LCD_V_RES (320)
#endif


/* --- Variables estáticas del módulo --- */
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_touch_handle_t touch_handle = NULL;

static lv_display_t *lvgl_disp = NULL;
static lv_indev_t *lvgl_touch_indev = NULL;


/* --- Declaraciones de funciones privadas --- */
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
    bsp_qmi8658_init(i2c_bus_handle);
    bsp_sdcard_init();

    ESP_LOGI(TAG, "Initializing display and touch panel...");
    bsp_display_init(&io_handle, &panel_handle, LCD_H_RES * DISPLAY_BUFF_HEIGHT);
    bsp_touch_init(&touch_handle, i2c_bus_handle, LCD_H_RES, LCD_V_RES, DISPLAY_ROTATION);

    ESP_LOGI(TAG, "Initializing LVGL graphics library driver...");
    ESP_ERROR_CHECK(lvgl_driver_init());

    //ESP_LOGI(TAG, "Initializing display brightness control...");
    //bsp_display_brightness_init();
    //bsp_display_set_brightness(DEFAULT_BRIGHTNESS); // [MODIFICADO] Usa la nueva constante

    // --- [NUEVO] REGISTRO DEL SISTEMA DE FICHEROS PARA LVGL ---
    // Le decimos a LVGL que el disco 'S' corresponde a la ruta "/sdcard"
    // que es donde el driver BSP monta la tarjeta SD.
    lvgl_fs_init();
    ESP_LOGI(TAG, "LVGL File System initialized and SD card registered.");

    return ESP_OK;
}

static esp_err_t lvgl_driver_init(void)
{
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 1024 * 10,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

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
    #else
        ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 34, 0));
    #endif

    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = touch_handle,
    };
    lvgl_touch_indev = lvgl_port_add_touch(&touch_cfg);

    return ESP_OK;
}