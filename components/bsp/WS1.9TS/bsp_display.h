/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp_display.h
 * Fecha: 12/08/2025 - 11:46
 * Último cambio: Añadidas las declaraciones para encendido/apagado del display.
 * Descripción: Cabecera privada para el driver del display. Se añaden las declaraciones de las funciones que controlan el estado de encendido del panel.
 */
#ifndef BSP_DISPLAY_H
#define BSP_DISPLAY_H

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

// Constantes de la pantalla
#define BSP_LCD_H_RES           170
#define BSP_LCD_V_RES           320

// Funciones exportadas por bsp_display.c
esp_err_t bsp_display_init(void);
void bsp_display_set_brightness(int percentage);
void bsp_display_turn_on(void);
void bsp_display_turn_off(void);
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_DISPLAY_H