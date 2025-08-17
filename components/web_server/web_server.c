/* Fecha: 16/08/2025 - 09:55  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\components\web_server\web_server.c */
/* Último cambio: Añadido el manejador POST /create para la creación de directorios. */
/* Descripción: Servidor web para la configuración del dispositivo. Se ha añadido un nuevo manejador de endpoint para la ruta '/create' que responde a peticiones POST. Este manejador extrae una ruta y un nombre de directorio del cuerpo de la petición, construye la ruta completa en la tarjeta SD y utiliza 'mkdir' para crear el nuevo directorio. Esto soluciona un error 404 (Not Found) que ocurría al intentar crear carpetas desde la interfaz web de gestión de archivos. */

#include "web_server.h"
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
#include <ctype.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "web_server_page.h"

static const char *TAG = "WEB_SERVER";
#define WEB_MOUNT_POINT "/sdcard"
#define UPLOAD_BUFFER_SIZE 2048

// --- Helper para establecer el Content-Type basado en la extensión del fichero ---
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filename) {
    if (strstr(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (strstr(filename, ".css")) {
        return httpd_resp_set_type(req, "text/css");
    } else if (strstr(filename, ".js")) {
        return httpd_resp_set_type(req, "application/javascript");
    }
    return httpd_resp_set_type(req, "text/plain");
}

// --- Helper genérico para servir un fichero estático desde el VFS ---
static esp_err_t serve_file_from_sd(httpd_req_t *req, const char *filepath) {
    ESP_LOGI(TAG, "Intentando servir fichero: %s", filepath);
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Fallo al abrir el fichero para lectura: %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "El fichero no existe en la SD.");
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
                ESP_LOGE(TAG, "Fallo en el envío del fichero.");
                return ESP_FAIL;
            }
        }
    } while (chunksize != 0);

    fclose(f);
    ESP_LOGI(TAG, "Envío de fichero completado.");
    httpd_resp_send_chunk(req, NULL, 0); // Enviar chunk final
    return ESP_OK;
}

// --- Función de decodificación de URL ---
static void url_decode(char *dst, const char *src) {
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

// --- Parser manual simple para extraer valores de multipart/form-data ---
static bool get_multipart_value(const char* buf, const char* name, char* result, size_t max_len) {
    char field_name[128];
    snprintf(field_name, sizeof(field_name), "name=\"%s\"", name);
    
    char *val_start = strstr(buf, field_name);
    if (!val_start) return false;

    val_start = strstr(val_start, "\r\n\r\n");
    if (!val_start) return false;
    val_start += 4;

    char *val_end = strstr(val_start, "\r\n--");
    if (!val_end) return false;
    
    size_t len = val_end - val_start;
    if (len >= max_len) return false;

    strncpy(result, val_start, len);
    result[len] = '\0';
    return true;
}

// --- Manejador para listar archivos ---
static esp_err_t list_files_handler(httpd_req_t *req) {
    char query_buf[128];
    char path_decoded[128] = "/";
    char current_path[256] = "/";

    if (httpd_req_get_url_query_str(req, query_buf, sizeof(query_buf)) == ESP_OK) {
        char param[128];
        if (httpd_query_key_value(query_buf, "path", param, sizeof(param)) == ESP_OK) {
            url_decode(path_decoded, param);
        }
    }
    if (path_decoded[0] != '/') {
        snprintf(current_path, sizeof(current_path), "/%s", path_decoded);
    } else {
        strncpy(current_path, path_decoded, sizeof(current_path) - 1);
    }

    if (strstr(current_path, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid path");
        return ESP_FAIL;
    }

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", WEB_MOUNT_POINT, strcmp(current_path, "/") == 0 ? "" : current_path);
    
    ESP_LOGI(TAG, "Listando archivos en: %s", full_path);

    DIR *d = opendir(full_path);
    if (!d) {
        ESP_LOGE(TAG, "No se pudo abrir el directorio: %s", full_path);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    
    size_t buf_size = 1024;
    char *json_buf = malloc(buf_size);
    strcpy(json_buf, "[");
    bool first = true;
    struct dirent *dir;

    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
        if (!first) strcat(json_buf, ",");
        
        char item_path[512];
        if (snprintf(item_path, sizeof(item_path), "%s/%s", full_path, dir->d_name) >= sizeof(item_path)) {
            ESP_LOGE(TAG, "Path too long, skipping: %s/%s", full_path, dir->d_name);
            continue;
        }
        
        struct stat st;
        char entry_buf[512];
        if (stat(item_path, &st) == 0) {
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

// --- Manejador para subida de archivos ---
static esp_err_t upload_post_handler(httpd_req_t *req) {
    char buf[UPLOAD_BUFFER_SIZE];
    char filepath[256];
    FILE *fd = NULL;
    int received;
    int remaining = req->content_len;

    received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (received <= 0) return ESP_FAIL;
    
    char path[128] = "/";
    char filename[64] = "";
    get_multipart_value(buf, "path", path, sizeof(path));
    char *filename_start = strstr(buf, "filename=\"");
    if (!filename_start) return ESP_FAIL;
    filename_start += strlen("filename=\"");
    char *filename_end = strchr(filename_start, '\"');
    if (!filename_end) return ESP_FAIL;
    strncpy(filename, filename_start, filename_end - filename_start);
    filename[filename_end - filename_start] = '\0';

    if (strlen(filename) == 0 || strstr(path, "..") || strstr(filename, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Ruta/fichero inválido.");
        return ESP_FAIL;
    }

    snprintf(filepath, sizeof(filepath), "%s%s/%s", WEB_MOUNT_POINT, strcmp(path, "/") == 0 ? "" : path, filename);
    fd = fopen(filepath, "wb");
    if (!fd) { httpd_resp_send_500(req); return ESP_FAIL; }

    char *data_start = strstr(filename_end, "\r\n\r\n");
    if (!data_start) { fclose(fd); return ESP_FAIL; }
    
    data_start += 4;
    int header_len = data_start - buf;
    int data_len = received - header_len;
    fwrite(data_start, 1, data_len, fd);
    remaining -= received;

    while (remaining > 0) {
        received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (received <= 0) { fclose(fd); return ESP_FAIL; }
        
        char *boundary_start = strstr(buf, "\r\n--");
        if (boundary_start) {
             fwrite(buf, 1, boundary_start - buf, fd);
        } else {
             fwrite(buf, 1, received, fd);
        }
        remaining -= received;
    }

    fclose(fd);
    ESP_LOGI(TAG, "Subida de archivo a %s completa.", filepath);
    httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// --- Manejador para borrar archivos ---
static esp_err_t delete_file_handler(httpd_req_t *req) {
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char path[128] = "";
    char filename[64] = "";
    get_multipart_value(buf, "path", path, sizeof(path));
    get_multipart_value(buf, "filename", filename, sizeof(filename));
    
    if (strlen(filename) == 0 || strstr(path, "..") || strstr(filename, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Nombre de archivo o ruta inválidos.");
        return ESP_FAIL;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s%s/%s", WEB_MOUNT_POINT, strcmp(path, "/") == 0 ? "" : path, filename);

    if (unlink(filepath) == 0) {
        ESP_LOGI(TAG, "Archivo borrado: %s", filepath);
        httpd_resp_send(req, "Archivo borrado.", HTTPD_RESP_USE_STRLEN);
    } else {
        ESP_LOGE(TAG, "Fallo al borrar el archivo: %s", filepath);
        httpd_resp_send_500(req);
    }
    return ESP_OK;
}

// --- Manejador para crear un nuevo directorio ---
static esp_err_t create_dir_handler(httpd_req_t *req) {
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    char path[128] = "";
    char dirname[64] = "";

    // Asumimos que el formulario envía 'path' para el directorio padre y 'dirname' para el nuevo.
    get_multipart_value(buf, "path", path, sizeof(path));
    get_multipart_value(buf, "dirname", dirname, sizeof(dirname));
    
    if (strlen(dirname) == 0 || strstr(path, "..") || strstr(dirname, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Nombre de directorio o ruta inválidos.");
        return ESP_FAIL;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s/%s", WEB_MOUNT_POINT, strcmp(path, "/") == 0 ? "" : path, dirname);

    // Intentar crear el directorio
    if (mkdir(full_path, 0755) == 0) {
        ESP_LOGI(TAG, "Directorio creado: %s", full_path);
        httpd_resp_send(req, "Directorio creado con éxito.", HTTPD_RESP_USE_STRLEN);
    } else {
        ESP_LOGE(TAG, "Fallo al crear el directorio: %s", full_path);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo crear el directorio. Puede que ya exista.");
    }
    return ESP_OK;
}


// --- Manejador para guardar credenciales WiFi ---
static esp_err_t save_post_handler(httpd_req_t *req) {
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) httpd_resp_send_408(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    char ssid[32] = {0}, password_encoded[64] = {0}, password_decoded[64] = {0}, authmode_str[4] = {0};
    
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password_encoded, sizeof(password_encoded));
    httpd_query_key_value(buf, "authmode", authmode_str, sizeof(authmode_str));
    int32_t authmode = atoi(authmode_str);

    url_decode(password_decoded, password_encoded);
    
    ESP_LOGI(TAG, "Recibido para guardar - SSID: [%s], Authmode: [%ld]", ssid, authmode);

    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_pass", password_decoded));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, "wifi_authmode", authmode));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales guardadas en NVS.");

    const char* resp_str = "<h1>Configuracion guardada!</h1><p>El dispositivo se reiniciara.</p>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    
    return ESP_OK;
}

// --- Manejador para servir la página principal desde la SD ---
static esp_err_t root_get_handler(httpd_req_t *req) {
    return serve_file_from_sd(req, "/sdcard/config/Index.html");
}

// --- Manejador para servir la página de backup desde la SD ---
static esp_err_t backup_get_handler(httpd_req_t *req) {
    return serve_file_from_sd(req, "/sdcard/config/backup.html");
}

static httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 8192;

    if (httpd_start(&server, &config) == ESP_OK) {
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
    }
    return server;
}

void web_server_start(void) {
    ESP_LOGI(TAG, "Iniciando servidor web de configuracion.");
    start_webserver();
}
