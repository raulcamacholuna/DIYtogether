/* Fecha: 17/08/2025 - 02:10  */
/* Fichero: components/ui/ui_state_manager.h */
/* Último cambio: Creación del gestor de estado de la UI para encapsular la lógica de inactividad. */
/* Descripción: Interfaz pública del gestor de estado de la UI. Este módulo es responsable de manejar la inactividad del usuario, el atenuado y apagado automático de la pantalla, y la lógica de despertar. Expone una única función de inicialización para mantener su implementación interna encapsulada. */

#ifndef UI_STATE_MANAGER_H
#define UI_STATE_MANAGER_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa el gestor de estado de la UI.
 * 
 * Configura los temporizadores y los callbacks de eventos necesarios para
 * gestionar la inactividad, el atenuado/apagado de la pantalla y la lógica
 * de despertar. Debe ser llamado una vez después de que la UI principal
 * haya sido creada.
 */
void ui_state_manager_init(void);

#ifdef __cplusplus
}
#endif

#endif // UI_STATE_MANAGER_H
