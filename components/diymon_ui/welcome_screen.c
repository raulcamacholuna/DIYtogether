#include "welcome_screen.h"
#include "esp_log.h"

static const char *TAG = "WELCOME_SCREEN";

// --- ANOTACIÓN: Puntero estático para almacenar el callback de inicio. ---
// Guardamos la función que nos pasa 'main' para poder llamarla cuando se pulse el botón.
static void (*g_on_start_callback)(void) = NULL;

/**
 * @brief Callback del evento del botón "INICIAR".
 *        Llama a la función de callback registrada si existe.
 */
static void start_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        ESP_LOGI(TAG, "Botón 'INICIAR' presionado.");
        if (g_on_start_callback) {
            g_on_start_callback();
        }
    }
}

/**
 * @brief Implementación de la creación de la pantalla de bienvenida.
 */
void welcome_screen_show(void (*on_start_cb)(void)) {
    g_on_start_callback = on_start_cb;

    // --- ANOTACIÓN: Creación de la pantalla y sus elementos. ---
    // Se crea un nuevo screen para asegurar que no hay elementos previos.
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(scr, 20, LV_PART_MAIN);

    const char *texts[] = {"SSID: DIYTogether", "PASS: MakeItYours", "192.168.4.1"};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, texts[i]);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    }

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_add_event_cb(btn, start_button_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label_btn = lv_label_create(btn);
    lv_label_set_text(label_btn, "INICIAR");
    lv_obj_center(label_btn);

    // --- ANOTACIÓN: Se carga la nueva pantalla. ---
    // Esta llamada reemplaza la pantalla activa anterior y evita el crash.
    lv_screen_load(scr);
    ESP_LOGI(TAG, "Pantalla de bienvenida mostrada.");
}