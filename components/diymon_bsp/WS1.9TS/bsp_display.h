/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp_display.h
 * Fecha: 08/08/2025 - 21:50
 * Último cambio: Eliminada la inclusión incorrecta del driver táctil 'axs5106'.
 * Descripción: Cabecera para el driver del display. Expone las funciones de
 *              inicialización y control del panel LCD. Se ha limpiado para
 *              eliminar dependencias de hardware incorrectas.
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
esp_lcd_panel_handle_t bsp_get_display_handle(void);
esp_lcd_panel_io_handle_t bsp_get_panel_io_handle(void);
int bsp_get_display_hres(void);
int bsp_get_display_vres(void);
size_t bsp_get_display_buffer_size(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_DISPLAY_H