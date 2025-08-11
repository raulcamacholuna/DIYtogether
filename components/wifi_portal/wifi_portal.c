#include "wifi_portal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "lwip/netif.h"
#include "lvgl.h"
#include "esp_mac.h"
#include "esp_lvgl_port.h"

#define PORTAL_AP_SSID          "DIYTogether"
#define PORTAL_AP_PASS          "MakeItYours"
#define PORTAL_AP_CHANNEL       1
#define PORTAL_AP_MAX_CONN      1

static const char *TAG = "WIFI_PORTAL";

static EventGroupHandle_t s_portal_event_group;
const int CREDENTIALS_RECEIVED_BIT = BIT0;
const int SKIP_BIT = BIT1; // <-- ANOTACIÓN: Nuevo bit para el botón de omitir.

static const char* HTML_FORM =
    "<!DOCTYPE html><html><head><title>Configuracion WiFi DIYMON</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>"
    "<body><h1>Configura el WiFi de tu DIYMON</h1>"
    "<form method='post' action='/save'>"
    "<p><label>SSID (Nombre de tu red):</label><br><input type='text' name='ssid'></p>"
    "<p><label>Password:</label><br><input type='password' name='password'></p>"
    "<p><button type='submit'>Guardar y Reiniciar</button></p>"
    "</form></body></html>";


static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, HTML_FORM, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t save_post_handler(httpd_req_t *req) {
    char buf[256];
    int ret, remaining = req->content_len;
    if (remaining > sizeof(buf) - 1) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';
    char ssid[32] = {0};
    char password[64] = {0};
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password, sizeof(password));

    ESP_LOGI(TAG, "Credenciales recibidas - SSID: [%s]", ssid);

    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_pass", password));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales guardadas en NVS.");

    httpd_resp_send(req, "<h1>Configuracion guardada!</h1><p>El dispositivo se reiniciara.</p>", HTTPD_RESP_USE_STRLEN);
    
    xEventGroupSetBits(s_portal_event_group, CREDENTIALS_RECEIVED_BIT);
    
    return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = root_get_handler};
        httpd_register_uri_handler(server, &root);
        httpd_uri_t save = {.uri = "/save", .method = HTTP_POST, .handler = save_post_handler};
        httpd_register_uri_handler(server, &save);
    }
    return server;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "Dispositivo conectado al AP");
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI(TAG, "Dispositivo desconectado del AP");
    }
}

static void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    wifi_config_t wifi_config = {
        .ap = {.ssid = PORTAL_AP_SSID, .password = PORTAL_AP_PASS, .ssid_len = strlen(PORTAL_AP_SSID), .channel = PORTAL_AP_CHANNEL, .authmode = WIFI_AUTH_WPA2_PSK, .max_connection = PORTAL_AP_MAX_CONN}};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// --- ANOTACIÓN: Callback para el botón de omitir. ---
static void skip_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        xEventGroupSetBits(s_portal_event_group, SKIP_BIT);
    }
}

static void display_portal_info_on_screen(void) {
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(scr, 15, LV_PART_MAIN);

    const char *texts[] = {"SSID: DIYTogether", "PASS: MakeItYours", "IP: 192.168.4.1"};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *label = lv_label_create(scr);
        lv_label_set_text(label, texts[i]);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    }
    
    // --- ANOTACIÓN: Texto de instrucciones actualizado. ---
    lv_obj_t *info_label = lv_label_create(scr);
    lv_label_set_text(info_label, "Conectate a esta red desde tu movil\ny visita la IP para configurar el WiFi.");
    lv_obj_set_style_text_color(info_label, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);

    // --- ANOTACIÓN: Creación del botón de omitir. ---
    lv_obj_t *skip_btn = lv_btn_create(scr);
    lv_obj_add_event_cb(skip_btn, skip_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_pad_top(skip_btn, 20, LV_PART_MAIN); // Añade espacio extra arriba
    lv_obj_t *label_btn = lv_label_create(skip_btn);
    lv_label_set_text(label_btn, "Omitir");
    lv_obj_center(label_btn);

    lv_screen_load(scr);
}

bool wifi_portal_credentials_saved(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) return false;

    size_t required_size = 0;
    err = nvs_get_str(nvs_handle, "wifi_ssid", NULL, &required_size);
    nvs_close(nvs_handle);

    if (err == ESP_OK && required_size > 1) { // >1 para no contar un string vacío.
        ESP_LOGI(TAG, "Credenciales WiFi encontradas en NVS.");
        return true;
    }
    
    ESP_LOGI(TAG, "No se encontraron credenciales WiFi en NVS.");
    return false;
}

wifi_portal_result_t wifi_portal_start(void) {
    s_portal_event_group = xEventGroupCreate();
    
    wifi_init_softap();
    httpd_handle_t server = start_webserver();

    if (server == NULL) {
        ESP_LOGE(TAG, "Error al iniciar el servidor web.");
        return PORTAL_CONFIG_SKIPPED; // Devuelve skip como un error seguro.
    }
    
    if(lvgl_port_lock(0)) {
        display_portal_info_on_screen();
        lvgl_port_unlock();
    }

    ESP_LOGI(TAG, "Portal en marcha. Esperando interaccion del usuario...");

    EventBits_t bits = xEventGroupWaitBits(s_portal_event_group,
                                           CREDENTIALS_RECEIVED_BIT | SKIP_BIT,
                                           pdTRUE, pdFALSE, portMAX_DELAY);

    wifi_portal_result_t result;
    if (bits & CREDENTIALS_RECEIVED_BIT) {
        ESP_LOGI(TAG, "Credenciales recibidas, deteniendo portal...");
        result = PORTAL_CONFIG_SAVED;
    } else {
        ESP_LOGI(TAG, "Portal omitido por el usuario...");
        result = PORTAL_CONFIG_SKIPPED;
    }

    httpd_stop(server);
    esp_wifi_stop();
    esp_netif_destroy_default_wifi(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"));
    vEventGroupDelete(s_portal_event_group);

    return result;
}