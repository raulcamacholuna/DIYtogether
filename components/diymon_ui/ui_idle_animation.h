/*
 * Fichero: ./components/diymon_ui/ui_idle_animation.h
 * Fecha: 13/08/2025 - 11:43 
 * Último cambio: Corregida la firma de la función de inicio.
 * Descripción: Interfaz pública para la animación de reposo. Se corrige la
 *              declaración de 'ui_idle_animation_start' para que acepte un
 *              parámetro y devuelva un puntero a objeto, solucionando el error
 *              de compilación.
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
