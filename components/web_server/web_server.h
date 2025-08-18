/* Fecha: 18/08/2025 - 07:05  */
/* Fichero: components/web_server/web_server.h */
/* Último cambio: Modificadas las funciones start/stop para gestionar el handle del servidor y evitar errores de socket. */
/* Descripción: Interfaz pública para el componente del servidor web. La función 'web_server_start' ahora devuelve un handle al servidor iniciado. Se ha añadido 'web_server_stop' para permitir un apagado controlado del servidor, solucionando los errores de socket ('error in send : 11') que ocurrían al salir del modo de configuración. */
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_http_server.h" // Necesario para el tipo httpd_handle_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia el servidor web y devuelve su handle.
 * @return El handle del servidor HTTPD si se inicia correctamente, NULL en caso de error.
 */
httpd_handle_t web_server_start(void);

/**
 * @brief Detiene el servidor web de forma controlada.
 * @param server El handle del servidor a detener, obtenido de web_server_start.
 */
void web_server_stop(httpd_handle_t server);

#ifdef __cplusplus
}
#endif

#endif // WEB_SERVER_H
