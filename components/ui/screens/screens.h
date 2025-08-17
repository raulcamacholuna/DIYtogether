/* Fecha: 17/08/2025 - 02:49  */
/* Fichero: components/ui/screens/screens.h */
/* Último cambio: Movido a 'screens/' y refactorizado como el gestor de pantallas. */
/* Descripción: Interfaz pública del módulo de pantallas. Orquesta la creación y destrucción de las diferentes pantallas de la aplicación, empezando por la principal. */

#ifndef SCREENS_H
#define SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- DECLARACIONES DE OBJETOS GLOBALES ---
// Se expone para que otros módulos (core, telemetry, etc.) puedan añadir widgets.
extern lv_obj_t *g_main_screen_obj;


// --- DECLARACIONES DE FUNCIONES ---
/**
 * @brief Crea todas las pantallas de la aplicación.
 *        Actualmente, solo invoca la creación de la pantalla principal.
 */
void create_screens(void);

/**
 * @brief Destruye la pantalla principal y libera sus recursos.
 */
void delete_screen_main(void);


#ifdef __cplusplus
}
#endif

#endif // SCREENS_H
