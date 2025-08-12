/*
  Fichero: ./components/diymon_ui/ui_actions_panel.h
  Fecha: 13/08/2025 - 21:15
  Último cambio: Añadidos getters para los botones del panel de configuración.
  Descripción: Interfaz pública para el gestor de paneles. Se añaden getters
               para el nuevo panel de configuración superior.
*/
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea todos los paneles de acciones (superiores y lateral).
 * @param parent El objeto padre sobre el que se crearán los paneles.
 */
void ui_actions_panel_create(lv_obj_t *parent);

/**
 * @brief Procesa un gesto detectado en la pantalla principal para mostrar/ocultar los paneles.
 * @param dir La dirección del gesto.
 * @param start_x La coordenada 'x' donde se inició el toque.
 * @param start_y La coordenada 'y' donde se inició el toque.
 */
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
lv_obj_t* ui_actions_panel_get_enable_ftp_btn(void);
lv_obj_t* ui_actions_panel_get_config_placeholder_btn(void);


#endif // UI_ACTIONS_PANEL_H