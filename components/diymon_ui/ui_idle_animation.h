/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.h
 * Fecha: 13/08/2025 - 09:34 
 * Último cambio: Simplificada la función de inicio.
 * Descripción: Interfaz pública para la animación de reposo. La función `ui_idle_animation_start` se ha simplificado para que no necesite parámetros ni devuelva un valor, ya que opera sobre el objeto de imagen global compartido.
 */
#ifndef UI_IDLE_ANIMATION_H
#define UI_IDLE_ANIMATION_H

#include <lvgl.h>

/**
 * @brief Inicia la animación de idle.
 */
void ui_idle_animation_start(void);

/**
 * @brief Detiene y libera todos los recursos de la animación de idle.
 */
void ui_idle_animation_stop(void);

/**
 * @brief Pausa el temporizador de la animación de idle.
 */
void ui_idle_animation_pause(void);

/**
 * @brief Reanuda el temporizador de la animación de idle.
 */
void ui_idle_animation_resume(void);

#endif // UI_IDLE_ANIMATION_H
