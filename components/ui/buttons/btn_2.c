/* Fecha: 17/08/2025 - 05:17  */
/* Fichero: components/ui/buttons/btn_2.c */
/* Último cambio: Corregido el posicionamiento del botón para que se mueva con su panel padre. */
/* Descripción: Se ha corregido el error de posicionamiento del botón. En lugar de tener su propia lógica de ocultación y posición fuera de pantalla, ahora se alinea estáticamente dentro de su panel padre usando 'lv_obj_align'. Esto asegura que cuando el panel se anima para mostrarse, el botón aparece correctamente en su interior, resolviendo el problema de que los paneles no se desplegaban visualmente. */

#include "btn_2.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
static const char *TAG = "BTN_2";

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_2_handle = NULL;

/**
 * @brief Callback de evento específico para el botón 'Ejercicio'.
 *        Ejecuta la acción de ejercicio cuando se presiona el botón.
 * @param e Puntero al evento de LVGL.
 */
static void btn_2_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED) {
        ESP_LOGI(TAG, "¡Evento CLICK recibido! Ejecutando acción de ejercicio.");
        execute_diymon_action(ACTION_ID_EJERCICIO);
    }
}

/**
 * @brief Crea el objeto del botón 'Ejercicio'.
 */
void btn_2_create(lv_obj_t *parent) {
    s_btn_2_handle = lv_btn_create(parent);
    lv_obj_remove_style_all(s_btn_2_handle);
    lv_obj_set_size(s_btn_2_handle, BUTTON_SIZE, BUTTON_SIZE);

    // --- Estilos del botón ---
    lv_obj_set_style_bg_opa(s_btn_2_handle, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(s_btn_2_handle, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(s_btn_2_handle, 0, LV_STATE_ANY);

    // --- Icono del botón ---
    lv_obj_t *img = lv_img_create(s_btn_2_handle);
    const lv_img_dsc_t* icon_src = ui_assets_get_icon(ASSET_ICON_GYM);
    if (icon_src) {
        lv_img_set_src(img, icon_src);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el icono 'ASSET_ICON_GYM'.");
    }
    lv_obj_center(img);

    // --- Posición DENTRO de su panel padre ---
    // [CORRECCIÓN] Se alinea el botón dentro del panel. El panel es el que se anima.
    lv_obj_align(s_btn_2_handle, LV_ALIGN_LEFT_MID, (BUTTON_SIZE + BUTTON_PADDING) * 1, 0);
    
    // --- Conexión del evento ---
    lv_obj_add_event_cb(s_btn_2_handle, btn_2_event_cb, LV_EVENT_PRESSED, NULL);
    
    ESP_LOGI(TAG, "Botón 'Ejercicio' (BTN_2) creado y posicionado dentro de su panel.");
}

/**
 * @brief Obtiene el manejador del botón 'Ejercicio'.
 */
lv_obj_t* btn_2_get_handle(void) {
    return s_btn_2_handle;
}
