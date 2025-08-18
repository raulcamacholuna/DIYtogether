/* Fecha: 18/08/2025 - 06:48  */
/* Fichero: components/ui/core/state_manager.h */
/* Último cambio: Añadidas funciones para pausar y reanudar el gestor de estado. */
/* Descripción: Interfaz pública del gestor de estado. Se añaden 'state_manager_pause' y 'state_manager_resume' para permitir que otros módulos (como el modo de configuración) desactiven temporalmente la gestión de inactividad de la pantalla, evitando que la pantalla se apague durante operaciones como el uso del servidor web. */

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

#ifdef __cplusplus
}
#endif

#endif // STATE_MANAGER_H
