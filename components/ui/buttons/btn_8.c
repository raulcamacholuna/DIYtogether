/* Fecha: 18/08/2025 - 09:55  */
/* Fichero: components/ui/buttons/btn_8.c */
/* Último cambio: Añadida llamada al módulo de feedback y ajustado el evento a LV_EVENT_CLICKED. */
/* Descripción: Implementación del botón placeholder (antes Servidor Ficheros). Se invoca a `button_feedback_add` para respuesta visual y se usa el evento 'CLICKED' para disparar la acción. */

#include "btn_8.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"
#include "button_feedback.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
static const char *TAG = "BTN_8";

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_8_handle = NULL;

/**
 * @brief Callback de evento para el botón 8.
 */
static void btn_8_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Botón 8 (placeholder) presionado.");
        execute_diymon_action(ACTION_ID_PERFORMANCE_MODE);
    }
}

/**
 * @brief Crea el objeto del botón 8.
 */
void btn_8_create(lv_obj_t *parent) {
    s_btn_8_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_btn_8_handle);
    lv_obj_set_size(s_btn_8_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_btn_8_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_btn_8_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_btn_8_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_btn_8_handle);
    const lv_img_dsc_t* icon_src = ui_assets_get_icon(ASSET_ICON_ENABLE_FILE_SERVER);
    if (icon_src) {
        lv_img_set_src(img, icon_src);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el icono 'ASSET_ICON_ENABLE_FILE_SERVER'.");
    }
    lv_obj_center(img);

    // --- Posición DENTRO de su panel padre ---
    lv_obj_align(s_btn_8_handle, LV_ALIGN_LEFT_MID, (BUTTON_SIZE + BUTTON_PADDING) * 1, 0);

    // --- Conexión del evento de acción ---
    lv_obj_add_event_cb(s_btn_8_handle, btn_8_event_cb, LV_EVENT_CLICKED, NULL);
    
    // --- Añadir feedback visual ---
    button_feedback_add(s_btn_8_handle);
    
    ESP_LOGI(TAG, "Botón 8 (placeholder) creado con feedback visual.");
}

/**
 * @brief Obtiene el manejador del botón 8.
 */
lv_obj_t* btn_8_get_handle(void) {
    return s_btn_8_handle;
}
