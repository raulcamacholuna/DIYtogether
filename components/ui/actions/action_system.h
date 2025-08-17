/* Fecha: 17/08/2025 - 03:35  */
/* Fichero: components/ui/actions/action_system.h */
/* Último cambio: Creación del módulo para agrupar las acciones de sistema como primer paso de la refactorización de 'actions.c'. */
/* Descripción: Interfaz pública para las acciones de sistema como 'activar servidor de ficheros' y 'reset total'. Este es el primer paso para descomponer el monolítico 'actions.c', siguiendo el Principio de Responsabilidad Única. */

#ifndef ACTION_SYSTEM_H
#define ACTION_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Activa el modo de servidor de ficheros.
 *        Establece una bandera en NVS y reinicia el dispositivo para que
 *        app_main entre en el modo de servicio correspondiente.
 */
void action_system_enable_file_server(void);

/**
 * @brief Realiza un reseteo total del dispositivo.
 *        Borra las credenciales WiFi, el estado de evolución y la bandera
 *        del servidor de ficheros de la NVS, y luego reinicia.
 */
void action_system_reset_all(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_SYSTEM_H
