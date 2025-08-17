/* Fecha: 17/08/2025 - 01:28 
# Fichero: components\ui\buttons\evo_5.c
# Último cambio: Implementación del módulo para el botón 'Involucionar' (EVO_5).
# Descripción: Encapsula la lógica del botón 'Involucionar', incluyendo creación, posicionamiento en el panel lateral, icono (ASSET_ICON_EVO_BACK) y conexión de evento a la acción ACTION_ID_EVO_BACK.
*/
#include "evo_5.h"
#include "ui_asset_loader.h"
#include "actions.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_evo_5_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Involucionar'.
 *        Ejecuta la acción de involucionar.
 * @param e Puntero al evento de LVGL.
 */
static void evo_5_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        execute_diymon_action(ACTION_ID_EVO_BACK);
    }
}

/**
 * @brief Crea el objeto del botón 'Involucionar'.
 */
void evo_5_create(lv_obj_t *parent) {
    s_evo_5_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_evo_5_handle);
    lv_obj_set_size(s_evo_5_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_evo_5_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_evo_5_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_evo_5_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_evo_5_handle);
    lv_img_set_src(img, ui_assets_get_icon(ASSET_ICON_EVO_BACK));
    lv_obj_center(img);

    // --- Posición inicial (para animación de entrada) ---
    // El índice de este botón es 4 en el panel lateral.
    lv_obj_set_pos(s_evo_5_handle, -BUTTON_SIZE, BUTTON_PADDING + (BUTTON_SIZE + BUTTON_PADDING) * 4);
    lv_obj_add_flag(s_evo_5_handle, LV_OBJ_FLAG_HIDDEN);

    // --- Conexión del evento ---
    lv_obj_add_event_cb(s_evo_5_handle, evo_5_event_cb, LV_EVENT_CLICKED, NULL);
}

/**
 * @brief Obtiene el manejador del botón 'Involucionar'.
 */
lv_obj_t* evo_5_get_handle(void) {
    return s_evo_5_handle;
}
