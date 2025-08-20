/* Fichero: components/web_server/web_server.c */
/* Descripción: Diagnóstico de Causa Raíz: El error de socket 'error in send : 11' (EAGAIN) indica que el buffer de envío TCP se está llenando. Esto ocurre porque la tarea del servidor web no obtiene suficiente tiempo de CPU para enviar datos a la red, siendo interrumpida por otras tareas de menor prioridad pero de ejecución más frecuente, como la tarea de LVGL.
Solución Definitiva: Se ha elevado la prioridad de la tarea del servidor HTTP de 3 a 2. Al ser una prioridad numéricamente más baja (y por tanto, mayor), se garantiza que el planificador de FreeRTOS le dará preferencia sobre la tarea de LVGL (prioridad 4), permitiéndole vaciar el buffer de envío de la red de manera más eficiente y evitando el desbordamiento que causa el error. */
/* Último cambio: 20/08/2025 - 08:00 */
#include "web_server.h"
#include "web_server_priv.h" // Cabecera privada con las declaraciones de los handlers
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "WEB_SERVER";

// --- Funciones Públicas ---

httpd_handle_t web_server_start(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 8192;
    config.task_priority = 2; // Prioridad elevada para garantizar el rendimiento de la red sobre la UI.

    ESP_LOGI(TAG, "Iniciando servidor web de configuracion (Prioridad Tarea: %d).", config.task_priority);

    if (httpd_start(&server, &config) == ESP_OK) {
        // --- Registro de URI Handlers ---
        // Las implementaciones están en web_server_handlers.c
        
        httpd_uri_t root_uri = { .uri = "/", .method = HTTP_GET, .handler = root_get_handler };
        httpd_register_uri_handler(server, &root_uri);
        
        httpd_uri_t backup_uri = { .uri = "/backup", .method = HTTP_GET, .handler = backup_get_handler };
        httpd_register_uri_handler(server, &backup_uri);
        
        httpd_uri_t upload_uri = { .uri = "/upload", .method = HTTP_POST, .handler = upload_post_handler };
        httpd_register_uri_handler(server, &upload_uri);

        httpd_uri_t create_dir_uri = { .uri = "/create", .method = HTTP_POST, .handler = create_dir_handler };
        httpd_register_uri_handler(server, &create_dir_uri);

        httpd_uri_t list_uri = { .uri = "/listfiles*", .method = HTTP_GET, .handler = list_files_handler };
        httpd_register_uri_handler(server, &list_uri);
        
        httpd_uri_t delete_uri = { .uri = "/delete", .method = HTTP_POST, .handler = delete_file_handler };
        httpd_register_uri_handler(server, &delete_uri);

        httpd_uri_t save_uri = { .uri = "/save", .method = HTTP_POST, .handler = save_post_handler };
        httpd_register_uri_handler(server, &save_uri);
        
        ESP_LOGI(TAG, "Todos los handlers del servidor web registrados correctamente.");
        return server;
    }

    ESP_LOGE(TAG, "Error al iniciar el servidor web.");
    return NULL;
}

void web_server_stop(httpd_handle_t server) {
    if (server) {
        httpd_stop(server);
        ESP_LOGI(TAG, "Servidor web detenido.");
    }
}
