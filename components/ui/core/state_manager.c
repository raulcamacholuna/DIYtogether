/* Fichero: components/ui/core/state_manager.c */
/* Descripción: Diagnóstico: El temporizador de inactividad se reseteaba con cada evento de LVGL (LV_EVENT_ALL), incluyendo eventos continuos como el arrastre, impidiendo que el contador de inactividad avanzara. Solución Definitiva: Se ha refactorizado screen_touch_event_cb para que solo considere LV_EVENT_PRESSED como una interacción deliberada del usuario. Únicamente este evento reiniciará ahora el contador de inactividad, cancelará el temporizador de sueño y restaurará el brillo. Esto asegura que un toque o un arrastre mantengan la pantalla activa, pero el contador de inactividad progrese correctamente una vez finalizada la interacción. */
/* Último cambio: 21/08/2025 - 22:59 */
#include "state_manager.h"
#include "screen_manager.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char *TAG = "UI_STATE_MANAGER";

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
static lv_timer_t *s_sleep_delay_timer = NULL;
static bool s_is_paused = false;

static void sleep_delay_timer_cb(lv_timer_t *timer) {
    ESP_LOGI(TAG, "[TIMER_SLEEP_DELAY] Callback ejecutado. Entrando en Light Sleep AHORA.");
    screen_manager_enter_light_sleep();
    s_sleep_delay_timer = NULL;
}

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
}

static void wake_prime_timer_cb(lv_timer_t * timer) {
    s_wake_state = WAKE_STATE_OFF;
    s_wake_click_count = 0;
    s_wake_prime_timer = NULL;
}

static void screen_touch_event_cb(lv_event_t * e) {
    if (s_is_paused) return;
    
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    
    lv_event_code_t code = lv_event_get_code(e);

    // La actividad del usuario se define como el inicio de una pulsación.
    // Esto evita que eventos continuos (como DRAG) reseteen el temporizador constantemente.
    if (code == LV_EVENT_PRESSED) {
        ESP_LOGI(TAG, "[INTERACCION] Evento PRESS detectado. Reseteando inactividad.");
        lv_display_trigger_activity(disp);

        // Si la pantalla estaba atenuada, el primer toque la restaura.
        if (s_is_dimmed && !screen_manager_is_off()) {
            ESP_LOGI(TAG, "[INTERACCION] Pantalla atenuada. Restaurando brillo a %d%%.", s_user_brightness);
            screen_manager_set_brightness(s_user_brightness, false);
            s_is_dimmed = false;
        }

        // Si estábamos a punto de dormir, cancelamos el proceso.
        if (s_sleep_delay_timer) {
            ESP_LOGI(TAG, "[INTERACCION] Cancelando entrada programada en Light Sleep.");
            lv_timer_del(s_sleep_delay_timer);
            s_sleep_delay_timer = NULL;
        }
    } 
    // La lógica de despertar con doble click solo aplica si la pantalla ya está apagada.
    else if (code == LV_EVENT_CLICKED && screen_manager_is_off()) {
        ESP_LOGI(TAG, "[INTERACCION] Click en pantalla apagada. Lógica de despertar iniciada.");
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
                ESP_LOGI(TAG, "[INTERACCION] Secuencia de despertar completada. Encendiendo pantalla.");
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

    if (s_sleep_delay_timer) {
        ESP_LOGI(TAG, "[TIMER_INACTIVITY] Tick ignorado: Ya está programado el Light Sleep.");
        return;
    }
    
    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();

    ESP_LOGD(TAG, "[TIMER_INACTIVITY] Tick: Inactividad=%ums, PantallaApagada=%d, Atenuada=%d", (unsigned int)inactivity_ms, is_off, s_is_dimmed);

    if (!is_off && inactivity_ms >= 60000) {
        ESP_LOGI(TAG, "[TIMER_INACTIVITY] ACCION: Inactividad >= 60s. Apagando pantalla y programando Light Sleep en 5s.");
        screen_manager_turn_off();
        s_is_dimmed = false; 
        s_sleep_delay_timer = lv_timer_create(sleep_delay_timer_cb, 5000, NULL);
        lv_timer_set_repeat_count(s_sleep_delay_timer, 1);
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
    if (s_sleep_delay_timer) lv_timer_del(s_sleep_delay_timer);
    s_inactivity_timer = s_double_click_timer = s_wake_prime_timer = s_sleep_delay_timer = NULL;
    ESP_LOGI(TAG, "Gestor de estado DESTRUIDO.");
}
