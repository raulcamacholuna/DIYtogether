/*
 * Fichero: components/diymon_ui/ui.h
 * Descripción: Interfaz PÚBLICA del componente de la UI.
 *              Define las funciones de alto nivel para controlar la UI desde el exterior.
 */
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