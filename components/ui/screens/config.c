/* Fecha: 17/08/2025 - 03:42  */
/* Fichero: components/ui/screens/config.c */
/* Último cambio: Renombrada la función 'config_screen_show' a 'config_show' para que coincida con su declaración y resolver el error del enlazador. */
/* Descripción: Implementación de la pantalla de configuración. El error de 'undefined reference' se debía a que la definición de la función en este fichero no coincidía con la declaración en el header y la llamada en main.c. Se ha estandarizado el nombre a 'config_show'. */

#include "config.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "assets/images/BG_config.h"

static const char *TAG = "UI_CONFIG_SCREEN";

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
void config_show(void) {
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
