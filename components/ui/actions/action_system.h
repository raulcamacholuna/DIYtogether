/* Fecha: 17/08/2025 - 01:50  */
/* Fichero: components/ui/actions/action_system.h */
/* Último cambio: Creación del módulo para agrupar las acciones de sistema. */
/* Descripción: Interfaz pública para las acciones de sistema como 'activar servidor de ficheros' y 'reset total'. Continúa la modularización de la lógica de acciones. */

#ifndef ACTION_SYSTEM_H
#define ACTION_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Activa el modo de servidor de ficheros.
 *        Establece una bandera en NVS y reinicia el dispositivo.
 */
void action_system_enable_file_server(void);

/**
 * @brief Realiza un reseteo total del dispositivo.
 *        Borra las credenciales WiFi, el estado de evolución y la bandera
 *        del servidor de ficheros, y luego reinicia.
 */
void action_system_reset_all(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_SYSTEM_H
