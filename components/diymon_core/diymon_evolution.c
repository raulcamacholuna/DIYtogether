/*
 * Fichero: ./components/diymon_core/diymon_evolution.c
 * Fecha: 12/08/2025 - 10:40
 * Último cambio: Implementada la función 'diymon_get_previous_evolution_in_sequence'.
 * Descripción: Añadida la lógica para obtener la evolución anterior en la secuencia, permitiendo la funcionalidad de "bajar nivel".
 */

#include "diymon_evolution.h"
#include <string.h>
#include <stdlib.h> // <-- Incluido para usar atoi()
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

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

static char G_CURRENT_DIYMON_CODE[16] = "0";

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
    if (strcmp(current_code, "1.1.1") == 0) return "1.1";
    if (strcmp(current_code, "1.1") == 0) return "1";
    if (strcmp(current_code, "1") == 0) return "0";
    return NULL;
}

/**
 * @brief [IMPLEMENTACIÓN AÑADIDA] Convierte el código de evolución a un ID numérico.
 */
int diymon_core_get_evolution_id(void) {
    const char *code_str = diymon_get_current_code();
    char numeric_str[16];
    int j = 0;

    // Recorremos el string original y copiamos solo los dígitos
    for (int i = 0; code_str[i] != '\0' && j < sizeof(numeric_str) - 1; i++) {
        if (code_str[i] >= '0' && code_str[i] <= '9') {
            numeric_str[j++] = code_str[i];
        }
    }
    numeric_str[j] = '\0'; // Terminamos el nuevo string

    // Convertimos el string de solo dígitos a un entero
    return atoi(numeric_str);
}