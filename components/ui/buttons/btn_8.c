/* Fecha: 18/08/2025 - 06:55  */
/* Fichero: components/ui/buttons/btn_8.c */
/* Último cambio: Reasignada la acción del botón a ACTION_ID_CONFIG_PLACEHOLDER tras eliminar el modo de servidor de ficheros. */
/* Descripción: Este botón activaba anteriormente el modo de servidor de ficheros. Como esa funcionalidad ahora está integrada en el modo de configuración principal, el botón se ha convertido en un placeholder. Ahora dispara una acción genérica que no realiza ninguna operación, manteniendo la consistencia de la UI mientras se rediseña su función. */

#include "btn_8.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"

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
    if (code == LV_EVENT_PRESSED) {
        ESP_LOGI(TAG, "Botón 8 (placeholder) presionado.");
        // [CORRECCIÓN] La acción del servidor de ficheros ha sido eliminada.
        // Ahora se dispara una acción placeholder que no hace nada.
        execute_diymon_action(ACTION_ID_CONFIG_PLACEHOLDER);
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
    // [CORRECCIÓN] Se mantiene el icono original por ahora, pero la acción ha cambiado.
    const lv_img_dsc_t* icon_src = ui_assets_get_icon(ASSET_ICON_ENABLE_FILE_SERVER);
    if (icon_src) {
        lv_img_set_src(img, icon_src);
    } else {
        ESP_LOGE(TAG, "Fallo al cargar el icono 'ASSET_ICON_ENABLE_FILE_SERVER'.");
    }
    lv_obj_center(img);

    // --- Posición DENTRO de su panel padre ---
    lv_obj_align(s_btn_8_handle, LV_ALIGN_LEFT_MID, (BUTTON_SIZE + BUTTON_PADDING) * 1, 0);

    // --- Conexión del evento ---
    lv_obj_add_event_cb(s_btn_8_handle, btn_8_event_cb, LV_EVENT_PRESSED, NULL);
    
    ESP_LOGI(TAG, "Botón 8 (placeholder) creado y posicionado.");
}

/**
 * @brief Obtiene el manejador del botón 8.
 */
lv_obj_t* btn_8_get_handle(void) {
    return s_btn_8_handle;
}
