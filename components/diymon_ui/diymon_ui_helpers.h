/*
 * Fichero: components/diymon_ui/diymon_ui_helpers.h
 * Descripción: Funciones de ayuda para la interfaz de usuario, como la carga
 *              de recursos dinámicos desde la tarjeta SD.
 */
#ifndef DIYMON_UI_HELPERS_H
#define DIYMON_UI_HELPERS_H

#include "lvgl.h"

/**
 * @brief Construye la ruta completa a un recurso (asset) en la SD.
 * 
 * Esta función obtiene el código de evolución actual, lo convierte a un nombre
 * de directorio compatible con 8.3 (ej: "1.1.1" -> "111"), y construye la ruta
 * final a un fichero dentro de ese directorio.
 * 
 * @param buffer Puntero al buffer donde se guardará la ruta generada.
 * @param buffer_size Tamaño del buffer.
 * @param asset_filename Nombre del fichero del recurso (ej: "IDLE.GIF"). Debe ser compatible 8.3.
 */
void ui_helpers_build_asset_path(char* buffer, size_t buffer_size, const char* asset_filename);

/**
 * @brief Carga la imagen de fondo desde el fichero BG.BIN en la SD.
 * 
 * Utiliza la función de construcción de rutas para encontrar el fichero BG.BIN
 * correspondiente a la evolución actual y lo establece como fondo del objeto padre.
 * 
 * @param parent Puntero al objeto que recibirá la imagen de fondo (normalmente, la pantalla principal).
 */
void ui_helpers_load_background(lv_obj_t* parent);

/**
 * @brief Libera la memoria utilizada por el búfer de la imagen de fondo.
 * 
 * Debe llamarse cuando la pantalla se destruye para evitar fugas de memoria.
 */
void ui_helpers_free_background_buffer();

/**
 * @brief Crea y muestra el GIF animado del DIYMON en el centro de la pantalla.
 * 
 * Carga el fichero diymon.gif correspondiente a la evolución actual en un búfer
 * de memoria y lo muestra centrado en su objeto padre.
 * 
 * @param parent Puntero al objeto sobre el que se creará el GIF (la pantalla principal).
 */
void ui_helpers_create_diymon_gif(lv_obj_t* parent);

/**
 * @brief Libera la memoria utilizada por el búfer del GIF del DIYMON.
 * 
 * Debe llamarse cuando la pantalla se destruye para evitar fugas de memoria.
 */
void ui_helpers_free_gif_buffer();


#endif // DIYMON_UI_HELPERS_H