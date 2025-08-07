/*
 * Archivo: screens.c
 * Versión: 10.0 (FINAL - Carga 100% dinámica de imágenes desde la SD)
 */
#include "screens.h"
#include "actions.h"
#include "styles.h"
#include "lvgl.h"
#include "ui_priv.h"
#include "diymon_evolution.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

// [CAMBIO CLAVE] ¡Ya no necesitamos declarar imágenes desde archivos .c!
// LV_IMAGE_DECLARE(eat);     // <-- ELIMINADO
// LV_IMAGE_DECLARE(train);   // <-- ELIMINADO
// LV_IMAGE_DECLARE(attack);  // <-- ELIMINADO

// Definición de las variables globales para los objetos de la UI
objects_t objects; 
lv_obj_t *g_diymon_gif_obj = NULL;
lv_obj_t *g_background_obj = NULL;

static const char *TAG = "DIYMON_SCREENS";

// Función para actualizar el fondo dinámicamente desde la SD
void ui_update_diymon_background(void) {
    if (!g_background_obj) {
        ESP_LOGE(TAG, "El objeto de fondo (g_background_obj) no existe!");
        return;
    }
    const char* evo_code = diymon_get_current_code();
    static char bg_path[128];

    // [CAMBIO CLAVE] Ahora cargamos un archivo .png directamente, gracias al decodificador.
    snprintf(bg_path, sizeof(bg_path), "S:/sdcard/diymon/%s/background.png", evo_code);
    ESP_LOGI(TAG, "Cargando fondo: %s", bg_path);
    lv_image_set_src(g_background_obj, bg_path);
}

void create_screen_main() {
    lv_obj_t *screen = lv_obj_create(NULL);
    objects.main = screen;
    lv_obj_set_size(screen, 170, 320);
    lv_obj_center(screen);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    // Creamos el objeto de fondo (la ruta se establecerá dinámicamente)
    g_background_obj = lv_image_create(screen);
    lv_obj_align(g_background_obj, LV_ALIGN_CENTER, 0, 0);

    // Creamos el objeto GIF (la ruta se establecerá dinámicamente)
    g_diymon_gif_obj = lv_gif_create(screen);
    objects.idle = g_diymon_gif_obj;
    lv_obj_align(g_diymon_gif_obj, LV_ALIGN_CENTER, 0, 30);

    // Creamos el contenedor de botones
    lv_obj_t *btn_container = lv_obj_create(screen);
    lv_obj_set_size(btn_container, lv_pct(100), 80);
    lv_obj_align(btn_container, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_container, 0, 0);

    {
        // Botón Comer
        lv_obj_t *btn_comer = lv_button_create(btn_container);
        objects.comer = btn_comer;
        lv_obj_set_size(btn_comer, 60, 60);
        lv_obj_set_style_bg_opa(btn_comer, LV_OPA_TRANSP, 0);
        lv_obj_t *img_comer = lv_image_create(btn_comer);
        // [CAMBIO CLAVE] Cargamos la imagen del botón directamente desde un archivo .png en la SD
        lv_image_set_src(img_comer, "S:/sdcard/buttons/actions/eat.png");
        lv_obj_center(img_comer);

        // Botón Entrenar
        lv_obj_t *btn_pesas = lv_button_create(btn_container);
        objects.pesas = btn_pesas;
        lv_obj_set_size(btn_pesas, 60, 60);
        lv_obj_set_style_bg_opa(btn_pesas, LV_OPA_TRANSP, 0);
        lv_obj_t *img_pesas = lv_image_create(btn_pesas);
        // [CAMBIO CLAVE] Cargamos la imagen del botón directamente desde un archivo .png en la SD
        lv_image_set_src(img_pesas, "S:/sdcard/buttons/actions/train.png");
        lv_obj_center(img_pesas);
        
        // Botón Atacar
        lv_obj_t *btn_atacar = lv_button_create(btn_container);
        objects.atacar = btn_atacar;
        lv_obj_set_size(btn_atacar, 60, 60);
        lv_obj_set_style_bg_opa(btn_atacar, LV_OPA_TRANSP, 0);
        lv_obj_t *img_atacar = lv_image_create(btn_atacar);
        // [CAMBIO CLAVE] Cargamos la imagen del botón directamente desde un archivo .png en la SD
        lv_image_set_src(img_atacar, "S:/sdcard/buttons/actions/attack.png");
        lv_obj_center(img_atacar);
    }
}

void delete_screen_main() {
    if(objects.main) {
        lv_obj_del(objects.main);
    }
    memset(&objects, 0, sizeof(objects_t));
    g_diymon_gif_obj = NULL;
    g_background_obj = NULL;
}

void create_screens() {
    create_screen_main();
}