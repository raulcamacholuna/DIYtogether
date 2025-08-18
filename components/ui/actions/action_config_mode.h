/* Fecha: 18/08/2025 - 09:21  */
/* Fichero: components/ui/actions/action_config_mode.h */
/* Último cambio: Eliminada la declaración de action_config_mode_stop al cambiar la lógica de salida a un reinicio. */
/* Descripción: Interfaz pública para el modo de configuración. La función de detención se ha eliminado ya que la única salida del modo de configuración es ahora un reinicio del dispositivo, simplificando la API. */

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

#ifdef __cplusplus
}
#endif

#endif // ACTION_CONFIG_MODE_H
