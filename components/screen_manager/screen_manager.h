/* Fichero: components/screen_manager/screen_manager.h */
/* Descripción: Se elimina la función 'enter_light_sleep'. La lógica de 'sueño' ya no se basa en el light sleep de ESP-IDF, sino en un estado gestionado por la UI que detiene las animaciones y apaga el backlight sin detener el manejador de eventos de LVGL, resolviendo el problema de la falta de respuesta al despertar. */
/* Último cambio: 22/2025 - 08:07
*/
#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t screen_manager_init(void);
void screen_manager_turn_on(void);
void screen_manager_turn_off(void);
void screen_manager_set_brightness(int percentage, bool save_to_nvs);
bool screen_manager_is_off(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_MANAGER_H
