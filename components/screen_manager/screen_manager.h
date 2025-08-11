#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el gestor de pantalla.
 * 
 * @return ESP_OK si la inicialización es correcta.
 */
esp_err_t screen_manager_init(void);

/**
 * @brief Enciende la pantalla y restaura el último nivel de brillo.
 */
void screen_manager_turn_on(void);

/**
 * @brief Apaga la pantalla y el backlight.
 */
void screen_manager_turn_off(void);

/**
 * @brief Establece el nivel de brillo de la pantalla.
 * 
 * @param percentage Brillo en porcentaje (0-100).
 */
void screen_manager_set_brightness(int percentage);

/**
 * @brief Devuelve si la pantalla está actualmente apagada.
 * 
 * @return true si la pantalla está apagada, false en caso contrario.
 */
bool screen_manager_is_off(void);


#ifdef __cplusplus
}
#endif

#endif // SCREEN_MANAGER_H