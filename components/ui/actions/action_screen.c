/* Fichero: components/ui/actions/action_screen.c */
/* Descripción: Se ha refactorizado la acción del botón de pantalla. Ahora, en lugar de un 'toggle', invoca directamente la nueva función 'screen_manager_enter_light_sleep'. Esto alinea el comportamiento del botón con el requisito del usuario de entrar en modo de bajo consumo de forma inmediata, utilizando la nueva interfaz modular del 'screen_manager'. */
/* Último cambio: 21/08/2025 - 21:37 */
#include "actions/action_screen.h"
#include "screen_manager.h"
#include "ui_actions_panel.h" // Necesario para ocultar los paneles

void action_screen_toggle(void) {
    if (!screen_manager_is_off()) {
        ui_actions_panel_hide_all(); 
        screen_manager_enter_light_sleep();
    }
}
