/*
  Fichero: ./components/service_screen/service_screen.h
  Fecha: 23/08/2025 - 13:00
  Último cambio: Sincronización con la implementación en C.
  Descripción: Interfaz pública para el gestor de pantallas de servicio.
*/
#ifndef SERVICE_SCREEN_H
#define SERVICE_SCREEN_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Muestra una imagen a pantalla completa desde un asset compilado en el firmware.
 *
 * @return ESP_OK si la imagen se muestra correctamente.
 */
esp_err_t service_screen_show_from_rom(void);

/**
 * @brief (Obsoleta) Muestra una imagen a pantalla completa desde la tarjeta SD.
 *
 * @param image_path Ruta completa al fichero .bin de la imagen en la SD.
 * @return ESP_ERR_NOT_SUPPORTED
 */
esp_err_t service_screen_show_from_sd(const char* image_path);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_SCREEN_H
