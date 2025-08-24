/* Fecha: 17/08/2025 - 01:29 
# Fichero: components\ui\buttons\buttons_manager.c
# Último cambio: Implementación del orquestador de botones.
# Descripción: Este fichero centraliza la creación de todos los botones de la UI. La función ui_buttons_init llama a las funciones de creación de cada módulo de botón individual, y los getters proporcionan acceso a sus manejadores para otros módulos.
*/
#include "buttons_manager.h"

// Include all individual button modules
#include "btn_1.h"
#include "btn_2.h"
#include "btn_3.h"
#include "btn_4.h"
#include "btn_5.h"
#include "btn_6.h"
#include "btn_7.h"
#include "btn_8.h"
#include "btn_9.h"
#include "evo_1.h"
#include "evo_2.h"
#include "evo_3.h"
#include "evo_4.h"
#include "evo_5.h"

/**
 * @brief Inicializa y crea todos los botones de la UI llamando a los constructores de cada módulo.
 */
void ui_buttons_init(lv_obj_t *parent, bool animations_enabled) {
    // --- Panel de Jugador (3 botones) ---
    btn_1_create(parent);
    btn_2_create(parent);
    btn_3_create(parent);

    // --- Panel de Administración (3 botones) ---
    btn_4_create(parent);
    btn_5_create(parent);
    btn_6_create(parent);
    
    // --- Panel de Configuración (3 botones) ---
    btn_7_create(parent);
    btn_8_create(parent, animations_enabled);
    btn_9_create(parent);

    // --- Panel Lateral de Evolución (5 botones) ---
    evo_1_create(parent);
    evo_2_create(parent);
    evo_3_create(parent);
    evo_4_create(parent);
    evo_5_create(parent);
}

// --- Implementación de Getters ---
lv_obj_t* ui_buttons_get_btn_1(void) { return btn_1_get_handle(); }
lv_obj_t* ui_buttons_get_btn_2(void) { return btn_2_get_handle(); }
lv_obj_t* ui_buttons_get_btn_3(void) { return btn_3_get_handle(); }
lv_obj_t* ui_buttons_get_btn_4(void) { return btn_4_get_handle(); }
lv_obj_t* ui_buttons_get_btn_5(void) { return btn_5_get_handle(); }
lv_obj_t* ui_buttons_get_btn_6(void) { return btn_6_get_handle(); }
lv_obj_t* ui_buttons_get_btn_7(void) { return btn_7_get_handle(); }
lv_obj_t* ui_buttons_get_btn_8(void) { return btn_8_get_handle(); }
lv_obj_t* ui_buttons_get_btn_9(void) { return btn_9_get_handle(); }

lv_obj_t* ui_buttons_get_evo_1(void) { return evo_1_get_handle(); }
lv_obj_t* ui_buttons_get_evo_2(void) { return evo_2_get_handle(); }
lv_obj_t* ui_buttons_get_evo_3(void) { return evo_3_get_handle(); }
lv_obj_t* ui_buttons_get_evo_4(void) { return evo_4_get_handle(); }
lv_obj_t* ui_buttons_get_evo_5(void) { return evo_5_get_handle(); }
