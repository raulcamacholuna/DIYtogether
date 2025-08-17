/* Fecha: 17/08/2025 - 02:08  */
/* Fichero: components/ui/ui_layout.h */
/* Último cambio: Centralizadas todas las constantes de diseño (tamaño de botones, tiempos de animación, etc.) en un único fichero para facilitar el mantenimiento y la coherencia visual. */
/* Descripción: Este fichero contiene todas las constantes mágicas que definen la apariencia y el comportamiento de la UI. Centraliza valores como el tamaño de los botones, los paddings y los tiempos de animación para aplicar el principio DRY (Don't Repeat Yourself) y facilitar cambios de diseño globales. */

#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

// --- Constantes de Diseño de Botones ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10

// --- Constantes de Animación y Comportamiento de Paneles ---
#define ANIM_TIME_MS 300
#define PANEL_AUTO_HIDE_DELAY 10000 
#define EDGE_SWIPE_THRESHOLD 50

// --- Constantes Estructurales de Paneles ---
#define NUM_TOP_BUTTONS 3
#define NUM_SIDE_BUTTONS 5


#endif // UI_LAYOUT_H
