/* Fecha: 18/08/2025 - 09:23  */
/* Fichero: components/ui/actions/action_system.h */
/* Último cambio: Eliminada la declaración de action_system_enable_file_server. */
/* Descripción: Interfaz pública para las acciones de sistema. Se ha eliminado la acción del servidor de ficheros, ya que ahora está integrada en el modo de configuración principal, dejando solo la acción de reseteo total. */

#ifndef ACTION_SYSTEM_H
#define ACTION_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Realiza un reseteo total del dispositivo.
 *        Borra las credenciales WiFi y el estado de evolución de la NVS,
 *        y luego reinicia.
 */
void action_system_reset_all(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_SYSTEM_H
