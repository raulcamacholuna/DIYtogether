/* Fecha: 17/08/2025 - 01:49  */
/* Fichero: components/ui/actions/action_evolution.h */
/* Último cambio: Creación del módulo para agrupar las acciones de evolución del Diymon. */
/* Descripción: Interfaz pública para las acciones de evolución y de involución. Agrupa la lógica relacionada con el cambio de estado del Diymon para mantener el orquestador principal limpio. */

#ifndef ACTION_EVOLUTION_H
#define ACTION_EVOLUTION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Intenta ejecutar una evolución ramificada basada en el estado actual.
 * @param branch_id El identificador de la rama a la que se intenta evolucionar (1-4).
 */
void action_evolution_branch(int branch_id);

/**
 * @brief Ejecuta una involución al estado evolutivo anterior.
 */
void action_evolution_devolve(void);

#ifdef __cplusplus
}
#endif

#endif // ACTION_EVOLUTION_H
