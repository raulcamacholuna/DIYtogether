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
 *        Created:  [Fecha de hoy]
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
 * Esta función debe ser llamada una sola vez al inicio del programa desde app_main.
 * Se encarga de configurar I2C, SPI, la pantalla, el panel táctil, los sensores,
 * la batería y los drivers de LVGL.
 *
 * @return ESP_OK si la inicialización fue exitosa, o un código de error en caso contrario.
 */
esp_err_t hardware_manager_init(void);

/*
 * NOTA: En el futuro, se pueden añadir aquí más funciones para interactuar
 * con el hardware desde otras partes del código. Por ejemplo:
 *
 * float hardware_manager_get_battery_voltage(void);
 * void hardware_manager_set_brightness(uint8_t percentage);
 *
 */

#endif // HARDWARE_MANAGER_H