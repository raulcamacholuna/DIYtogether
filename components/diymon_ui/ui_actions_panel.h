/*
 * Fichero: ./components/diymon_ui/ui_actions_panel.h
 * Fecha: 10/08/2025 - 03:15
 * Último cambio: Creación del fichero.
 * Descripción: Interfaz pública para el módulo del panel de acciones. Expone funciones para crear el panel, manejar gestos y obtener los manejadores de los botones.
 */
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

/**
 * @brief Crea el panel de acciones superior.
 * @param parent El objeto padre sobre el que se creará el panel.
 */
void ui_actions_panel_create(lv_obj_t *parent);

/**
 * @brief Procesa un gesto detectado en la pantalla principal para mostrar u ocultar el panel.
 * @param dir La dirección del gesto.
 * @param start_y La coordenada 'y' donde se inició el toque.
 */
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_y);

/**
 * @brief Obtiene el manejador del botón 'comer'.
 * @return Puntero al objeto del botón.
 */
lv_obj_t* ui_actions_panel_get_eat_btn(void);

/**
 * @brief Obtiene el manejador del botón 'pesas'.
 * @return Puntero al objeto del botón.
 */
lv_obj_t* ui_actions_panel_get_gym_btn(void);

/**
 * @brief Obtiene el manejador del botón 'atacar'.
 * @return Puntero al objeto del botón.
 */
lv_obj_t* ui_actions_panel_get_atk_btn(void);

#endif // UI_ACTIONS_PANEL_H