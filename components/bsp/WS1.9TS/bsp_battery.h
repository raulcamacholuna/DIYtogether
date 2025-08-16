/*
 * Fichero: ./components/diymon_bsp/WS1.9TS/bsp_battery.h
 * Fecha: 13/08/2025 - 12:06 
 * Último cambio: Corregido el tipo de retorno de la función de inicialización.
 * Descripción: Cabecera privada para el driver de la batería. Se ha corregido la firma
 *              de sp_battery_init a sp_err_t para que sea consistente con la
 *              API pública y evitar un crash en tiempo de ejecución.
 */
#ifndef __BSP_BATTERY_H__
#define __BSP_BATTERY_H__
#include <stdio.h>
#include "esp_err.h"
#include <stdint.h>

#define EXAMPLE_ADC_UNIT ADC_UNIT_1
#define EXAMPLE_BATTERY_ADC_CHANNEL ADC_CHANNEL_0
#define EXAMPLE_ADC_ATTEN ADC_ATTEN_DB_12


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t bsp_battery_init(void);
void bsp_battery_get_voltage(float *voltage, uint16_t *adc_value);

#ifdef __cplusplus
}
#endif


#endif
