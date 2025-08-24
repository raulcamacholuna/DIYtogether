// Fecha: 24/08/2025 - 17:30
// Fichero: components/ui/actions/action_zigbee.h
// Último cambio: Añadidas las declaraciones de funciones para acciones de Zigbee.
// Descripción: Este archivo de cabecera ahora declara las funciones para iniciar y unirse a juegos Zigbee
// desde la interfaz de usuario, permitiendo que actions.c las invoque correctamente.

#ifndef UI_ACTION_ZIGBEE_H
#define UI_ACTION_ZIGBEE_H

#ifdef __cplusplus
extern "C" {
#endif

void action_zigbee_create_game();
void action_zigbee_join_game();

#ifdef __cplusplus
}
#endif

#endif // UI_ACTION_ZIGBEE_H