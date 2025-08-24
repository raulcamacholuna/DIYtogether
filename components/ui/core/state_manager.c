/* Fichero: components/ui/core/state_manager.c */
/* Descripción: Diagnóstico: El despertar de la pantalla con un solo toque era demasiado sensible. Causa Raíz: La lógica de despertar anterior fue simplificada en exceso. Solución Definitiva: Se ha reintroducido una máquina de estados robusta para el despertar. Ahora se requiere una secuencia de 'doble-doble toque' para encender la pantalla. El primer doble toque 'prepara' el sistema por 3 segundos, y un segundo doble toque dentro de ese periodo completa el despertar. Se utilizan dos temporizadores de LVGL para gestionar las ventanas de tiempo entre clics y para el estado 'preparado', garantizando que solo una secuencia deliberada reactive la pantalla y evitando encendidos accidentales.
// Último cambio: 22/08/2025 - 08:13
*/
#include "state_manager.h"
#include "screen_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "ui_idle_animation.h"

static const char *TAG = "UI_STATE_MANAGER";

typedef enum {
    WAKE_STATE_OFF,
    WAKE_STATE_PRIMED,
} wake_up_state_t;

static bool s_is_dimmed = false;
static int s_user_brightness = 100;
static bool s_user_brightness_known = false;
static lv_timer_t *s_inactivity_timer = NULL;
static bool s_is_paused = false;

static wake_up_state_t s_wake_state = WAKE_STATE_OFF;
static uint8_t s_wake_click_count = 0;
static lv_timer_t *s_double_click_timer = NULL;
static lv_timer_t *s_wake_prime_timer = NULL;

static void read_user_brightness_from_nvs(void) {
    if (s_user_brightness_known) return;
    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        if (nvs_get_i32(nvs_handle, "brightness", (int32_t*)&s_user_brightness) == ESP_OK) {
            s_user_brightness_known = true;
        } else {
            s_user_brightness = 100;
        }
        nvs_close(nvs_handle);
    }
}

static void double_click_timer_cb(lv_timer_t * timer) {
    s_wake_click_count = 0;
    s_double_click_timer = NULL;
    ESP_LOGD(TAG, "[WAKEUP] Timer de doble click expirado. Reseteando contador de clicks.");
}

static void wake_prime_timer_cb(lv_timer_t * timer) {
    s_wake_state = WAKE_STATE_OFF;
    s_wake_click_count = 0;
    s_wake_prime_timer = NULL;
    ESP_LOGD(TAG, "[WAKEUP] Timer de estado 'primed' expirado. Reseteando máquina de estados.");
}

static void screen_touch_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        if (screen_manager_is_off()) {
            s_wake_click_count++;
            ESP_LOGI(TAG, "[WAKEUP] Click %d en pantalla apagada.", s_wake_click_count);

            if (s_wake_state == WAKE_STATE_OFF) {
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
                    ESP_LOGI(TAG, "[WAKEUP] Estado 'PRIMED'. Esperando segundo doble click.");
                }
            } else if (s_wake_state == WAKE_STATE_PRIMED) {
                 if (s_wake_click_count == 1) {
                    s_double_click_timer = lv_timer_create(double_click_timer_cb, 500, NULL);
                    lv_timer_set_repeat_count(s_double_click_timer, 1);
                } else if (s_wake_click_count >= 2) {
                    if (s_double_click_timer) lv_timer_del(s_double_click_timer);
                    if (s_wake_prime_timer) lv_timer_del(s_wake_prime_timer);
                    s_double_click_timer = NULL;
                    s_wake_prime_timer = NULL;
                    ESP_LOGI(TAG, "[WAKEUP] Secuencia completada. Despertando UI.");
                    screen_manager_turn_on();
                    ui_idle_animation_resume();
                    state_manager_resume();
                    s_wake_state = WAKE_STATE_OFF;
                    s_wake_click_count = 0;
                }
            }
            return; 
        }

        if (s_is_paused) return;
        
        lv_disp_t * disp = lv_display_get_default();
        if (!disp) return;

        ESP_LOGD(TAG, "[INTERACCION] Evento PRESS detectado. Reseteando inactividad.");
        lv_display_trigger_activity(disp);

        if (s_is_dimmed) {
            ESP_LOGI(TAG, "[INTERACCION] Pantalla atenuada. Restaurando brillo a %d%%.", s_user_brightness);
            screen_manager_set_brightness(s_user_brightness, false);
            s_is_dimmed = false;
        }
    }
}

static void inactivity_timer_cb(lv_timer_t * timer) {
    if (s_is_paused) return;

    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;

    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();

    if (!is_off && inactivity_ms >= 60000) {
        ESP_LOGI(TAG, "[TIMER_INACTIVITY] ACCION: Inactividad >= 60s. Entrando en sueño de UI.");
        state_manager_pause();
        ui_idle_animation_pause();
        screen_manager_turn_off();
        s_is_dimmed = false; 
    } else if (!is_off && !s_is_dimmed && inactivity_ms >= 30000) {
        ESP_LOGI(TAG, "[TIMER_INACTIVITY] ACCION: Inactividad >= 30s. Atenuando pantalla al 5%%.");
        read_user_brightness_from_nvs();
        screen_manager_set_brightness(5, false);
        s_is_dimmed = true;
    }
}

void state_manager_init(void) {
    read_user_brightness_from_nvs();
    s_inactivity_timer = lv_timer_create(inactivity_timer_cb, 5000, NULL);
    lv_obj_t * scr = lv_screen_active();
    if (scr) {
        lv_obj_add_event_cb(scr, screen_touch_event_cb, LV_EVENT_ALL, NULL);
    }
    ESP_LOGI(TAG, "Gestor de estado de UI inicializado.");
}

void state_manager_pause(void) {
    if (!s_is_paused) {
        s_is_paused = true;
        ESP_LOGI(TAG, "Gestor de estado PAUSADO.");
    }
}

void state_manager_resume(void) {
    if (s_is_paused) {
        s_is_paused = false;
        lv_disp_t * disp = lv_display_get_default();
        if (disp) {
            lv_display_trigger_activity(disp);
        }
        ESP_LOGI(TAG, "Gestor de estado REANUDADO.");
    }
}

void state_manager_destroy(void) {
    if (s_inactivity_timer) lv_timer_del(s_inactivity_timer);
    if (s_double_click_timer) lv_timer_del(s_double_click_timer);
    if (s_wake_prime_timer) lv_timer_del(s_wake_prime_timer);
    s_inactivity_timer = s_double_click_timer = s_wake_prime_timer = NULL;
    ESP_LOGI(TAG, "Gestor de estado DESTRUIDO.");
}
