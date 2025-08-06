/*
 * =====================================================================================
 *       Filename:  hardware_manager.c
 *    Description:  Implementación del gestor de hardware para DIYMON.
 *        Version:  2.1 (ARQUITECTURA CON LVGL PORT CORRECTO)
 * =====================================================================================
 */

#include "hardware_manager.h"

// Includes generales del sistema y drivers
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Includes de la BSP (Board Support Package)
#include "bsp_api.h" 

// Includes de LVGL
#include "esp_lvgl_port.h"

// TAG para los logs de este fichero
static const char *TAG = "HW_MANAGER";

#define DEFAULT_BRIGHTNESS           50

/* --- Declaraciones de funciones privadas --- */
// [EXPLICACIÓN] Ya no necesitamos esta función, su lógica se integra en el init.
// static esp_err_t lvgl_driver_init(void);

/* --- Implementación de funciones --- */

esp_err_t hardware_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing Board Support Package hardware...");
    bsp_i2c_init();
    bsp_spi_init();
    bsp_display_init(); // Esto prepara el hardware del display
    bsp_touch_init();   // Esto prepara el hardware del táctil
    bsp_sdcard_init();  // Esto prepara el hardware de la SD
    
    // --- PASO 1: Inicializar el motor de LVGL ---
    // Esto crea la tarea que gestionará LVGL, su temporizador, etc.
    ESP_LOGI(TAG, "Initializing LVGL core...");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    // --- PASO 2: Registrar el DISPLAY en LVGL ---
    // Ahora le decimos a LVGL: "¡Usa esta pantalla para dibujar!"
    ESP_LOGI(TAG, "Registering display with LVGL...");
    // Creamos una configuración de display para el port
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = bsp_get_panel_io_handle(),     // Obtenemos el IO handle desde la BSP
        .panel_handle = bsp_get_display_handle(),   // Obtenemos el panel handle desde la BSP
        .buffer_size = bsp_get_display_buffer_size(), // Dejamos que la BSP decida el tamaño
        .double_buffer = 1,                         // Usamos doble buffer
        .hres = bsp_get_display_hres(),             // La resolución horizontal desde la BSP
        .vres = bsp_get_display_vres(),             // La resolución vertical desde la BSP
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true, // Usar DMA para el buffer es más rápido
        }
    };
    // Añadimos la pantalla a LVGL
    ESP_ERROR_CHECK(lvgl_port_add_disp(&disp_cfg));
    
    // --- PASO 3: Registrar el panel TÁCTIL en LVGL ---
    // Ahora le decimos a LVGL: "¡Usa este panel táctil para la entrada!"
    ESP_LOGI(TAG, "Registering touch input with LVGL...");
    lvgl_port_indev_cfg_t indev_cfg = {
        .disp = lvgl_port_get_disp_default(), // Lo asociamos a la pantalla por defecto
        .indev_type = LV_INDEV_TYPE_POINTER,
        .handle = bsp_get_touch_handle()      // Obtenemos el handle del táctil desde la BSP
    };
    ESP_ERROR_CHECK(lvgl_port_add_indev(&indev_cfg));
    
    ESP_LOGI(TAG, "Initializing display brightness control...");
    bsp_display_set_brightness(DEFAULT_BRIGHTNESS);
    
    // [EXPLICACIÓN] La función lvgl_fs_init() no es parte de esta librería.
    // La gestión del sistema de ficheros se hace montando la SD en el VFS de ESP-IDF
    // y luego registrando un driver en LVGL. Es un paso más avanzado que podemos
    // ver después. Por ahora lo quitamos para que el núcleo funcione.
    ESP_LOGI(TAG, "Hardware Manager initialized successfully!");

    return ESP_OK;
}