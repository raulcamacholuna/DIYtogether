/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_ui\ui_config_screen.h
# Fecha: $timestamp
# Último cambio: Creada la cabecera para la nueva pantalla de configuración.
# Descripción: Interfaz pública para la pantalla de configuración dinámica. Permite a `main.c` llamar a la función que muestra la pantalla de servicio con información de red.
*/
#ifndef UI_CONFIG_SCREEN_H
#define UI_CONFIG_SCREEN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Crea y muestra la pantalla de configuración con la información de red proporcionada.
 *
 * @param title El título principal de la pantalla.
 * @param ssid  El SSID de la red a mostrar.
 * @param pass  La contraseña de la red.
 * @param ip    La dirección IP del servidor.
 */
void ui_config_screen_show(const char* title, const char* ssid, const char* pass, const char* ip);

#ifdef __cplusplus
}
#endif

#endif // UI_CONFIG_SCREEN_H
