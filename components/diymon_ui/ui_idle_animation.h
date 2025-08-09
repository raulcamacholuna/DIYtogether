/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.h
 * Fecha: 11/08/2025 - 10:15
 * Último cambio: Añadidas declaraciones para pausar y reanudar la animación.
 * Descripción: Interfaz pública para la animación de reposo. Se añaden controles
 *              para pausar y reanudar el temporizador de la animación, permitiendo
 *              que otras partes de la UI puedan detenerla temporalmente sin destruirla.
 */
#ifndef UI_IDLE_ANIMATION_H
#define UI_IDLE_ANIMATION_H

#include <lvgl.h>

/**
 * @brief Inicia la animación de idle a pantalla completa.
 * @param parent El objeto padre sobre el que se creará la animación (la pantalla principal).
 * @return Un puntero al objeto de imagen de la animación para que otros módulos puedan interactuar con él (ej: ocultarlo).
 */
lv_obj_t* ui_idle_animation_start(lv_obj_t *parent);

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