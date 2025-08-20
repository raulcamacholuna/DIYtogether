/* Fichero: components/bsp/bsp_battery.h */
/* Último cambio: Creado como fichero de cabecera privado y unificado para el driver de la batería. */
/* Descripción: Cabecera privada para el driver de la batería. Define las constantes de configuración del ADC. Al ser idénticas para ambas placas, este fichero es común y no requiere directivas de preprocesador. */
/* Último cambio: 20/08/2025 - 05:06 */
#ifndef __BSP_BATTERY_H__
#define __BSP_BATTERY_H__

#include <stdio.h>
#include "esp_err.h"
#include <stdint.h>

#define EXAMPLE_ADC_UNIT            ADC_UNIT_1
#define EXAMPLE_BATTERY_ADC_CHANNEL ADC_CHANNEL_0
#define EXAMPLE_ADC_ATTEN           ADC_ATTEN_DB_12

#endif // __BSP_BATTERY_H__
