/* Fecha: 17/08/2025 - 01:49  */
/* Fichero: components/ui/actions/action_config_mode.h */
/* Último cambio: Creación del módulo para la acción del modo de configuración. */
/* Descripción: Interfaz pública para gestionar el ciclo de vida del modo de configuración. Expone funciones para iniciar y detener el modo, encapsulando su estado y la gestión de la tarea de red. */

#ifndef ACTION_CONFIG_MODE_H
#define ACTION_CONFIG_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia el modo de configuración WiFi.
 *        Oculta la UI principal, muestra la información de red,
 *        y lanza una tarea para gestionar la conexión y el servidor web.
 */
void action_config_mode_start(void);

/**
 * @brief Detiene el modo de configuración WiFi.
 *        Limpia los elementos de la UI de configuración, detiene la tarea
 *        de red y los servicios asociados, y restaura la UI principal.
 */
void action_config_mode_stop(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_CONFIG_MODE_H
