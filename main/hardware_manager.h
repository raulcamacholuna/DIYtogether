/*
 * =====================================================================================
 *
 *       Filename:  hardware_manager.h
 *
 *    Description:  Archivo de cabecera para el gestor de hardware de DIYMON.
 *                  Define la interfaz pública para inicializar y controlar
 *                  los periféricos del dispositivo.
 *
 *        Version:  1.0
 *        Created:  08/08/2025
 *
 *         Author:  Raúl Camacho Luna (con revisión de IA)
 *   Organization:  DIYMON Project
 *
 * =====================================================================================
 */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "esp_err.h"

/**
 * @brief Inicializa todos los periféricos de hardware necesarios para DIYMON.
 *
 * Esta función debe ser llamada una sola vez al inicio del programa.
 * Se encarga de configurar la pantalla, el panel táctil y los drivers de LVGL.
 *
 * @return ESP_OK si la inicialización fue exitosa, o un código de error en caso contrario.
 */
esp_err_t hardware_manager_init(void);

#endif // HARDWARE_MANAGER_H