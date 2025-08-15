/* Fecha: 15/08/2025 - 04:10  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\main\hardware_manager.h */
/* Último cambio: Añadida la declaración de hardware_manager_mount_lvgl_filesystem. */
/* Descripción: Define la interfaz pública del gestor de hardware. Se añade una nueva función para registrar explícitamente el sistema de ficheros de la tarjeta SD con LVGL, permitiendo que la librería acceda a los ficheros de animación. */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "esp_err.h"

/**
 * @brief Inicializa los periféricos de hardware base (display, touch, bsp, etc.) y el port de LVGL.
 *
 * @return ESP_OK si la inicialización fue exitosa.
 */
esp_err_t hardware_manager_init(void);

/**
 * @brief Registra el sistema de ficheros de la tarjeta SD (montada en /sdcard) con LVGL.
 * 
 * Esta función asigna la letra de unidad 'S' al punto de montaje /sdcard,
 * permitiendo que LVGL acceda a los ficheros de animación usando rutas como "S:/DIYMON/..."
 */
void hardware_manager_mount_lvgl_filesystem(void);


#endif // HARDWARE_MANAGER_H
