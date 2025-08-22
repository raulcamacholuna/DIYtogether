/* Fichero: components/ui/actions/action_screen.c */
/* Descripción: Se ha refactorizado la acción para implementar un 'sueño de UI'. En lugar de invocar el light sleep del sistema, ahora se pausan explícitamente los componentes de la UI que consumen CPU (state_manager, animación de idle) y luego se apaga el backlight. Esto mantiene el manejador de eventos de LVGL activo para detectar el despertar. */
/* Último cambio: 22/2025 - 08:07
*/
#include "actions/action_screen.h"
#include "screen_manager.h"
#include "ui_actions_panel.h"
#include "core/state_manager.h"
#include "ui_idle_animation.h"

void action_screen_toggle(void) {
    if (!screen_manager_is_off()) {
        ui_actions_panel_hide_all(); 
        state_manager_pause();
        ui_idle_animation_pause();
        screen_manager_turn_off();
    }
}
