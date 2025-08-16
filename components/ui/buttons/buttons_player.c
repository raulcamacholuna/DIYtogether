/* Fecha: 16/08/2025 - 09:09  */
/* Fichero: components/ui/buttons/buttons_player.c */
/* Último cambio: Creación del fichero de implementación para los botones del jugador. */
/* Descripción: Este fichero contiene la lógica para crear, posicionar y conectar los eventos de los botones de acción del jugador (Comer, Ejercicio, Atacar). Está diseñado para ser utilizado por el gestor de paneles de la UI. */

#include "buttons_player.h"
#include "actions.h"
#include "ui_asset_loader.h"
#include "esp_log.h"

// --- Constantes de diseño ---
#define BUTTON_SIZE 50
#define BUTTON_PADDING 10

// --- Variables estáticas para los objetos de los botones ---
// 'static' asegura que estas variables solo son visibles dentro de este fichero.
static lv_obj_t *s_eat_btn = NULL;
static lv_obj_t *s_gym_btn = NULL;
static lv_obj_t *s_atk_btn = NULL;

/**
 * @brief Callback genérico para todos los botones de este módulo.
 *        Ejecuta la acción correspondiente al botón presionado.
 * @param e Puntero al evento de LVGL.
 */
static void button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSED || code == LV_EVENT_CLICKED) {
        // Obtenemos el ID de la acción que guardamos al conectar el evento.
        diymon_action_id_t action_id = (diymon_action_id_t)(intptr_t)lv_event_get_user_data(e);
        execute_diymon_action(action_id);
    }
}

/**
 * @brief Función de ayuda para crear un botón de acción estandarizado.
 * @param parent El objeto padre.
 * @param asset_id El icono a mostrar en el botón.
 * @param index La posición del botón en el panel (0, 1, 2...).
 * @return Un puntero al objeto de botón creado.
 */
static lv_obj_t* create_player_action_button(lv_obj_t *parent, ui_asset_id_t asset_id, int index) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_size(btn, BUTTON_SIZE, BUTTON_SIZE);
    
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_STATE_ANY);
    lv_obj_set_style_border_width(btn, 0, LV_STATE_ANY);
    lv_obj_set_style_shadow_width(btn, 0, LV_STATE_ANY);

    lv_obj_t *img = lv_img_create(btn);
    lv_img_set_src(img, ui_assets_get_icon(asset_id));
    lv_obj_center(img);
        
    // Posición inicial fuera de la pantalla para la animación de entrada.
    lv_obj_set_pos(btn, (BUTTON_SIZE + BUTTON_PADDING) * index, -BUTTON_SIZE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
    
    return btn;
}

// --- Implementación de Funciones Públicas ---

void ui_buttons_player_create(lv_obj_t *parent) {
    s_eat_btn = create_player_action_button(parent, ASSET_ICON_EAT, 0);
    s_gym_btn = create_player_action_button(parent, ASSET_ICON_GYM, 1);
    s_atk_btn = create_player_action_button(parent, ASSET_ICON_ATK, 2);
}

void ui_buttons_player_connect_events(void) {
    if (s_eat_btn) lv_obj_add_event_cb(s_eat_btn, button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_COMER);
    if (s_gym_btn) lv_obj_add_event_cb(s_gym_btn, button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_EJERCICIO);
    if (s_atk_btn) lv_obj_add_event_cb(s_atk_btn, button_event_cb, LV_EVENT_PRESSED, (void*)ACTION_ID_ATACAR);
}

lv_obj_t* ui_buttons_player_get_eat_btn(void) {
    return s_eat_btn;
}

lv_obj_t* ui_buttons_player_get_gym_btn(void) {
    return s_gym_btn;
}

lv_obj_t* ui_buttons_player_get_atk_btn(void) {
    return s_atk_btn;
}
