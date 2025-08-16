/*
 * Fichero: ./components/diymon_ui/screens.h
 * Fecha: 13/08/2025 - 09:34 
 * Último cambio: Eliminado el puntero g_idle_animation_obj obsoleto.
 * Descripción: Se ha limpiado la cabecera eliminando la declaración del puntero g_idle_animation_obj, ya que la animación de reposo ahora utiliza el objeto de imagen compartido.
 */
#ifndef SCREENS_H
#define SCREENS_H

#include "ui.h" 
#include <lvgl.h>

// --- DECLARACIONES DE OBJETOS GLOBALES ---
// Usamos 'extern' para decirle a otros archivos .c (como ui.c)
// que estas variables existen y serán definidas en screens.c.
extern lv_obj_t *g_main_screen_obj;


// --- DECLARACIONES DE FUNCIONES ---
// Estas son las funciones definidas en screens.c que ui.c necesita llamar.
void create_screen_main(void);
void delete_screen_main(void);
void create_screens(void);

#endif // SCREENS_H
