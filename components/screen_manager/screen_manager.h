/* Fichero: components/screen_manager/screen_manager.h */
/* Descripción: Diagnóstico: La lógica de apagado y sueño ligero estaba acoplada. Solución Holística: Se ha refactorizado la interfaz para desacoplar las operaciones. 'turn_off' ahora solo apaga el backlight, mientras que la nueva función 'enter_light_sleep' gestiona explícitamente la entrada en bajo consumo. Esta granularidad permite que el 'state_manager' implemente una lógica de inactividad por fases (apagar y luego dormir), mientras que las acciones directas del usuario pueden invocar el sueño inmediato, resultando en un sistema más modular y predecible. */
/* Último cambio: 21/08/2025 - 21:37 */
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
void screen_manager_enter_light_sleep(void);
void screen_manager_set_brightness(int percentage, bool save_to_nvs);
bool screen_manager_is_off(void);

#ifdef __cplusplus
}
#endif

#endif // SCREEN_MANAGER_H
