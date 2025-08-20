/* Fichero: components/bsp/bsp_priv.h */
/* Último cambio: Creado para declarar los prototipos de las funciones de inicialización internas y específicas de cada placa. */
/* Descripción: Cabecera privada del componente BSP. Define la interfaz interna que deben implementar los ficheros de cada placa (ej: WS1.9TS/bsp_display.c). El fichero bsp.c (orquestador) utiliza estos prototipos para llamar a la implementación correcta, que es seleccionada en tiempo de compilación por PlatformIO. Esto desacopla la lógica de orquestación de la implementación de hardware específica. */
/* Último cambio: 20/08/2025 - 04:57 */
#ifndef BSP_PRIV_H
#define BSP_PRIV_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Prototipos de funciones de inicialización de bajo nivel ---
// Estas funciones deben ser implementadas por los ficheros C
// dentro de cada directorio de placa específico (WS1.9TS, WS1.47TS).

esp_err_t bsp_i2c_init_internal(void);
esp_err_t bsp_spi_init_internal(void);
esp_err_t bsp_display_init_internal(void);
esp_err_t bsp_touch_init_internal(void);
esp_err_t bsp_sdcard_init_internal(void);
esp_err_t bsp_imu_init_internal(void);
esp_err_t bsp_battery_init_internal(void);

#ifdef __cplusplus
}
#endif

#endif // BSP_PRIV_H
