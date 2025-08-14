/*
Fichero: ./components/diymon_ui/ui_config_screen.c
Fecha: $timestamp
Último cambio: Modificado para mostrar una imagen de fondo I8 estática (bg_config_img) y un único botón de reinicio, eliminando la visualización dinámica de datos de red.
Descripción: Implementa la pantalla de servicio para los modos de configuración. Ahora muestra una imagen de fondo fija y un botón de reinicio, simplificando la interfaz para el usuario.
*/
#include "ui_config_screen.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "UI_CONFIG_SCREEN";

// Declaración externa del descriptor de la imagen de fondo compilada en el firmware.
// Se espera que este símbolo esté definido en otro fichero .c (ej: BG_config.c).
extern const lv_img_dsc_t bg_config;

/**
 * @brief Callback que se ejecuta al presionar el botón de reinicio.
 */
static void reset_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGW(TAG, "Botón de reinicio presionado. Reiniciando el dispositivo...");
        vTaskDelay(pdMS_TO_TICKS(500)); // Pequeña pausa para que el log se envíe
        esp_restart();
    }
}

/**
 * @brief Crea y muestra la pantalla de configuración estática.
 */
void ui_config_screen_show(void) {
    // Crear una pantalla nueva y limpia
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_remove_style_all(scr); // Empezamos sin estilos por defecto
    lv_obj_set_size(scr, 170, 320);

    // Crear un objeto de imagen para el fondo de pantalla
    lv_obj_t *bg_img = lv_image_create(scr);
    lv_image_set_src(bg_img, &bg_config);
    lv_obj_set_pos(bg_img, 0, 0);
    lv_obj_add_flag(bg_img, LV_OBJ_FLAG_IGNORE_LAYOUT); // Asegura que no afecte a otros elementos

    // Botón de Reinicio
    lv_obj_t *btn_reset = lv_btn_create(scr);
    lv_obj_add_event_cb(btn_reset, reset_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_align(btn_reset, LV_ALIGN_BOTTOM_MID, 0, -20); // Centrado abajo con padding

    lv_obj_t *lbl_btn = lv_label_create(btn_reset);
    lv_label_set_text(lbl_btn, "Reiniciar");
    lv_obj_center(lbl_btn);

    // Cargar la nueva pantalla
    lv_screen_load(scr);
    ESP_LOGI(TAG, "Pantalla de configuración estática con fondo I8 mostrada.");
}
