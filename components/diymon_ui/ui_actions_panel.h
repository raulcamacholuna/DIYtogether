/* Fecha: 16/08/2025 - 07:19  */
/* Fichero: components/diymon_ui/ui_actions_panel.h */
/* Último cambio: Añadida la declaración de ui_actions_panel_hide_all() para ocultar paneles bajo demanda. */
/* Descripción: Interfaz pública para el gestor de paneles de acciones. Expone funciones para crear los paneles, manejar gestos y, ahora, forzar el ocultado de cualquier panel visible. */

#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea todos los paneles de acción y sus botones.
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

// --- Getters para botones (se mantienen para la conexión de eventos) ---
lv_obj_t* ui_actions_panel_get_eat_btn(void);
lv_obj_t* ui_actions_panel_get_gym_btn(void);
lv_obj_t* ui_actions_panel_get_atk_btn(void);
lv_obj_t* ui_actions_panel_get_brightness_btn(void);
lv_obj_t* ui_actions_panel_get_toggle_screen_btn(void);
lv_obj_t* ui_actions_panel_get_admin_placeholder_btn(void);
lv_obj_t* ui_actions_panel_get_reset_all_btn(void);
lv_obj_t* ui_actions_panel_get_enable_file_server_btn(void);
lv_obj_t* ui_actions_panel_get_config_placeholder_btn(void);
lv_obj_t* ui_actions_panel_get_evo_fire_btn(void);
lv_obj_t* ui_actions_panel_get_evo_water_btn(void);
lv_obj_t* ui_actions_panel_get_evo_earth_btn(void);
lv_obj_t* ui_actions_panel_get_evo_wind_btn(void);
lv_obj_t* ui_actions_panel_get_evo_back_btn(void);

#endif // UI_ACTIONS_PANEL_H
