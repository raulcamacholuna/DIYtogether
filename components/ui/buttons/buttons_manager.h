/* Fecha: 17/08/2025 - 01:28 
# Fichero: components\ui\buttons\buttons_manager.h
# Último cambio: Creación del fichero de cabecera para el orquestador de botones.
# Descripción: Define la interfaz pública del gestor de botones. Expone una función de inicialización para crear todos los botones de la UI y getters individuales para que otros módulos, como el gestor de paneles, puedan acceder a sus manejadores.
*/
#ifndef BUTTONS_MANAGER_H
#define BUTTONS_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa y crea todos los botones de la interfaz de usuario.
 * 
 * @param parent El objeto LVGL padre sobre el cual se crearán todos los botones.
 */
void ui_buttons_init(lv_obj_t *parent, bool animations_enabled);

// --- Getters para los botones del panel superior ---
lv_obj_t* ui_buttons_get_btn_1(void); // Comer
lv_obj_t* ui_buttons_get_btn_2(void); // Ejercicio
lv_obj_t* ui_buttons_get_btn_3(void); // Atacar
lv_obj_t* ui_buttons_get_btn_4(void); // Brillo
lv_obj_t* ui_buttons_get_btn_5(void); // Apagar Pantalla
lv_obj_t* ui_buttons_get_btn_6(void); // Modo Config
lv_obj_t* ui_buttons_get_btn_7(void); // Reset Total
lv_obj_t* ui_buttons_get_btn_8(void); // Servidor Ficheros
lv_obj_t* ui_buttons_get_btn_9(void); // Placeholder Config

// --- Getters para los botones del panel lateral (Evolución) ---
lv_obj_t* ui_buttons_get_evo_1(void); // Fuego
lv_obj_t* ui_buttons_get_evo_2(void); // Agua
lv_obj_t* ui_buttons_get_evo_3(void); // Tierra
lv_obj_t* ui_buttons_get_evo_4(void); // Viento
lv_obj_t* ui_buttons_get_evo_5(void); // Involucionar


#ifdef __cplusplus
}
#endif

#endif // BUTTONS_MANAGER_H
