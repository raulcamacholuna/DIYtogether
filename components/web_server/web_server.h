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