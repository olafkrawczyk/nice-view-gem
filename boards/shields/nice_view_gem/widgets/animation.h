#pragma once

#include <lvgl.h>
#include "util.h"
#include "screen_peripheral.h"

void draw_animation(lv_obj_t *canvas);

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
void animation_reset_timeout(void);
void animation_update(void);
#endif
