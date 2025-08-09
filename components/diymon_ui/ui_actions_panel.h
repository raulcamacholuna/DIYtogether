/*
 * Fichero: ./components/diymon_ui/ui_actions_panel.h
 * Fecha: 12/08/2025 - 11:35
 * Último cambio: Añadidas las declaraciones para los botones del panel de administración.
 * Descripción: Interfaz pública para el módulo gestor de paneles de acción. Expone funciones para crear los paneles, manejar gestos y obtener los manejadores de todos los botones.
 */
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea los paneles de acciones (jugador y admin).
 * @param parent El objeto padre sobre el que se crearán los paneles.
 */
void ui_actions_panel_create(lv_obj_t *parent);

/**
 * @brief Procesa un gesto detectado en la pantalla principal para mostrar/ocultar los paneles.
 * @param dir La dirección del gesto.
 * @param start_y La coordenada 'y' donde se inició el toque.
 */
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_y);

// --- Getters para botones del panel de JUGADOR ---
lv_obj_t* ui_actions_panel_get_eat_btn(void);
lv_obj_t* ui_actions_panel_get_gym_btn(void);
lv_obj_t* ui_actions_panel_get_atk_btn(void);

// --- Getters para botones del panel de ADMIN ---
lv_obj_t* ui_actions_panel_get_lvl_down_btn(void);
lv_obj_t* ui_actions_panel_get_screen_off_btn(void);
lv_obj_t* ui_actions_panel_get_lvl_up_btn(void);


#endif // UI_ACTIONS_PANEL_H