/*
 * Archivo: ui_priv.h
 * Descripción: Header PRIVADO para el componente UI.
 *              Comparte definiciones y variables globales SOLO entre los .c de la UI.
 * Versión: 1.0
 */
#ifndef UI_PRIV_H
#define UI_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// 1. [DEFINICIÓN ÚNICA] La estructura que contiene los punteros a nuestros objetos de UI.
typedef struct {
    lv_obj_t *main;
    lv_obj_t *idle;
    lv_obj_t *comer;
    lv_obj_t *pesas;
    lv_obj_t *atacar;
    // Añade aquí cualquier otro objeto que necesites referenciar desde múltiples archivos.
} objects_t;

// 2. [DECLARACIÓN EXTERN] Le decimos a los archivos .c que incluyan este header
//    que una variable global llamada 'objects' existe y será definida en otro lugar
//    (concretamente, en screens.c).
extern objects_t objects;

#ifdef __cplusplus
}
#endif

#endif // UI_PRIV_H