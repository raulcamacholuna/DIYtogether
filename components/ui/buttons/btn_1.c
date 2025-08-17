/* Fecha: 17/08/2025 - 01:18 
# Fichero: components/ui/buttons/btn_1.c
# Último cambio: Implementación del módulo para el botón 'Comer' (BTN_1) como parte de la refactorización.
# Descripción: Este fichero encapsula toda la lógica para el botón 'Comer': su creación, la asignación del icono, el posicionamiento inicial y la conexión de su evento de clic a la acción ACTION_ID_COMER. Esto elimina la dependencia de ui_actions_panel.c y ui.c para este botón específico.
*/
#include "btn_1.h"
#include "ui_asset_loader.h"
#include "actions.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_1_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Comer'.
 *        Ejecuta la acción de comer cuando se presiona el botón.
 * @param e Puntero al evento de LVGL.
 */
static void btn_1_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        execute_diymon_action(ACTION_ID_COMER);
    }
}

/**
 * @brief Crea el objeto del botón 'Comer'.
 */
void btn_1_create(lv_obj_t *parent) {
    s_btn_1_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_btn_1_handle);
    lv_obj_set_size(s_btn_1_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_btn_1_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_btn_1_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_btn_1_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_btn_1_handle);
    lv_img_set_src(img, ui_assets_get_icon(ASSET_ICON_EAT));
    lv_obj_center(img);

    // --- Posición inicial (para animación de entrada) ---
    // El índice de este botón es 0 en su panel.
    lv_obj_set_pos(s_btn_1_handle, (BUTTON_SIZE + BUTTON_PADDING) * 0, -BUTTON_SIZE);
    lv_obj_add_flag(s_btn_1_handle, LV_OBJ_FLAG_HIDDEN);

    // --- Conexión del evento ---
    lv_obj_add_event_cb(s_btn_1_handle, btn_1_event_cb, LV_EVENT_PRESSED, NULL);
}

/**
 * @brief Obtiene el manejador del botón 'Comer'.
 */
lv_obj_t* btn_1_get_handle(void) {
    return s_btn_1_handle;
}
