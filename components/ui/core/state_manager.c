/* Fichero: components/ui/core/state_manager.c */
/* Descripción: Diagnóstico: La lógica de inactividad era monolítica. Solución: Se implementa la nueva lógica de inactividad de tres fases. A los 30s, atenúa la pantalla al 5% sin guardar en NVS. Al recibir un toque en estado atenuado, restaura el brillo guardado por el usuario (leído de NVS). A los 60s, apaga completamente la pantalla. Esto crea un sistema de ahorro de energía modular y robusto. */
/* Último cambio: 21/08/2025 - 19:23 */
#include "state_manager.h"
#include "screen_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "UI_STATE_MANAGER";

// --- Tipos y variables de estado internas ---
typedef enum {
    WAKE_STATE_OFF,
    WAKE_STATE_PRIMED,
} wake_up_state_t;

static bool s_is_dimmed = false;
static int s_user_brightness = 100;
static bool s_user_brightness_known = false;
static wake_up_state_t s_wake_state = WAKE_STATE_OFF;
static uint8_t s_wake_click_count = 0;
static lv_timer_t *s_double_click_timer = NULL;
static lv_timer_t *s_wake_prime_timer = NULL;
static lv_timer_t *s_inactivity_timer = NULL;
static bool s_is_paused = false; // Flag para pausar el gestor

// --- Funciones de ayuda y callbacks ---

static void read_user_brightness_from_nvs(void) {
    if (s_user_brightness_known) return;
    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        if (nvs_get_i32(nvs_handle, "brightness", (int32_t*)&s_user_brightness) == ESP_OK) {
            ESP_LOGI(TAG, "Brillo de usuario cargado desde NVS: %d%%", s_user_brightness);
            s_user_brightness_known = true;
        } else {
            ESP_LOGW(TAG, "No se encontró brillo en NVS, usando 100%% por defecto.");
            s_user_brightness = 100;
        }
        nvs_close(nvs_handle);
    }
}

static void double_click_timer_cb(lv_timer_t * timer) {
    s_wake_click_count = 0;
    s_double_click_timer = NULL;
}

static void wake_prime_timer_cb(lv_timer_t * timer) {
    s_wake_state = WAKE_STATE_OFF;
    s_wake_click_count = 0;
    s_wake_prime_timer = NULL;
}

static void screen_touch_event_cb(lv_event_t * e) {
    if (s_is_paused) return; // Ignorar eventos si está pausado
    lv_event_code_t code = lv_event_get_code(e);
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    
    lv_display_trigger_activity(disp);

    if (code == LV_EVENT_PRESSED && s_is_dimmed && !screen_manager_is_off()) {
        ESP_LOGD(TAG, "Toque en pantalla atenuada. Restaurando brillo a %d%%", s_user_brightness);
        screen_manager_set_brightness(s_user_brightness, false); // No guardar, es solo restauración
        s_is_dimmed = false;
        return;
    }

    if (code == LV_EVENT_CLICKED && screen_manager_is_off()) {
        if (s_wake_state == WAKE_STATE_OFF) {
            s_wake_click_count++;
            if (s_wake_click_count == 1) {
                s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL);
                lv_timer_set_repeat_count(s_double_click_timer, 1);
            } else if (s_wake_click_count == 2) {
                if (s_double_click_timer) lv_timer_del(s_double_click_timer);
                s_double_click_timer = NULL;
                s_wake_state = WAKE_STATE_PRIMED;
                s_wake_click_count = 0;
                s_wake_prime_timer = lv_timer_create(wake_prime_timer_cb, 3000, NULL);
                lv_timer_set_repeat_count(s_wake_prime_timer, 1);
            }
        } else if (s_wake_state == WAKE_STATE_PRIMED) {
            s_wake_click_count++;
            if (s_wake_click_count == 1) {
                s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL);
                lv_timer_set_repeat_count(s_double_click_timer, 1);
            } else if (s_wake_click_count >= 2) {
                if (s_double_click_timer) lv_timer_del(s_double_click_timer);
                if (s_wake_prime_timer) lv_timer_del(s_wake_prime_timer);
                s_double_click_timer = NULL;
                s_wake_prime_timer = NULL;
                screen_manager_turn_on();
                s_is_dimmed = false;
                s_wake_state = WAKE_STATE_OFF;
                s_wake_click_count = 0;
            }
        }
    }
}

static void inactivity_timer_cb(lv_timer_t * timer) {
    if (s_is_paused) return;

    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    
    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();

    if (!is_off && inactivity_ms > 60000) {
        ESP_LOGI(TAG, "Inactividad > 60s. Apagando pantalla.");
        screen_manager_turn_off();
        s_is_dimmed = false; 
    } else if (!is_off && !s_is_dimmed && inactivity_ms > 30000) {
        ESP_LOGI(TAG, "Inactividad > 30s. Atenuando pantalla al 5%%.");
        read_user_brightness_from_nvs(); // Asegurarse de tener el brillo del usuario
        screen_manager_set_brightness(5, false); // Atenuar sin guardar
        s_is_dimmed = true;
    }
}

// --- Funciones de inicialización y control públicas ---

void state_manager_init(void) {
    read_user_brightness_from_nvs(); // Carga inicial
    s_inactivity_timer = lv_timer_create(inactivity_timer_cb, 5000, NULL);
    lv_obj_t * scr = lv_screen_active();
    if (scr) {
        lv_obj_add_event_cb(scr, screen_touch_event_cb, LV_EVENT_ALL, NULL);
    }
    ESP_LOGI(TAG, "Gestor de estado de UI inicializado. Manejadores de inactividad activos.");
}

void state_manager_pause(void) {
    if (!s_is_paused) {
        s_is_paused = true;
        ESP_LOGI(TAG, "Gestor de estado PAUSADO. La pantalla no se atenuará ni apagará por inactividad.");
    }
}

void state_manager_resume(void) {
    if (s_is_paused) {
        s_is_paused = false;
        // Resetea el contador de inactividad para dar al usuario tiempo completo antes del próximo evento.
        lv_disp_t * disp = lv_display_get_default();
        if (disp) {
            lv_display_trigger_activity(disp);
        }
        ESP_LOGI(TAG, "Gestor de estado REANUDADO.");
    }
}

void state_manager_destroy(void) {
    if (s_inactivity_timer) {
        lv_timer_del(s_inactivity_timer);
        s_inactivity_timer = NULL;
    }
    if (s_double_click_timer) {
        lv_timer_del(s_double_click_timer);
        s_double_click_timer = NULL;
    }
    if (s_wake_prime_timer) {
        lv_timer_del(s_wake_prime_timer);
        s_wake_prime_timer = NULL;
    }
    ESP_LOGI(TAG, "Gestor de estado DESTRUIDO. Todos los temporizadores liberados.");
}
