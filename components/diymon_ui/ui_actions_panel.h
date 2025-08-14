/*
  Fichero: ./components/diymon_ui/ui_actions_panel.h
  Fecha: `$timestamp
  Último cambio: Corregido para ser sintácticamente válido en C.
  Descripción: Interfaz pública para el gestor de paneles. Se renombra `ui_actions_panel_get_enable_config_mode_btn` a `ui_actions_panel_get_enable_file_server_btn` para que coincida con la nueva funcionalidad.
*/
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

void ui_actions_panel_create(lv_obj_t *parent);
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y);

// --- Getters para botones del panel SUPERIOR 1 (JUGADOR) ---
lv_obj_t* ui_actions_panel_get_eat_btn(void);
lv_obj_t* ui_actions_panel_get_gym_btn(void);
lv_obj_t* ui_actions_panel_get_atk_btn(void);

// --- Getters para botones del panel SUPERIOR 2 (ADMIN) ---
lv_obj_t* ui_actions_panel_get_brightness_btn(void);
lv_obj_t* ui_actions_panel_get_toggle_screen_btn(void);
lv_obj_t* ui_actions_panel_get_admin_placeholder_btn(void);

// --- Getters para botones del panel LATERAL (EVOLUCIÓN) ---
lv_obj_t* ui_actions_panel_get_evo_fire_btn(void);
lv_obj_t* ui_actions_panel_get_evo_water_btn(void);
lv_obj_t* ui_actions_panel_get_evo_earth_btn(void);
lv_obj_t* ui_actions_panel_get_evo_wind_btn(void);
lv_obj_t* ui_actions_panel_get_evo_back_btn(void);

// --- Getters para botones del panel SUPERIOR 3 (CONFIG) ---
lv_obj_t* ui_actions_panel_get_reset_all_btn(void);
lv_obj_t* ui_actions_panel_get_enable_file_server_btn(void); // Renombrado
lv_obj_t* ui_actions_panel_get_config_placeholder_btn(void);

#endif // UI_ACTIONS_PANEL_H
