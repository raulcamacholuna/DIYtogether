/* Fecha: 18/08/2025 - 09:58  */
/* Fichero: components/ui/buttons/evo_4.c */
/* Último cambio: Añadida llamada al módulo de feedback y ajustado el evento a LV_EVENT_CLICKED. */
/* Descripción: Implementación del botón 'Evo Viento'. Se invoca a `button_feedback_add` para respuesta visual y se usa el evento 'CLICKED' para disparar la acción, asegurando que el feedback visual se complete antes de la ejecución de la acción. */

#include "evo_4.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"
#include "button_feedback.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
static const char *TAG = "EVO_4";

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_evo_4_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Evo Viento'.
 * @param e Puntero al evento de LVGL.
 */
static void evo_4_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "¡Evento CLICKED recibido! Ejecutando acción de evo viento.");
        execute_diymon_action(ACTION_ID_EVO_WIND);
    }
}

/**
 * @brief Crea el objeto del botón 'Evo Viento'.
 */
void evo_4_create(lv_obj_t *parent) {
    s_evo_4_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_evo_4_handle);
    lv_obj_set_size(s_evo_4_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_evo_4_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_evo_4_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_evo_4_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_evo_4_handle);
    const lv_img_dsc_t* icon_src = ui_assets_get_icon(ASSET_ICON_EVO_WIND);
    if (icon_src) {
        lv_img_set_src(img, icon_src);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el icono 'ASSET_ICON_EVO_WIND'.");
    }
    lv_obj_center(img);

    // --- Posición DENTRO de su panel padre ---
    lv_obj_align(s_evo_4_handle, LV_ALIGN_TOP_MID, 0, (BUTTON_SIZE + BUTTON_PADDING) * 3);

    // --- Conexión del evento de acción ---
    lv_obj_add_event_cb(s_evo_4_handle, evo_4_event_cb, LV_EVENT_CLICKED, NULL);

    // --- Añadir feedback visual ---
    button_feedback_add(s_evo_4_handle);

    ESP_LOGI(TAG, "Botón 'Evo Viento' (EVO_4) creado con feedback visual.");
}

/**
 * @brief Obtiene el manejador del botón 'Evo Viento'.
 */
lv_obj_t* evo_4_get_handle(void) {
    return s_evo_4_handle;
}
