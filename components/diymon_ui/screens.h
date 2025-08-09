/*
 * Fichero: ./components/diymon_ui/screens.h
 * Fecha: 10/08/2025 - 03:40
 * Último cambio: Añadida la inclusión de `lvgl.h` para resolver tipos desconocidos.
 * Descripción: Se incluye la cabecera principal de LVGL para que el compilador reconozca el tipo `lv_obj_t` y corrija el error de "unknown type name".
 */
#ifndef SCREENS_H
#define SCREENS_H

#include "ui.h" 
#include <lvgl.h> // <-- LA SOLUCIÓN AL PRIMER ERROR

// --- DECLARACIONES DE OBJETOS GLOBALES ---
// Usamos 'extern' para decirle a otros archivos .c (como ui.c)
// que estas variables existen y serán definidas en screens.c.
extern lv_obj_t *g_idle_animation_obj;
extern lv_obj_t *g_main_screen_obj;


// --- DECLARACIONES DE FUNCIONES ---
// Estas son las funciones definidas en screens.c que ui.c necesita llamar.
void create_screen_main(void);
void delete_screen_main(void);
void create_screens(void);

#endif // SCREENS_H