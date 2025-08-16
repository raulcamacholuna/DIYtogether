/* Fecha: 15/08/2025 - 09:54  */
/* Fichero: Z:\DIYTOGETHER\DIYtogether\components\diymon_core\diymon_evolution.c */
/* Último cambio: Añadidas las evoluciones de la Etapa 3 para resolver el error de "evolución no válida". */
/* Descripción: Se ha expandido la tabla maestra de evoluciones (`G_MASTER_TABLE`) para incluir las ramificaciones de la Etapa 3 que faltaban (ej: desde "3.3" a "3.3.1"). Esto corrige el error reportado por el usuario y permite que el DIYMON continúe evolucionando. */

#include "diymon_evolution.h"
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char* TAG = "DIYMON_CORE";

// --- LA TABLA MAESTRA DE ESTADÍSTICAS ---
static const struct {
    const char* evo_code;
    const diymon_stats_t stats;
} G_MASTER_TABLE[] = {
    // Base
    {"0",       {5, 5, 5, 5}},
    // Etapa 1
    {"1",       {8, 5, 6, 5}}, // Fuego
    {"2",       {5, 8, 5, 6}}, // Agua
    {"3",       {6, 5, 8, 5}}, // Tierra
    {"4",       {5, 6, 5, 8}}, // Aire
    // Etapa 2 (ramas de "1" - Fuego)
    {"1.1",     {10, 5, 7, 7}}, // Fuego+Fuego
    {"1.2",     {7, 8, 7, 7}}, // Fuego+Agua
    {"1.3",     {8, 7, 8, 6}}, // Fuego+Tierra
    {"1.4",     {7, 7, 6, 8}}, // Fuego+Aire
    // Etapa 2 (ramas de "2" - Agua)
    {"2.1",     {8, 7, 7, 7}}, // Agua+Fuego
    {"2.2",     {5, 10, 7, 7}}, // Agua+Agua
    // Etapa 2 (ramas de "3" - Tierra)
    {"3.1",     {8, 6, 8, 7}}, // Tierra+Fuego
    {"3.3",     {7, 7, 10, 5}}, // Tierra+Tierra
    // Etapa 2 (ramas de "4" - Aire)
    {"4.1",     {8, 7, 6, 8}}, // Aire+Fuego
    {"4.4",     {6, 7, 5, 10}}, // Aire+Aire
    // Etapa 3 (ramas de "1.1")
    {"1.1.1",   {12, 5, 8, 8}},
    {"1.1.2",   {10, 8, 7, 8}},
    // Etapa 3 (ramas de "2.2")
    {"2.2.1",   {8, 11, 8, 8}},
    {"2.2.2",   {5, 13, 8, 9}},
    // Etapa 3 (ramas de "3.3")
    {"3.3.1",   {10, 8, 11, 6}},
    {"3.3.2",   {8, 10, 11, 6}},
    {"3.3.3",   {9, 8, 13, 5}},
    {"3.3.4",   {8, 8, 11, 8}},
    // Etapa 3 (ramas de "4.4")
    {"4.4.1",   {9, 8, 6, 11}},
    {"4.4.4",   {6, 8, 5, 13}},
};

static char G_CURRENT_DIYMON_CODE[16] = "0";
// Búfer estático para construir códigos de evolución candidatos.
static char G_EVO_CODE_BUFFER[16];


// ----- Funciones para interactuar con la memoria FLASH (NVS) -----

static void diymon_core_save_state(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("diymon_storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) abriendo NVS para escribir!", esp_err_to_name(err));
        return;
    }
    err = nvs_set_str(nvs_handle, "evo_code", G_CURRENT_DIYMON_CODE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) guardando 'evo_code' en NVS!", esp_err_to_name(err));
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) haciendo commit en NVS!", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Estado guardado en memoria flash: %s", G_CURRENT_DIYMON_CODE);
}

static void diymon_core_load_state(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("diymon_storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS: No se encontró partición, empezando de cero.");
        strcpy(G_CURRENT_DIYMON_CODE, "0"); // Asegurar estado inicial si no hay NVS
        return;
    }
    size_t required_size = sizeof(G_CURRENT_DIYMON_CODE);
    err = nvs_get_str(nvs_handle, "evo_code", G_CURRENT_DIYMON_CODE, &required_size);
    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Estado cargado de memoria flash: %s", G_CURRENT_DIYMON_CODE);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "NVS: Clave 'evo_code' no encontrada. Es la primera ejecución.");
            strcpy(G_CURRENT_DIYMON_CODE, "0"); // Estado inicial
            break;
        default:
            ESP_LOGE(TAG, "Error (%s) cargando 'evo_code' desde NVS!", esp_err_to_name(err));
    }
    nvs_close(nvs_handle);
}

// ----- Funciones públicas -----

void diymon_evolution_init(void) {
    ESP_LOGI(TAG, "Motor de evolución inicializado.");
    diymon_core_load_state();
}

void diymon_set_current_code(const char* new_code) {
    strncpy(G_CURRENT_DIYMON_CODE, new_code, sizeof(G_CURRENT_DIYMON_CODE) - 1);
    diymon_core_save_state();
}

const diymon_stats_t* diymon_get_stats_for_code(const char* evo_code) {
    for (int i = 0; i < sizeof(G_MASTER_TABLE) / sizeof(G_MASTER_TABLE[0]); ++i) {
        if (strcmp(G_MASTER_TABLE[i].evo_code, evo_code) == 0) {
            return &G_MASTER_TABLE[i].stats;
        }
    }
    return NULL;
}

const char* diymon_get_current_code(void) {
    return G_CURRENT_DIYMON_CODE;
}

const char* diymon_get_next_evolution_in_sequence(const char* current_code) {
    if (strcmp(current_code, "0") == 0) return "1";
    if (strcmp(current_code, "1") == 0) return "1.1";
    if (strcmp(current_code, "1.1") == 0) return "1.1.1";
    return NULL;
}

const char* diymon_get_previous_evolution_in_sequence(const char* current_code) {
    if (strcmp(current_code, "0") == 0) {
        return NULL; // Ya está en la forma base.
    }

    const char* last_dot = strrchr(current_code, '.');
    
    // Si no hay punto (ej: "1", "2", "3", "4"), la involución es hacia "0".
    if (!last_dot) {
        return "0";
    }

    // Si hay un punto, se trunca el código para obtener el padre.
    size_t parent_len = last_dot - current_code;
    strncpy(G_EVO_CODE_BUFFER, current_code, parent_len);
    G_EVO_CODE_BUFFER[parent_len] = '\0';

    // Se busca el código padre en la tabla para devolver un puntero válido.
    for (int i = 0; i < sizeof(G_MASTER_TABLE) / sizeof(G_MASTER_TABLE[0]); ++i) {
        if (strcmp(G_MASTER_TABLE[i].evo_code, G_EVO_CODE_BUFFER) == 0) {
            return G_MASTER_TABLE[i].evo_code;
        }
    }

    return NULL; // El padre no existe en la tabla.
}

const char* diymon_get_branched_evolution(const char* current_code, int branch_id) {
    // Si el código actual es "0", la evolución es directamente a la rama, sin prefijo.
    if (strcmp(current_code, "0") == 0) {
        snprintf(G_EVO_CODE_BUFFER, sizeof(G_EVO_CODE_BUFFER), "%d", branch_id);
    } else {
        // Para cualquier otro estado, la evolución es una sub-rama.
        snprintf(G_EVO_CODE_BUFFER, sizeof(G_EVO_CODE_BUFFER), "%s.%d", current_code, branch_id);
    }

    // Buscar si el código generado existe en la tabla de evoluciones
    for (int i = 0; i < sizeof(G_MASTER_TABLE) / sizeof(G_MASTER_TABLE[0]); ++i) {
        if (strcmp(G_MASTER_TABLE[i].evo_code, G_EVO_CODE_BUFFER) == 0) {
            return G_MASTER_TABLE[i].evo_code; // Devuelve el puntero de la tabla
        }
    }

    return NULL; // La evolución no es válida
}

void diymon_evolution_reset_state(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("diymon_storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) abriendo NVS para borrar estado de evolución.", esp_err_to_name(err));
        return;
    }
    err = nvs_erase_key(nvs_handle, "evo_code");
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Clave 'evo_code' borrada de NVS.");
    } else {
        ESP_LOGE(TAG, "Error al borrar 'evo_code': %s", esp_err_to_name(err));
    }
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}
