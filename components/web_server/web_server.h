/*
# Fichero: Z:\DIYTOGETHER\DIYtogether\components\web_server\web_server.h
# Fecha: `$timestamp
# Último cambio: Corregido el fichero para que sea sintácticamente válido en C.
# Descripción: Interfaz pública para el componente del servidor web, que expone la función de arranque del servicio.
*/
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia el modo de operación del servidor web.
 * @note  Esta es una función de bloqueo que no retorna.
 */
void web_server_start(void);

#ifdef __cplusplus
}
#endif

#endif // WEB_SERVER_H
