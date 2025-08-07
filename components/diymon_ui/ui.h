/*
 * Archivo: ui.h
 * Descripción: Interfaz PÚBLICA del componente de la UI.
 *              Esta es la ÚNICA fuente de verdad para los tipos y enums compartidos.
 * Versión: 8.0
 */
#ifndef UI_H
#define UI_H

#include "lvgl.h" // Incluimos la cabecera principal de LVGL

// --- DEFINICIONES COMPARTIDAS ---

// [DEFINICIÓN ÚNICA] Enum para identificar las pantallas.
enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

// [DEFINICIÓN ÚNICA] Enum para identificar las acciones de los botones.
typedef enum {
    ACTION_ID_COMER,
    ACTION_ID_EJERCICIO,
    ACTION_ID_ATACAR,
} diymon_action_id_t;


// --- FUNCIONES PÚBLICAS DE LA UI ---

void ui_init(void);
void ui_update_diymon_sprite(void);
void ui_update_diymon_background(void);
void ui_play_action_animation(const char* action_name);

#endif // UI_H