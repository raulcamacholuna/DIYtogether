/* Fecha: 17/08/2025 - 02:29  */
/* Fichero: components/ui/core/state_manager.h */
/* Último cambio: Movido a ui/core/ y renombrado de ui_state_manager.h como parte de la refactorización. */
/* Descripción: Interfaz pública del gestor de estado de la UI. Responsable de manejar la inactividad, atenuado/apagado de pantalla y lógica de despertar. */

#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el gestor de estado de la UI.
 * 
 * Configura los temporizadores y callbacks necesarios para gestionar la
 * inactividad, el atenuado/apagado de la pantalla y la lógica de despertar.
 * Debe ser llamado una vez después de que la UI principal haya sido creada.
 */
void state_manager_init(void);

#ifdef __cplusplus
}
#endif

#endif // STATE_MANAGER_H
