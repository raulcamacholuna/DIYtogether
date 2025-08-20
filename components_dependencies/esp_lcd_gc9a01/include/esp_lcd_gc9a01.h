/* Fecha: 19/08/2025 - 03:58  */
/* Fichero: components_dependencies/esp_lcd_gc9a01/include/esp_lcd_gc9a01.h */
/* Último cambio: Creación del fichero de cabecera para el controlador GC9a01. */
/* Descripción: Interfaz pública del driver para el controlador de pantalla GC9A01. Declara la función de inicialización sp_lcd_new_panel_gc9a01 que permite crear una nueva instancia del panel LCD. */

/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create LCD panel for model GC9A01
 *
 * @param[in] io LCD panel IO handle
 * @param[in] panel_dev_config general panel device configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *          - ESP_OK: on success
 *
 */
esp_err_t esp_lcd_new_panel_gc9a01(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
