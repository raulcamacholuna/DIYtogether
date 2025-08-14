/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\web_server\web_server.c
# Fecha: `$timestamp
# Último cambio: Corregido el fichero para que sea sintácticamente válido en C.
# Descripción: Servidor web para configuración. Implementa los endpoints /listfiles, /upload, y /delete para interactuar con la tarjeta SD y dar soporte a la interfaz web. La página principal se sirve desde web_server_page.h.
*/
#include "web_server.h"
#include "web_server_page.h" // Incluir el HTML embebido
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "bsp_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "WEB_SERVER";
#define WEB_MOUNT_POINT "/sdcard"
#define FILE_UPLOAD_BUFFER_SIZE 2048

// Estructura para gestionar el contexto de subida de archivos grandes
struct file_upload_ctx {
    FILE *fd;
    char filepath[256];
};

// Manejador para la subida de archivos (POST /upload)
static esp_err_t upload_post_handler(httpd_req_t *req) {
    char buf[FILE_UPLOAD_BUFFER_SIZE];
    int received;
    struct file_upload_ctx *ctx = (struct file_upload_ctx *)req->user_ctx;

    // Si es el primer chunk, inicializar el contexto
    if (ctx == NULL) {
        ctx = malloc(sizeof(struct file_upload_ctx));
        if (!ctx) return ESP_ERR_NO_MEM;
        req->user_ctx = ctx;

        char header_val[128];
        if (httpd_req_get_hdr_value_str(req, "Content-Disposition", header_val, sizeof(header_val)) == ESP_OK) {
            char *filename_ptr = strstr(header_val, "filename=\"");
            if (filename_ptr) {
                filename_ptr += strlen("filename=\"");
                char *end_ptr = strchr(filename_ptr, '\"');
                if (end_ptr) {
                    *end_ptr = '\0';
                    snprintf(ctx->filepath, sizeof(ctx->filepath), "%s/%s", WEB_MOUNT_POINT, filename_ptr);
                    ESP_LOGI(TAG, "Iniciando subida de archivo: %s", ctx->filepath);
                    ctx->fd = fopen(ctx->filepath, "wb");
                    if (!ctx->fd) {
                        ESP_LOGE(TAG, "No se pudo abrir el archivo para escritura");
                        httpd_resp_send_500(req);
                        return ESP_FAIL;
                    }
                }
            }
        } else {
             ESP_LOGE(TAG, "Falta la cabecera 'Content-Disposition'");
             httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Falta cabecera 'Content-Disposition'.");
             return ESP_FAIL;
        }
    }

    // Leer un chunk del cuerpo de la petición
    received = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf)));
    if (received <= 0) {
        if (received == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    // Escribir el chunk en el archivo
    if (fwrite(buf, 1, received, ctx->fd) != received) {
        ESP_LOGE(TAG, "Error al escribir en el archivo");
        fclose(ctx->fd);
        unlink(ctx->filepath);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Si es el último chunk, cerrar el archivo y limpiar el contexto
    if (received < sizeof(buf)) {
        fclose(ctx->fd);
        ESP_LOGI(TAG, "Archivo subido correctamente a %s", ctx->filepath);
        httpd_resp_send(req, "Archivo subido con exito!", HTTPD_RESP_USE_STRLEN);
        free(ctx);
        req->user_ctx = NULL;
    }

    return ESP_OK;
}

// Manejador para listar archivos (GET /listfiles)
static esp_err_t list_files_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Petición recibida para listar archivos en %s", WEB_MOUNT_POINT);

    DIR *d = opendir(WEB_MOUNT_POINT);
    if (!d) {
        ESP_LOGE(TAG, "No se pudo abrir el directorio: %s", WEB_MOUNT_POINT);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    
    // Usar un buffer dinámico para construir la respuesta JSON
    size_t buf_size = 1024;
    char *json_buf = malloc(buf_size);
    if (!json_buf) { closedir(d); httpd_resp_send_500(req); return ESP_ERR_NO_MEM; }
    
    strcpy(json_buf, "[");
    bool first = true;
    struct dirent *dir;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        
        if (!first) strcat(json_buf, ",");
        
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", WEB_MOUNT_POINT, dir->d_name);
        
        struct stat st;
        char entry_buf[512];
        if (stat(path, &st) == 0) {
            snprintf(entry_buf, sizeof(entry_buf), 
                     "{\"name\":\"%s\",\"size\":%lld,\"type\":\"%s\"}", 
                     dir->d_name, (long long)st.st_size, S_ISDIR(st.st_mode) ? "dir" : "file");
        } else {
            snprintf(entry_buf, sizeof(entry_buf), 
                     "{\"name\":\"%s\",\"size\":0,\"type\":\"file\"}", dir->d_name);
        }

        if (strlen(json_buf) + strlen(entry_buf) + 2 > buf_size) {
            buf_size *= 2;
            char *new_buf = realloc(json_buf, buf_size);
            if (!new_buf) { free(json_buf); closedir(d); httpd_resp_send_500(req); return ESP_ERR_NO_MEM; }
            json_buf = new_buf;
        }

        strcat(json_buf, entry_buf);
        first = false;
    }
    closedir(d);

    strcat(json_buf, "]");
    httpd_resp_send(req, json_buf, strlen(json_buf));
    free(json_buf);

    return ESP_OK;
}

// Manejador para borrar archivos (POST /delete)
static esp_err_t delete_file_handler(httpd_req_t *req) {
    char buf[192];
    char filename[64] = {0};
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);

    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    // Parser simplificado para multipart/form-data que extrae el nombre del archivo
    char *ptr = strstr(buf, "name=\"filename\"");
    if (ptr) {
        ptr = strstr(ptr, "\r\n\r\n");
        if (ptr) {
            ptr += 4;
            char *end_ptr = strstr(ptr, "\r\n--");
            if (end_ptr) {
                size_t len = end_ptr - ptr;
                if (len < sizeof(filename)) {
                    strncpy(filename, ptr, len);
                    filename[len] = '\0';
                }
            }
        }
    }

    if (strlen(filename) == 0) {
        ESP_LOGE(TAG, "No se pudo extraer el nombre del archivo de la petición POST.");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Nombre de archivo no encontrado.");
        return ESP_FAIL;
    }
    
    if (strstr(filename, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Nombre de archivo inválido.");
        return ESP_FAIL;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", WEB_MOUNT_POINT, filename);

    if (unlink(filepath) == 0) {
        ESP_LOGI(TAG, "Archivo borrado: %s", filepath);
        httpd_resp_send(req, "Archivo borrado.", HTTPD_RESP_USE_STRLEN);
    } else {
        ESP_LOGE(TAG, "Fallo al borrar el archivo: %s", filepath);
        httpd_resp_send_500(req);
    }

    return ESP_OK;
}

// Manejador genérico para servir archivos estáticos (GET /*)
static esp_err_t file_get_handler(httpd_req_t *req) {
    const char *uri = req->uri;
    
    // Si la URI es la raíz o index.html, servir el contenido embebido
    if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0) {
        ESP_LOGI(TAG, "Sirviendo página principal embebida.");
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, INDEX_HTML_CONTENT, HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    // Para cualquier otra URI, intentar servir desde la SD (para futuros CSS/JS, etc.)
    char filepath[256];
    const char *base_path = WEB_MOUNT_POINT "/config";

    if ((strlen(base_path) + strlen(uri) + 1) > sizeof(filepath)) {
        httpd_resp_send_err(req, HTTPD_414_URI_TOO_LONG, "URI is too long");
        return ESP_FAIL;
    }

    strcpy(filepath, base_path);
    strcat(filepath, uri);

    ESP_LOGI(TAG, "Intentando servir archivo estático: %s", filepath);

    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "No se pudo encontrar el archivo: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    if (strstr(filepath, ".css")) httpd_resp_set_type(req, "text/css");
    else if (strstr(filepath, ".js")) httpd_resp_set_type(req, "application/javascript");
    else httpd_resp_set_type(req, "text/html");

    char chunk[512];
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, sizeof(chunk), f);
        if (chunksize > 0 && httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
            fclose(f);
            ESP_LOGE(TAG, "Error al enviar chunk de archivo");
            return ESP_FAIL;
        }
    } while (chunksize != 0);

    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 10;

    ESP_LOGI(TAG, "Iniciando servidor httpd");
    if (httpd_start(&server, &config) == ESP_OK) {
        
        httpd_uri_t upload_uri = { .uri = "/upload", .method = HTTP_POST, .handler = upload_post_handler, .user_ctx  = NULL };
        httpd_register_uri_handler(server, &upload_uri);
        
        // Registrar los nuevos endpoints ANTES del comodín
        httpd_uri_t list_uri = { .uri = "/listfiles", .method = HTTP_GET, .handler = list_files_handler, .user_ctx = NULL };
        httpd_register_uri_handler(server, &list_uri);
        
        httpd_uri_t delete_uri = { .uri = "/delete", .method = HTTP_POST, .handler = delete_file_handler, .user_ctx = NULL };
        httpd_register_uri_handler(server, &delete_uri);
        
        // El comodín debe ser el ÚLTIMO en registrarse
        httpd_uri_t file_get_uri = { .uri = "/*", .method = HTTP_GET, .handler = file_get_handler, .user_ctx = NULL };
        httpd_register_uri_handler(server, &file_get_uri);
    }
    return server;
}

void web_server_start(void) {
    ESP_LOGI(TAG, "Iniciando servidor web de configuracion.");
    start_webserver();
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
