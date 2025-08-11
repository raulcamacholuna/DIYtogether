/*
  Fichero: ./components/ftp_server/ftp_server.h
  Fecha: 12/08/2025 - 03:30
  Último cambio: Simplificado para el modo sin pantalla.
  Descripción: Interfaz pública para el componente del servidor FTP.
*/
#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicia el modo de operación del servidor FTP.
 * @note  Esta es una función de bloqueo que no retorna.
 */
void ftp_server_start(void);

#ifdef __cplusplus
}
#endif

#endif // FTP_SERVER_H