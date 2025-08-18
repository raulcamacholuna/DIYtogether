/* Fecha: 18/08/2025 - 07:41  */
/* Fichero: components/web_server/web_server_priv.h */
/* Último cambio: Creación del fichero como parte de la refactorización para declarar handlers y helpers internos. */
/* Descripción: Cabecera privada para el componente web_server. Declara las funciones de los handlers y helpers que son compartidas internamente entre los ficheros del componente, pero no expuestas públicamente. Esto permite a web_server.c registrar los handlers mientras que sus implementaciones residen en web_server_handlers.c. */

#ifndef WEB_SERVER_PRIV_H
#define WEB_SERVER_PRIV_H

#include "esp_http_server.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Constantes internas del componente ---
#define WEB_MOUNT_POINT "/sdcard"
#define UPLOAD_BUFFER_SIZE 2048

// --- Declaraciones de Handlers (implementados en web_server_handlers.c) ---
esp_err_t root_get_handler(httpd_req_t *req);
esp_err_t backup_get_handler(httpd_req_t *req);
esp_err_t list_files_handler(httpd_req_t *req);
esp_err_t upload_post_handler(httpd_req_t *req);
esp_err_t delete_file_handler(httpd_req_t *req);
esp_err_t create_dir_handler(httpd_req_t *req);
esp_err_t save_post_handler(httpd_req_t *req);

// --- Declaraciones de Helpers (implementados en web_server_helpers.c) ---
esp_err_t serve_file_from_sd(httpd_req_t *req, const char *filepath);
void url_decode(char *dst, const char *src);
bool get_multipart_value(const char* buf, const char* name, char* result, size_t max_len);
esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename);

#ifdef __cplusplus
}
#endif

#endif // WEB_SERVER_PRIV_H
