/* Fichero: components/ui/ui_state_manager.c */
/* Descripción: Diagnóstico: Error de compilación 'too few arguments' en las llamadas a 'screen_manager_set_brightness'. Causa Raíz: La firma de la función fue actualizada para requerir un segundo argumento booleano ('save_to_nvs'), pero las llamadas dentro de este fichero no se actualizaron. Solución: Se han corregido ambas llamadas para pasar 'false' como segundo argumento, ya que tanto el atenuado temporal como la restauración del brillo no deben modificar la preferencia guardada por el usuario, resolviendo así el error de compilación y asegurando la lógica correcta. */
/* Último cambio: 21/08/2025 - 19:29 */
#include "ui_state_manager.h"
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

// --- Funciones de ayuda y callbacks ---

static void read_user_brightness_from_nvs(void) {
    if (s_user_brightness_known) return;
    nvs_handle_t nvs_handle;
    if (nvs_open("storage", NVS_READONLY, &nvs_handle) == ESP_OK) {
        nvs_get_i32(nvs_handle, "brightness", (int32_t*)&s_user_brightness);
        nvs_close(nvs_handle);
        s_user_brightness_known = true;
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
    lv_event_code_t code = lv_event_get_code(e);
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    
    lv_display_trigger_activity(disp);

    if (code == LV_EVENT_PRESSED && s_is_dimmed && !screen_manager_is_off()) {
        read_user_brightness_from_nvs();
        screen_manager_set_brightness(s_user_brightness, false);
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
    lv_disp_t * disp = lv_display_get_default();
    if (!disp) return;
    
    uint32_t inactivity_ms = lv_display_get_inactive_time(disp);
    bool is_off = screen_manager_is_off();

    if (inactivity_ms < 30000 && s_is_dimmed) {
        s_is_dimmed = false;
    }

    if (!is_off && inactivity_ms > 60000) {
        screen_manager_turn_off();
        s_is_dimmed = false;
    } else if (!is_off && !s_is_dimmed && inactivity_ms > 30000) {
        read_user_brightness_from_nvs();
        screen_manager_set_brightness(10, false);
        s_is_dimmed = true;
    }
}

// --- Función de inicialización pública ---
void ui_state_manager_init(void) {
    lv_timer_create(inactivity_timer_cb, 5000, NULL);
    lv_obj_t * scr = lv_screen_active();
    if (scr) {
        lv_obj_add_event_cb(scr, screen_touch_event_cb, LV_EVENT_ALL, NULL);
    }
    ESP_LOGI(TAG, "Gestor de estado de UI inicializado. Manejadores de inactividad activos.");
}
