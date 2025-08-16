/*
 * Fichero: bsp_sdcard.h
 * Fecha: 08/08/2025
 * Último cambio: Añadida la declaración de bsp_sdcard_init.
 * Descripción: Interfaz pública para el controlador de la tarjeta SD.
 */
#ifndef __BSP_SDCARD_H_
#define __BSP_SDCARD_H_

#include "esp_err.h"
#include <stdint.h> // Para uint64_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el bus SPI y monta el sistema de ficheros FAT en la tarjeta SD.
 * 
 * @return esp_err_t 
 *         - ESP_OK si la inicialización y el montaje son exitosos.
 *         - ESP_FAIL u otros códigos de error si algo falla.
 */
esp_err_t bsp_sdcard_init(void);

/**
 * @brief Obtiene el tamaño total de la tarjeta SD en bytes.
 * 
 * @note Esta función debe ser llamada después de una inicialización exitosa.
 * @return uint64_t Tamaño de la tarjeta en bytes.
 */
uint64_t bsp_sdcard_get_size(void);


#ifdef __cplusplus
}
#endif

#endif // __BSP_SDCARD_H_