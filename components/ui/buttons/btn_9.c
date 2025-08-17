/* Fecha: 17/08/2025 - 01:25 
# Fichero: components\ui\buttons\btn_9.c
# Último cambio: Implementación del módulo para el botón 'Placeholder de Config' (BTN_9).
# Descripción: Encapsula la lógica del botón placeholder del panel de configuración, incluyendo creación, estilo, icono (ASSET_ICON_CONFIG_PLACEHOLDER) y conexión de evento a la acción ACTION_ID_CONFIG_PLACEHOLDER.
*/
#include "btn_9.h"
#include "ui_asset_loader.h"
#include "actions.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_9_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Placeholder de Config'.
 *        Ejecuta la acción de placeholder.
 * @param e Puntero al evento de LVGL.
 */
static void btn_9_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        execute_diymon_action(ACTION_ID_CONFIG_PLACEHOLDER);
    }
}

/**
 * @brief Crea el objeto del botón 'Placeholder de Config'.
 */
void btn_9_create(lv_obj_t *parent) {
    s_btn_9_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_btn_9_handle);
    lv_obj_set_size(s_btn_9_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_btn_9_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_btn_9_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_btn_9_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_btn_9_handle);
    lv_img_set_src(img, ui_assets_get_icon(ASSET_ICON_CONFIG_PLACEHOLDER));
    lv_obj_center(img);

    // --- Posición inicial (para animación de entrada) ---
    // El índice de este botón es 2 en su panel.
    lv_obj_set_pos(s_btn_9_handle, (BUTTON_SIZE + BUTTON_PADDING) * 2, -BUTTON_SIZE);
    lv_obj_add_flag(s_btn_9_handle, LV_OBJ_FLAG_HIDDEN);

    // --- Conexión del evento ---
    lv_obj_add_event_cb(s_btn_9_handle, btn_9_event_cb, LV_EVENT_CLICKED, NULL);
}

/**
 * @brief Obtiene el manejador del botón 'Placeholder de Config'.
 */
lv_obj_t* btn_9_get_handle(void) {
    return s_btn_9_handle;
}
