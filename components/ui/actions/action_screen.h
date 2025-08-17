/* Fecha: 17/08/2025 - 01:48  */
/* Fichero: components/ui/actions/action_screen.h */
/* Último cambio: Creación del módulo para la acción de encender/apagar la pantalla. */
/* Descripción: Interfaz pública para la acción de control de pantalla. Encapsula la lógica de encendido y apagado, continuando la refactorización para desacoplar el orquestador principal. */

#ifndef ACTION_SCREEN_H
#define ACTION_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Alterna el estado de la pantalla (encendido/apagado).
 *        Si está encendida, oculta los paneles y la apaga.
 *        Si está apagada, la enciende.
 */
void action_screen_toggle(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_SCREEN_H
