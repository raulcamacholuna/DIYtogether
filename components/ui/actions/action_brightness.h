/* Fecha: 17/08/2025 - 01:47  */
/* Fichero: components/ui/actions/action_brightness.h */
/* Último cambio: Creación del módulo de acción de brillo como primer paso de la refactorización. */
/* Descripción: Interfaz pública para la acción de ciclar el brillo. Encapsula la lógica de control de brillo, separándola del orquestador principal de acciones. */

#ifndef ACTION_BRIGHTNESS_H
#define ACTION_BRIGHTNESS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cícla a través de los niveles de brillo predefinidos y aplica el siguiente.
 *        Esta función es llamada por el orquestador de acciones principal.
 */
void action_brightness_cycle(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_BRIGHTNESS_H
