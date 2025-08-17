/* Fecha: 17/08/2025 - 01:48  */
/* Fichero: components/ui/actions/action_interaction.h */
/* Último cambio: Creación del módulo para agrupar las acciones de interacción del Diymon. */
/* Descripción: Interfaz pública para las acciones de 'comer', 'ejercicio' y 'atacar'. Agrupa estas acciones relacionadas en un solo módulo, siguiendo el patrón de refactorización. */

#ifndef ACTION_INTERACTION_H
#define ACTION_INTERACTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Ejecuta la acción de 'comer', que dispara la animación correspondiente.
 */
void action_interaction_eat(void);

/**
 * @brief Ejecuta la acción de 'ejercicio', que dispara la animación correspondiente.
 */
void action_interaction_gym(void);

/**
 * @brief Ejecuta la acción de 'atacar', que dispara la animación correspondiente.
 */
void action_interaction_attack(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_INTERACTION_H
