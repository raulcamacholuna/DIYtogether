/* Fecha: 17/08/2025 - 01:29 
# Fichero: components/ui/ui_actions_panel.h
# Último cambio: Eliminados los getters de botones individuales tras la refactorización a un gestor de botones.
# Descripción: Interfaz pública para el gestor de paneles de acciones. Ahora solo expone las funciones de alto nivel para crear, mostrar/ocultar y manejar gestos, ya que la gestión de los botones individuales se ha delegado al uttons_manager.
*/
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea todos los paneles de acción, delegando la creación de botones al `buttons_manager`.
 * @param parent El objeto padre sobre el cual se crearán los botones.
 */
void ui_actions_panel_create(lv_obj_t *parent);

/**
 * @brief Procesa un gesto de deslizamiento para mostrar u ocultar paneles.
 * @param dir La dirección del gesto detectado por LVGL.
 * @param start_x Coordenada X inicial del gesto.
 * @param start_y Coordenada Y inicial del gesto.
 */
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y);

/**
 * @brief Oculta inmediatamente cualquier panel de acciones que esté visible.
 *        Útil para forzar el cierre de los paneles, por ejemplo, al apagar la pantalla.
 */
void ui_actions_panel_hide_all(void);

#endif // UI_ACTIONS_PANEL_H
