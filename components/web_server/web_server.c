/* Fecha: 18/08/2025 - 07:43  */
/* Fichero: components/web_server/web_server.c */
/* Último cambio: Aumentada la prioridad de la tarea del servidor HTTP a 3 para resolver el error de socket. */
/* Descripción: Orquestador del servidor web. La prioridad de la tarea se ha elevado a 3, que es mayor que la de la tarea de LVGL (prioridad 4). Esto es crucial para asegurar que el servidor web tenga la CPU necesaria para gestionar el envío de datos a través del socket, evitando que el buffer TCP se llene y cause el error EAGAIN ('error in send : 11'), que es la causa raíz del fallo en la transferencia de ficheros. */

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
    config.task_priority = 3; // [CORRECCIÓN] Prioridad > LVGL (4) para evitar inanición de la red.

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
