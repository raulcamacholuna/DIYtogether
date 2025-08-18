/* Fecha: 18/08/2025 - 07:48  */
/* Fichero: components/web_server/web_server_handlers.c */
/* Último cambio: Eliminado el calificador 'static' de las definiciones de los handlers para resolver un error de enlazado (linkage error). */
/* Descripción: Se ha eliminado la palabra clave 'static' de la definición de todas las funciones handler. El error de compilación 'static declaration of '...' follows non-static declaration' ocurría porque las funciones se declaraban sin 'static' en la cabecera privada (web_server_priv.h) pero se definían con 'static' en este fichero, creando un conflicto de enlazado. Ahora las definiciones coinciden con las declaraciones, permitiendo que web_server.c las enlace correctamente. */

#include "web_server_priv.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

static const char *TAG = "WEB_HANDLERS";

// --- Implementación de Handlers de Endpoints ---

esp_err_t root_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: GET /. Sirviendo página principal.");
    return serve_file_from_sd(req, "/sdcard/config/Index.html");
}

esp_err_t backup_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: GET /backup. Sirviendo página de respaldo.");
    return serve_file_from_sd(req, "/sdcard/config/backup.html");
}

esp_err_t list_files_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: GET /listfiles. Listando contenido de directorio.");
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
        ESP_LOGE(TAG, "Intento de path traversal detectado: %s", current_path);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid path");
        return ESP_FAIL;
    }

    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s%s", WEB_MOUNT_POINT, strcmp(current_path, "/") == 0 ? "" : current_path);
    ESP_LOGD(TAG, "Listando directorio VFS: %s", full_path);
    
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
        int written = snprintf(item_path, sizeof(item_path), "%s/%s", full_path, dir->d_name);
        if (written < 0 || written >= sizeof(item_path)) {
            ESP_LOGE(TAG, "Error de truncamiento de ruta para: %s/%s", full_path, dir->d_name);
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
    ESP_LOGD(TAG, "JSON de ficheros generado: %s", json_buf);
    httpd_resp_send(req, json_buf, strlen(json_buf));
    free(json_buf);
    return ESP_OK;
}

esp_err_t upload_post_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: POST /upload. Iniciando subida de fichero.");
    char buf[UPLOAD_BUFFER_SIZE];
    char filepath[256];
    FILE *fd = NULL;
    int received;
    int remaining = req->content_len;
    ESP_LOGD(TAG, "Tamaño total del contenido: %d bytes.", remaining);

    received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
    if (received <= 0) {
        ESP_LOGE(TAG, "Fallo al recibir el primer chunk de datos.");
        return ESP_FAIL;
    }
    
    char path[128] = "/";
    char filename[64] = "";
    get_multipart_value(buf, "path", path, sizeof(path));
    char *filename_start = strstr(buf, "filename=\"");
    if (!filename_start) { ESP_LOGE(TAG, "No se encontró 'filename' en la cabecera multipart."); return ESP_FAIL; }
    filename_start += strlen("filename=\"");
    char *filename_end = strchr(filename_start, '\"');
    if (!filename_end) { ESP_LOGE(TAG, "Formato de 'filename' inválido."); return ESP_FAIL; }
    strncpy(filename, filename_start, filename_end - filename_start);
    filename[filename_end - filename_start] = '\0';
    ESP_LOGD(TAG, "Path extraído: '%s', Filename: '%s'", path, filename);

    if (strlen(filename) == 0 || strstr(path, "..") || strstr(filename, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Ruta/fichero inválido.");
        return ESP_FAIL;
    }

    snprintf(filepath, sizeof(filepath), "%s%s/%s", WEB_MOUNT_POINT, strcmp(path, "/") == 0 ? "" : path, filename);
    ESP_LOGI(TAG, "Abriendo fichero para escritura: %s", filepath);
    fd = fopen(filepath, "wb");
    if (!fd) { ESP_LOGE(TAG, "Fallo al abrir fichero."); httpd_resp_send_500(req); return ESP_FAIL; }

    char *data_start = strstr(filename_end, "\r\n\r\n");
    if (!data_start) { ESP_LOGE(TAG, "No se encontró el delimitador de datos."); fclose(fd); return ESP_FAIL; }
    
    data_start += 4;
    int header_len = data_start - buf;
    int data_len = received - header_len;
    fwrite(data_start, 1, data_len, fd);
    remaining -= received;

    while (remaining > 0) {
        received = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)));
        if (received <= 0) { ESP_LOGE(TAG, "Error durante la recepción del stream."); fclose(fd); return ESP_FAIL; }
        
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

esp_err_t delete_file_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: POST /delete. Petición de borrado recibida.");
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char path[128] = "";
    char filename[64] = "";
    get_multipart_value(buf, "path", path, sizeof(path));
    get_multipart_value(buf, "filename", filename, sizeof(filename));
    ESP_LOGD(TAG, "Petición de borrado: path='%s', filename='%s'", path, filename);
    
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

esp_err_t create_dir_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: POST /create. Petición de creación de directorio.");
    char buf[512];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char path[128] = "";
    char dirname[64] = "";
    get_multipart_value(buf, "path", path, sizeof(path));
    get_multipart_value(buf, "dirname", dirname, sizeof(dirname));
    ESP_LOGD(TAG, "Petición de creación de dir: path='%s', dirname='%s'", path, dirname);
    
    if (strlen(dirname) == 0 || strstr(path, "..") || strstr(dirname, "..")) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Nombre de directorio o ruta inválidos.");
        return ESP_FAIL;
    }

    char full_path[256];
    snprintf(full_path, sizeof(full_path), "%s%s/%s", WEB_MOUNT_POINT, strcmp(path, "/") == 0 ? "" : path, dirname);

    if (mkdir(full_path, 0755) == 0) {
        ESP_LOGI(TAG, "Directorio creado: %s", full_path);
        httpd_resp_send(req, "Directorio creado.", HTTPD_RESP_USE_STRLEN);
    } else {
        ESP_LOGE(TAG, "Fallo al crear directorio: %s", full_path);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No se pudo crear el directorio.");
    }
    return ESP_OK;
}

esp_err_t save_post_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Handler: POST /save. Guardando configuración WiFi.");
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    char ssid[32] = {0}, password_encoded[64] = {0}, password_decoded[64] = {0}, authmode_str[4] = {0};
    
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password_encoded, sizeof(password_encoded));
    httpd_query_key_value(buf, "authmode", authmode_str, sizeof(authmode_str));
    int32_t authmode = atoi(authmode_str);

    url_decode(password_decoded, password_encoded);
    ESP_LOGI(TAG, "Guardando SSID: '%s'", ssid); // No loguear la contraseña por seguridad
    
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "wifi_pass", password_decoded));
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, "wifi_authmode", authmode));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Credenciales guardadas en NVS. Dispositivo se reiniciará.");

    const char* resp_str = "<h1>Configuracion guardada!</h1><p>El dispositivo se reiniciara.</p>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    
    return ESP_OK;
}
