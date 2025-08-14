/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\main\ui_config_screen.c
# Fecha: $timestamp
# Último cambio: Creada la nueva pantalla de configuración con LVGL.
# Descripción: Implementa una pantalla de servicio dinámica para los modos de configuración (WiFi/FTP), mostrando la información de conexión y un botón de reinicio. Reemplaza las imágenes estáticas .bin.
*/
#include "ui_config_screen.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
 * @brief Crea y muestra la pantalla de configuración.
 */
void ui_config_screen_show(const char* title, const char* ssid, const char* pass, const char* ip) {
    // Crear una pantalla nueva y limpia
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x2c3e50), LV_PART_MAIN);
    lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(scr, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_hor(scr, 10, LV_PART_MAIN);

    // Estilo general para los labels
    static lv_style_t style_text;
    lv_style_init(&style_text);
    lv_style_set_text_color(&style_text, lv_color_white());
    lv_style_set_text_align(&style_text, LV_TEXT_ALIGN_CENTER);
    
    // Título
    lv_obj_t *lbl_title = lv_label_create(scr);
    lv_obj_add_style(lbl_title, &style_text, 0);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0); // Usamos una fuente disponible

    // Contenedor para los datos
    lv_obj_t* data_cont = lv_obj_create(scr);
    lv_obj_remove_style_all(data_cont);
    lv_obj_set_layout(data_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(data_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(data_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(data_cont, 10, 0);

    char buffer[64];
    // SSID
    snprintf(buffer, sizeof(buffer), "SSID: %s", ssid);
    lv_obj_t *lbl_ssid = lv_label_create(data_cont);
    lv_obj_add_style(lbl_ssid, &style_text, 0);
    lv_label_set_text(lbl_ssid, buffer);

    // Contraseña
    snprintf(buffer, sizeof(buffer), "Pass: %s", pass);
    lv_obj_t *lbl_pass = lv_label_create(data_cont);
    lv_obj_add_style(lbl_pass, &style_text, 0);
    lv_label_set_text(lbl_pass, buffer);

    // IP
    snprintf(buffer, sizeof(buffer), "IP: %s", ip);
    lv_obj_t *lbl_ip = lv_label_create(data_cont);
    lv_obj_add_style(lbl_ip, &style_text, 0);
    lv_label_set_text(lbl_ip, buffer);

    // Botón de Reinicio
    lv_obj_t *btn_reset = lv_btn_create(scr);
    lv_obj_add_event_cb(btn_reset, reset_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *lbl_btn = lv_label_create(btn_reset);
    lv_label_set_text(lbl_btn, "Reiniciar");
    lv_obj_center(lbl_btn);

    // Cargar la nueva pantalla
    lv_screen_load(scr);
    ESP_LOGI(TAG, "Pantalla de configuración dinámica mostrada.");
}
