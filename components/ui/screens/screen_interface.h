/* Fecha: 17/08/2025 - 03:35  */
/* Fichero: components/ui/screens/screen_interface.h */
/* Último cambio: Creación de la interfaz común para las pantallas para estandarizar su ciclo de vida. */
/* Descripción: Define una interfaz común (un 'contrato') para todas las pantallas de la aplicación. Esto permite que un futuro gestor de navegación trate a todas las pantallas de manera uniforme, llamando a sus funciones de ciclo de vida (crear, mostrar, etc.) sin conocer su implementación específica, reduciendo así el acoplamiento y mejorando la modularidad. */

#ifndef SCREEN_INTERFACE_H
#define SCREEN_INTERFACE_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define una interfaz común para el ciclo de vida de una pantalla.
 *
 * Esta estructura de punteros a función permite manejar diferentes pantallas
 * de forma polimórfica.
 */
typedef struct {
    /**
     * @brief Crea los widgets de la pantalla sobre un objeto padre.
     * @param parent El objeto LVGL que actuará como contenedor.
     */
    void (*create)(lv_obj_t *parent);

    /**
     * @brief Hace visible la pantalla y sus elementos.
     */
    void (*show)(void);

    /**
     * @brief Oculta la pantalla y sus elementos.
     */
    void (*hide)(void);

    /**
     * @brief Destruye la pantalla y libera todos sus recursos.
     */
    void (*destroy)(void);
} screen_interface_t;

#ifdef __cplusplus
}
#endif

#endif // SCREEN_INTERFACE_H
