/*
Fichero: ./components/diymon_ui/ui_config_screen.h
Fecha: $timestamp
Último cambio: Modificada la firma de la función 'ui_config_screen_show' para que no acepte argumentos, reflejando su nueva implementación estática que muestra una imagen de fondo.
Descripción: Interfaz pública para la pantalla de configuración. Muestra una pantalla estática con una imagen de fondo y un botón de reinicio.
*/
#ifndef UI_CONFIG_SCREEN_H
#define UI_CONFIG_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea y muestra la pantalla de configuración estática con una imagen de fondo y un botón de reinicio.
 */
void ui_config_screen_show(void);

#ifdef __cplusplus
}
#endif

#endif // UI_CONFIG_SCREEN_H
