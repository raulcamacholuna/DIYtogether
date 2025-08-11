/*
 * Fichero: ./components/diymon_ui/ui_actions_panel.h
 * Fecha: 11/08/2025 - 14:30
 * Último cambio: Renombrados los getters de botones del panel de administración.
 * Descripción: Interfaz pública para el módulo gestor de paneles de acción. Expone funciones para crear los paneles, manejar gestos y obtener los manejadores de todos los botones.
 */
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea todos los paneles de acciones (superior y lateral).
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

// --- Getters para botones del panel SUPERIOR de JUGADOR ---
lv_obj_t* ui_actions_panel_get_eat_btn(void);
lv_obj_t* ui_actions_panel_get_gym_btn(void);
lv_obj_t* ui_actions_panel_get_atk_btn(void);

// --- ANOTACIÓN: Getters para botones del panel SUPERIOR de ADMIN (nuevas funciones) ---
lv_obj_t* ui_actions_panel_get_brightness_btn(void); // Antes ui_actions_panel_get_lvl_down_btn
lv_obj_t* ui_actions_panel_get_toggle_screen_btn(void); // Antes ui_actions_panel_get_screen_off_btn
lv_obj_t* ui_actions_panel_get_erase_wifi_btn(void); // Antes ui_actions_panel_get_lvl_up_btn

// --- Getters para botones del panel LATERAL de EVOLUCIÓN --- (Se mantienen)
lv_obj_t* ui_actions_panel_get_evo_fire_btn(void);
lv_obj_t* ui_actions_panel_get_evo_water_btn(void);
lv_obj_t* ui_actions_panel_get_evo_earth_btn(void);
lv_obj_t* ui_actions_panel_get_evo_wind_btn(void);
lv_obj_t* ui_actions_panel_get_evo_back_btn(void);


#endif // UI_ACTIONS_PANEL_H