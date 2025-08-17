/* Fecha: 17/08/2025 - 02:29  */
/* Fichero: components/ui/core/ui.h */
/* Último cambio: Movido a ui/core/ como parte de la refactorización del núcleo de la UI. */
/* Descripción: Interfaz PÚBLICA del componente de la UI. Define las funciones de alto nivel para controlar la UI desde el exterior. Se ha movido a este subdirectorio para encapsular la lógica principal. */

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

// --- DEFINICIONES COMPARTIDAS ---

// Enum para identificar las pantallas. Se mantiene aquí porque es parte de la
// interfaz pública para, potencialmente, cambiar de pantalla.
enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};


// --- FUNCIONES PÚBLICAS DE LA UI ---

/**
 * @brief Pre-inicializa la UI reservando los buffers de memoria más grandes.
 *
 * Esta función debe llamarse al principio de app_main para evitar la fragmentación
 * de la memoria RAM, asegurando que la asignación de memoria más grande (el buffer
 * de animación) tenga éxito.
 */
void ui_preinit(void);

/**
 * @brief Inicializa y configura toda la interfaz de usuario.
 *
 * Esta es la función principal que se debe llamar desde fuera del componente
 * (por ejemplo, desde main.c) para poner en marcha toda la UI.
 */
void ui_init(void);


#ifdef __cplusplus
}
#endif

#endif // UI_H
