/* Fecha: 18/08/2025 - 09:49  */
/* Fichero: components/ui/buttons/btn_1.c */
/* Último cambio: Corregido el evento de acción a LV_EVENT_CLICKED para restaurar la funcionalidad. */
/* Descripción: Implementación del botón 'Comer'. Se ha corregido el evento que dispara la acción de 'RELEASED' a 'CLICKED'. Este es el evento de alto nivel correcto para una pulsación, resolviendo el problema por el cual la acción no se registraba, mientras se mantiene el feedback visual en los eventos de 'press' y 'release'. */

#include "btn_1.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"
#include "button_feedback.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
static const char *TAG = "BTN_1";

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_1_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Comer'.
 * @param e Puntero al evento de LVGL.
 */
static void btn_1_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    // [CORRECCIÓN] La acción se debe ejecutar en el evento CLICKED.
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "¡Evento CLICKED recibido! Ejecutando acción de comer.");
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
    const lv_img_dsc_t* icon_src = ui_assets_get_icon(ASSET_ICON_EAT);
    if (icon_src) {
        lv_img_set_src(img, icon_src);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el icono 'ASSET_ICON_EAT'.");
    }
    lv_obj_center(img);

    // --- Posición DENTRO de su panel padre ---
    lv_obj_align(s_btn_1_handle, LV_ALIGN_LEFT_MID, (BUTTON_SIZE + BUTTON_PADDING) * 0, 0);

    // --- Conexión del evento de acción ---
    // [CORRECCIÓN] Se registra el callback para el evento CLICKED.
    lv_obj_add_event_cb(s_btn_1_handle, btn_1_event_cb, LV_EVENT_CLICKED, NULL);

    // --- Añadir feedback visual ---
    button_feedback_add(s_btn_1_handle);
    
    ESP_LOGI(TAG, "Botón 'Comer' (BTN_1) creado con feedback visual.");
}

/**
 * @brief Obtiene el manejador del botón 'Comer'.
 */
lv_obj_t* btn_1_get_handle(void) {
    return s_btn_1_handle;
}
