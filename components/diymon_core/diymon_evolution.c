// =============================================================================
//  diymon_evolution.c (Versión con Memoria Persistente)
// =============================================================================

#include "diymon_evolution.h"
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h" // Necesario para la memoria no volátil
#include "nvs.h"       // Herramientas para leer/escribir en NVS

static const char* TAG = "DIYMON_CORE";

// --- LA TABLA MAESTRA DE ESTADÍSTICAS ---
static const struct {
    const char* evo_code;
    const diymon_stats_t stats;
} G_MASTER_TABLE[] = {
    {"0",     {5, 5, 5, 5}},
    {"1",     {7, 5, 6, 6}},
    {"1.1",   {9, 5, 7, 7}},
    {"1.1.1", {11, 5, 8, 8}},
};

// Variable estática que guarda el estado EN RAM del DIYMON actual.
static char G_CURRENT_DIYMON_CODE[16] = "0";


// ----- [NUEVO] Funciones para interactuar con la memoria FLASH (NVS) -----

/**
 * @brief Guarda el código evolutivo actual en la partición NVS.
 * Esta función es privada al componente.
 */
static void diymon_core_save_state(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("diymon_storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) abriendo NVS para escribir!", esp_err_to_name(err));
        return;
    }

    // Guardamos el string G_CURRENT_DIYMON_CODE bajo la clave "evo_code"
    err = nvs_set_str(nvs_handle, "evo_code", G_CURRENT_DIYMON_CODE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) guardando 'evo_code' en NVS!", esp_err_to_name(err));
    }

    // Confirmamos los cambios para que se escriban físicamente en la flash
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) haciendo commit en NVS!", esp_err_to_name(err));
    }
    
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Estado guardado en memoria flash: %s", G_CURRENT_DIYMON_CODE);
}

/**
 * @brief Carga el código evolutivo desde la NVS a la variable en RAM.
 * Esta función es privada al componente.
 */
static void diymon_core_load_state(void) {
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("diymon_storage", NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "NVS: No se encontró partición, empezando de cero.");
        return; // No hay nada que cargar, G_CURRENT_DIYMON_CODE se queda en "0"
    }

    size_t required_size = sizeof(G_CURRENT_DIYMON_CODE);
    err = nvs_get_str(nvs_handle, "evo_code", G_CURRENT_DIYMON_CODE, &required_size);

    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Estado cargado de memoria flash: %s", G_CURRENT_DIYMON_CODE);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG, "NVS: Clave 'evo_code' no encontrada. Es la primera ejecución.");
            break;
        default:
            ESP_LOGE(TAG, "Error (%s) cargando 'evo_code' desde NVS!", esp_err_to_name(err));
    }
    
    nvs_close(nvs_handle);
}


// ----- [MODIFICADO] Funciones públicas que ahora usan la memoria -----

void diymon_evolution_init(void) {
    ESP_LOGI(TAG, "Motor de evolución inicializado.");
    // Al iniciar el cerebro, intentamos cargar el estado que teníamos guardado.
    diymon_core_load_state();
}

void diymon_set_current_code(const char* new_code) {
    // 1. Actualizamos la variable en RAM
    strncpy(G_CURRENT_DIYMON_CODE, new_code, sizeof(G_CURRENT_DIYMON_CODE) - 1);
    // 2. Guardamos inmediatamente el nuevo estado en la memoria flash
    diymon_core_save_state();
}


// ----- Funciones sin cambios en su lógica -----

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