#include <stdlib.h>
#include <zephyr/kernel.h>
#include "animation.h"

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#endif

LV_IMG_DECLARE(crystal_01);
LV_IMG_DECLARE(crystal_02);
LV_IMG_DECLARE(crystal_03);
LV_IMG_DECLARE(crystal_04);
LV_IMG_DECLARE(crystal_05);
LV_IMG_DECLARE(crystal_06);
LV_IMG_DECLARE(crystal_07);
LV_IMG_DECLARE(crystal_08);
LV_IMG_DECLARE(crystal_09);
LV_IMG_DECLARE(crystal_10);
LV_IMG_DECLARE(crystal_11);
LV_IMG_DECLARE(crystal_12);
LV_IMG_DECLARE(crystal_13);
LV_IMG_DECLARE(crystal_14);
LV_IMG_DECLARE(crystal_15);
LV_IMG_DECLARE(crystal_16);

const lv_img_dsc_t *anim_imgs[] = {
    &crystal_01, &crystal_02, &crystal_03, &crystal_04, &crystal_05, &crystal_06,
    &crystal_07, &crystal_08, &crystal_09, &crystal_10, &crystal_11, &crystal_12,
    &crystal_13, &crystal_14, &crystal_15, &crystal_16,
};

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
#define ANIM_TIMEOUT_MS (CONFIG_NICE_VIEW_GEM_ANIM_TIMEOUT_SECONDS * 1000)
static uint32_t last_anim_activity_time;
static bool anim_stopped = false;
static lv_obj_t *animation_obj = NULL;
#endif

void draw_animation(lv_obj_t *canvas) {
#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
    lv_obj_t *art = lv_animimg_create(canvas);
    lv_obj_center(art);

    lv_animimg_set_src(art, (const void **)anim_imgs, 16);
    lv_animimg_set_duration(art, CONFIG_NICE_VIEW_GEM_ANIMATION_MS);
    lv_animimg_set_repeat_count(art, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(art);

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
    animation_obj = art;
    last_anim_activity_time = k_uptime_get_32();
    anim_stopped = false;
#endif
#else
    lv_obj_t *art = lv_img_create(canvas);

    int length = sizeof(anim_imgs) / sizeof(anim_imgs[0]);
    srand(k_uptime_get_32());
    int random_index = rand() % length;

    lv_img_set_src(art, anim_imgs[random_index]);
#endif

    lv_obj_align(art, LV_ALIGN_TOP_LEFT, 36, 0);
}

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
void animation_reset_timeout(void) {
    last_anim_activity_time = k_uptime_get_32();
    if (anim_stopped && animation_obj != NULL) {
        anim_stopped = false;
// Re-enable animation
#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
        lv_animimg_start(animation_obj);
#endif
    }
}

static int position_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (ev && ev->state) { // Only react to key press (not release)
        animation_reset_timeout();
    }
    return 0;
}

ZMK_LISTENER(animation_position_state_listener, position_state_changed_listener);
ZMK_SUBSCRIPTION(animation_position_state_listener, zmk_position_state_changed);
#endif

void animation_update(void) {
#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT)
    if (animation_obj == NULL) {
        return;
    }

    uint32_t now = k_uptime_get_32();
    if (!anim_stopped && (now - last_anim_activity_time > ANIM_TIMEOUT_MS)) {
        anim_stopped = true;
// Stop animation to preserve battery
#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
        lv_animimg_stop(animation_obj);
#endif
    }
#endif
}