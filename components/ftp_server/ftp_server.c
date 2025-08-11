/*
  Fichero: ./components/ftp_server/ftp_server.c
  Fecha: 12/08/2025 - 03:30
  Último cambio: Convertido a modo sin pantalla (headless).
  Descripción: Implementa un modo de operación para el servidor FTP.
               Inicia el servicio y entra en un bucle infinito, ya que no
               hay interfaz de usuario para salir de este modo.
*/
#include "ftp_server.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "esp_ftp_server.h" // Incluir aquí la librería real

static const char *TAG = "FTP_SERVER";

void ftp_server_start(void) {
    ESP_LOGI(TAG, "Iniciando modo de servidor FTP (headless).");
    
    // Aquí iría la llamada a la función de inicialización de la librería FTP
    // Ejemplo: esp_ftp_server_start("/sdcard");
    ESP_LOGW(TAG, "La logica del servidor FTP no esta implementada, solo el flujo de la aplicacion.");

    // Bucle infinito para mantener el modo de servicio activo
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}