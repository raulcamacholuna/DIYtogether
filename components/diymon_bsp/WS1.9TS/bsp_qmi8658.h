/*
 * Fichero: ./diymon_bsp/WS1.9TS/bsp_qmi8658.h
 * Fecha: 08/08/2025 - 02:40
 * Último cambio: Convertido en cabecera privada del componente.
 * Descripción: Declaraciones y registros específicos para el sensor QMI8658. Esta cabecera es privada y solo debe ser incluida por los ficheros `.c` dentro del mismo componente BSP.
 */
#ifndef BSP_QMI8658_H
#define BSP_QMI8658_H

#include <stdint.h>

// Definición de los registros del sensor QMI8658
typedef enum
{
    QMI8658_WHO_AM_I        = 0x00,
    QMI8658_REVISION_ID     = 0x01,
    QMI8658_CTRL1           = 0x02,
    QMI8658_CTRL2           = 0x03,
    QMI8658_CTRL3           = 0x04,
    QMI8658_CTRL4           = 0x05,
    QMI8658_CTRL5           = 0x06,
    QMI8658_CTRL7           = 0x08,
    //... (se pueden añadir más registros si es necesario)
    QMI8658_TEMP_L          = 0x31,
    QMI8658_TEMP_H          = 0x32,
    QMI8658_AX_L            = 0x33,
    QMI8658_AX_H            = 0x34,
    QMI8658_AY_L            = 0x35,
    QMI8658_AY_H            = 0x36,
    QMI8658_AZ_L            = 0x37,
    QMI8658_AZ_H            = 0x38,
    QMI8658_GX_L            = 0x39,
    QMI8658_GX_H            = 0x3A,
    QMI8658_GY_L            = 0x3B,
    QMI8658_GY_H            = 0x3C,
    QMI8658_GZ_L            = 0x3D,
    QMI8658_GZ_H            = 0x3E,
} qmi8658_reg_t;

#endif // BSP_QMI8658_H