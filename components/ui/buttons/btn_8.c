/* Fichero: components/ui/buttons/btn_8.c */
/* Descripción: Diagnóstico: Se necesita una forma de probar la comunicación Zigbee desde la UI. Causa Raíz: El botón 8 estaba asignado a una acción de modo rendimiento. Solución Definitiva: Se ha modificado el callback 'btn_8_event_cb'. Ahora, si el dispositivo está compilado como Router ('CONFIG_ESP_ZB_ZR_ENABLED'), al pulsar el botón se crea un paquete de prueba ('game_packet_t') y se envía al Coordinador (dirección 0x0000) usando la nueva API 'zigbee_comm_send_unicast'. Esto proporciona un método de prueba simple y directo para la nueva funcionalidad Zigbee. */
/* Último cambio: 23/08/2025 - 10:52 */
#include "btn_8.h"
#include "ui_asset_loader.h"
#include "actions.h"
#include "esp_log.h"
#include "button_feedback.h"
#include "zigbee_comm.h" // Incluir la nueva cabecera

// --- Definiciones de diseño locales ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10
static const char *TAG = "BTN_8";

// --- Variable estática para el manejador del botón ---
static lv_obj_t *s_btn_8_handle = NULL;
static bool s_animations_enabled = false; // Nueva variable para el estado de las animaciones

/**
 * @brief Callback de evento para el botón 8. Ahora envía un paquete Zigbee de prueba o activa el modo rendimiento.
 */
static void btn_8_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (s_animations_enabled) {
            ESP_LOGI(TAG, "Botón 8 presionado. Activando modo rendimiento.");
            execute_diymon_action(ACTION_ID_PERFORMANCE_MODE);
        } else {
        #if defined(CONFIG_ESP_ZB_ZR_ENABLED) // Solo el Router (cliente) envía el paquete de prueba
            ESP_LOGI(TAG, "Botón 8 (Enviar Test Zigbee) presionado.");
            game_packet_t test_packet = {
                .game_id = 1, // ID de juego de prueba
                .len = 5,
            };
            memcpy(test_packet.payload, "HELLO", 5);
            
            // El coordinador siempre tiene la dirección 0x0000
            esp_err_t err = zigbee_comm_send_unicast(0x0000, &test_packet);
            if (err == ESP_OK) {
                ESP_LOGI(TAG, "Paquete de prueba Zigbee enviado al coordinador.");
            } else {
                ESP_LOGE(TAG, "Fallo al enviar paquete de prueba Zigbee: %s", esp_err_to_name(err));
            }
        #else
            ESP_LOGI(TAG, "Botón 8 presionado. (Acción de envío solo para Routers)");
        #endif
        }
    }
}

/**
 * @brief Crea el objeto del botón 8.
 */
void btn_8_create(lv_obj_t *parent, bool animations_enabled) {
    s_animations_enabled = animations_enabled; // Almacenar el estado de las animaciones
    s_btn_8_handle = lv_btn_create(parent);
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
    
    ESP_LOGI(TAG, "Botón 8 (Enviar Test Zigbee) creado con feedback visual.");
}

/**
 * @brief Obtiene el manejador del botón 8.
 */
lv_obj_t* btn_8_get_handle(void) {
    return s_btn_8_handle;
}
