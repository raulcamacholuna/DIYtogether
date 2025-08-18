/* Fecha: 18/08/2025 - 08:00  */
/* Fichero: components/ui/core/state_manager.h */
/* Último cambio: Añadida la función state_manager_destroy para liberar recursos. */
/* Descripción: Interfaz pública del gestor de estado. Se añade una función 'destroy' para eliminar los temporizadores de LVGL y liberar memoria cuando el gestor no es necesario, como en el modo de configuración. Esto optimiza el uso de recursos y previene la ejecución de callbacks innecesarios. */

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

/**
 * @brief Pausa el gestor de estado, deteniendo los temporizadores de inactividad.
 */
void state_manager_pause(void);

/**
 * @brief Reanuda el gestor de estado, reactivando los temporizadores de inactividad.
 */
void state_manager_resume(void);

/**
 * @brief Destruye el gestor de estado y libera sus recursos.
 *
 * Elimina todos los temporizadores de LVGL creados por el gestor,
 * liberando memoria y deteniendo su lógica por completo.
 */
void state_manager_destroy(void);


#ifdef __cplusplus
}
#endif

#endif // STATE_MANAGER_H
