/*
 * Fichero: ui_priv.h
 * Fecha: 08/08/2025
 * Último cambio: Añadido puntero para el panel de acciones desplegable.
 * Descripción: Cabecera PRIVADA para el componente UI.
 *              Comparte definiciones y variables globales SOLO entre los .c de la UI.
 */
#ifndef UI_PRIV_H
#define UI_PRIV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

/**
 * @struct objects_t
 * @brief  Contiene los punteros a todos los objetos LVGL gestionados por la UI.
 * 
 * Esta estructura centraliza el acceso a los elementos visuales. Se agrupan por
 * tipo (objetos principales, panel, botones, animaciones) para una mayor claridad.
 */
typedef struct {
    // --- Objetos Principales ---
    lv_obj_t *main;         // Objeto de la pantalla principal.
    lv_obj_t *idle;         // GIF de la animación de reposo.

    // --- Panel de Acciones Desplegable ---
    lv_obj_t *actions_panel; // El contenedor que se desliza desde arriba.

    // --- Botones de Acción (contenidos en el panel) ---
    lv_obj_t *comer;        // Botón para la acción 'Comer'.
    lv_obj_t *pesas;        // Botón para la acción 'Ejercicio'.
    lv_obj_t *atacar;       // Botón para la acción 'Atacar'.

    // --- Animaciones GIF de Acciones ---
    lv_obj_t *comiendo;     // GIF para la animación 'Comiendo'.
    lv_obj_t *ejercicio;    // GIF para la animación 'Ejercicio'.
    lv_obj_t *ataque;       // GIF para la animación 'Ataque'.

} objects_t;

/**
 * @brief Variable global 'objects'.
 * 
 * Declaración 'extern' que permite a todos los ficheros del componente UI
 * acceder a la instancia única de la estructura 'objects_t', que será
 * definida en 'screens.c'.
 */
extern objects_t objects;

#ifdef __cplusplus
}
#endif

#endif // UI_PRIV_H