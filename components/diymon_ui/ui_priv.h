#ifndef UI_PRIV_H
#define UI_PRIV_H

#include "lvgl.h"

// Variable global para nuestro objeto GIF. La creamos en screens.c
// y la usamos en ui.c y actions.c para controlarla.
extern lv_obj_t *g_diymon_gif_obj;

#endif // UI_PRIV_H