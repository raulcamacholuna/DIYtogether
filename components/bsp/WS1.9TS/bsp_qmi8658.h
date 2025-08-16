/*
 * Fichero: ./diymon_bsp/WS1.9TS/bsp_qmi8658.h
 * Fecha: 12/08/2025 - 09:25 pm
 * Último cambio: Corregidos los valores de los registros y añadidos los de calibración.
 * Descripción: Cabecera privada para el sensor QMI8658. Se han corregido las direcciones
 *              de los registros de datos (temperatura, acelerómetro, etc.) para que coincidan
 *              con la hoja de datos oficial y se han añadido los registros y comandos
 *              necesarios para la calibración y configuración avanzada del sensor.
 */
#ifndef BSP_QMI8658_H
#define BSP_QMI8658_H

#include <stdint.h>

// Definición de los registros del sensor QMI8658 (corregidos según datasheet)
typedef enum
{
    QMI8658_WHO_AM_I        = 0x00,
    QMI8658_REVISION_ID     = 0x01,
    QMI8658_CTRL1           = 0x02,
    QMI8658_CTRL2           = 0x03,
    QMI8658_CTRL3           = 0x04,
    QMI8658_CTRL4           = 0x05,
    QMI8658_CTRL5           = 0x06,
    QMI8658_CTRL6           = 0x07,
    QMI8658_CTRL7           = 0x08,
    QMI8658_CTRL9           = 0x0A,
    QMI8658_CAL1_L          = 0x0B,
    QMI8658_STATUS_INT      = 0x2D,
    QMI8658_TEMP_L          = 0x33,
    QMI8658_TEMP_H          = 0x34,
    QMI8658_AX_L            = 0x35,
    QMI8658_AX_H            = 0x36,
    QMI8658_AY_L            = 0x37,
    QMI8658_AY_H            = 0x38,
    QMI8658_AZ_L            = 0x39,
    QMI8658_AZ_H            = 0x3A,
    QMI8658_GX_L            = 0x3B,
    QMI8658_GX_H            = 0x3C,
    QMI8658_GY_L            = 0x3D,
    QMI8658_GY_H            = 0x3E,
    QMI8658_GZ_L            = 0x3F,
    QMI8658_GZ_H            = 0x40,
    QMI8658_RESET           = 0x60
} qmi8658_reg_t;

// Comandos para el registro CTRL9
typedef enum {
    QMI8658_CTRL9_CMD_NOP = 0x00,
    QMI8658_CTRL9_CMD_ON_DEMAND_CALI = 0xA2,
} qmi8658_ctrl9_cmd_t;

#endif // BSP_QMI8658_H
