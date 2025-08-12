/*
 * Fichero: ./components/diymon_core/include/diymon_evolution.h
 * Fecha: 13/08/2025 - 19:45
 * Último cambio: Añadida la declaración para resetear el estado.
 * Descripción: Cabecera del motor de evolución. Expone las funciones para gestionar el estado y la secuencia de evolución del DIYMON, incluyendo la involución y el reseteo de estado.
 */
#ifndef DIYMON_EVOLUTION_H
#define DIYMON_EVOLUTION_H

#include <stdint.h> // Para usar tipos como uint8_t

// Estructura para almacenar las estadísticas de una forma canónica
typedef struct {
    uint8_t fue;
    uint8_t res;
    uint8_t vel;
    uint8_t intel;
} diymon_stats_t;

/**
 * @brief Inicializa el motor de evolución.
 */
void diymon_evolution_init(void);

/**
 * @brief Obtiene las estadísticas base para un Código Evolutivo dado.
 * @param evo_code El código a buscar (ej: "1.2.3").
 * @return Un puntero a las estadísticas (solo lectura) o NULL si no se encuentra.
 */
const diymon_stats_t* diymon_get_stats_for_code(const char* evo_code);

/**
 * @brief Establece el código evolutivo del DIYMON activo.
 * @param new_code El nuevo código a establecer.
 */
void diymon_set_current_code(const char* new_code);

/**
 * @brief Obtiene el código evolutivo del DIYMON activo.
 * @return Un puntero al código actual.
 */
const char* diymon_get_current_code(void);

/**
 * @brief Obtiene el siguiente código en la secuencia de evolución predefinida.
 * @param current_code El código actual.
 * @return El siguiente código en la secuencia, o NULL si es la evolución final.
 */
const char* diymon_get_next_evolution_in_sequence(const char* current_code);

/**
 * @brief Obtiene el código anterior en la secuencia de evolución predefinida.
 * @param current_code El código actual.
 * @return El código anterior en la secuencia, o NULL si es la forma inicial.
 */
const char* diymon_get_previous_evolution_in_sequence(const char* current_code);

/**
 * @brief Borra el estado de evolución guardado en la NVS.
 */
void diymon_evolution_reset_state(void);


#endif // DIYMON_EVOLUTION_H