/*
 * Archivo: screens.h
 * Descripción: Declaraciones privadas para el módulo de pantallas.
 * Versión: 8.0
 */
#ifndef SCREENS_H
#define SCREENS_H

#include "ui.h" // [AÑADIDO] Incluimos la API pública para obtener los enums correctos.

// [ELIMINADO] El enum ScreensEnum se ha movido a ui.h para ser la única fuente de verdad.

// --- DECLARACIONES DE OBJETOS GLOBALES ---
// Usamos 'extern' para decirle a otros archivos .c (como ui.c)
// que estas variables existen y serán definidas en screens.c.
extern lv_obj_t *g_diymon_gif_obj;
extern lv_obj_t *g_background_obj;

// --- DECLARACIONES DE FUNCIONES ---
// Estas son las funciones definidas en screens.c que ui.c necesita llamar.
void create_screens(void);

#endif // SCREENS_H