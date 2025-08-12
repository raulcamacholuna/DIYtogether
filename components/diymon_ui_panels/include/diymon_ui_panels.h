/*
  Fichero: ./components/diymon_ui/ui_actions_panel.h
  Fecha: 12/08/2025 - 08:00 pm
  Último cambio: Creado como parte de la unificación de la UI.
  Descripción: Interfaz pública para el gestor de paneles de acciones, ahora
               integrado dentro del componente `diymon_ui`.
*/
#ifndef UI_ACTIONS_PANEL_H
#define UI_ACTIONS_PANEL_H

#include <lvgl.h>

void ui_actions_panel_create(lv_obj_t *parent);
void ui_actions_panel_handle_gesture(lv_dir_t dir, lv_coord_t start_x, lv_coord_t start_y);

#endif // UI_ACTIONS_PANEL_H