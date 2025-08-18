/* Fecha: 18/08/2025 - 07:43  */
/* Fichero: components/web_server/web_server_helpers.c */
/* Último cambio: Creado como parte de la refactorización para aislar las funciones de ayuda. */
/* Descripción: Este fichero contiene funciones de utilidad utilizadas por los handlers del servidor web. Incluye la lógica para servir ficheros estáticos desde la tarjeta SD, decodificar URLs, parsear datos de formularios multipart y determinar el tipo de contenido de un fichero. Separar estas funciones mejora la legibilidad y permite reutilizarlas fácilmente. */

#include "web_server_priv.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static const char *TAG = "WEB_HELPERS";

// --- Implementación de Funciones de Ayuda ---

esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename) {
    if (strstr(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (strstr(filename, ".css")) {
        return httpd_resp_set_type(req, "text/css");
    } else if (strstr(filename, ".js")) {
        return httpd_resp_set_type(req, "application/javascript");
    }
    // Añadir más tipos MIME si es necesario (ej: .json, .png, etc.)
    return httpd_resp_set_type(req, "text/plain");
}

esp_err_t serve_file_from_sd(httpd_req_t *req, const char *filepath) {
    ESP_LOGI(TAG, "Helper: Sirviendo fichero estático: %s", filepath);
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Helper: Fallo al abrir el fichero: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Fichero no encontrado.");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char chunk[1024];
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, sizeof(chunk), f);
        if (chunksize > 0) {
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(f);
                ESP_LOGE(TAG, "Helper: Fallo al enviar chunk del fichero.");
                // No se puede enviar un error aquí porque la respuesta ya ha comenzado.
                return ESP_FAIL;
            }
        }
    } while (chunksize != 0);

    fclose(f);
    ESP_LOGI(TAG, "Helper: Envío de fichero completado.");
    httpd_resp_send_chunk(req, NULL, 0); // Señal de finalización
    return ESP_OK;
}

void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10); else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10); else b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

bool get_multipart_value(const char* buf, const char* name, char* result, size_t max_len) {
    char field_name[128];
    snprintf(field_name, sizeof(field_name), "name=\"%s\"", name);
    
    char *val_start = strstr(buf, field_name);
    if (!val_start) return false;

    // Avanzar hasta el final de la línea del content-disposition
    val_start = strstr(val_start, "\r\n\r\n");
    if (!val_start) return false;
    val_start += 4; // Saltar el doble CRLF

    // El final del valor es el siguiente boundary
    char *val_end = strstr(val_start, "\r\n--");
    if (!val_end) return false;
    
    size_t len = val_end - val_start;
    if (len >= max_len) {
        ESP_LOGW(TAG, "Valor multipart para '%s' es demasiado largo (%d vs max %d)", name, len, max_len);
        return false;
    }

    strncpy(result, val_start, len);
    result[len] = '\0';
    return true;
}
