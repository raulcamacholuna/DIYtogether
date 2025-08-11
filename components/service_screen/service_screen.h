/*
  Fichero: ./components/service_screen/service_screen.h
  Fecha: 12/08/2025 - 07:00
  Último cambio: Creación del componente.
  Descripción: Interfaz pública para el gestor de pantallas de servicio.
*/
#ifndef SERVICE_SCREEN_H
#define SERVICE_SCREEN_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Muestra una imagen a pantalla completa desde la tarjeta SD.
 *
 * Esta función realiza una inicialización mínima de hardware (display y LVGL)
 * para mostrar una única imagen binaria. No inicia la UI principal ni el gestor de hardware.
 *
 * @param image_path Ruta completa al fichero .bin de la imagen en la SD.
 * @return ESP_OK si la imagen se muestra correctamente.
 */
esp_err_t service_screen_show(const char* image_path);

#ifdef __cplusplus
}
#endif

#endif // SERVICE_SCREEN_H