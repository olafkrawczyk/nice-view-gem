#include <stdlib.h>
#include <zephyr/kernel.h>
#include "animation.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
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

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
#define ANIM_TIMEOUT_MS (CONFIG_NICE_VIEW_GEM_ANIM_TIMEOUT_SECONDS * 1000)
#define ANIM_CHECK_INTERVAL_MS 1000 // Check for animation timeout every second
static uint32_t last_anim_activity_time;
static bool anim_stopped = false;
static lv_obj_t *animation_obj = NULL;
static struct k_work_delayable animation_timer;

static void animation_timer_handler(struct k_work *work) {
    animation_update();
    // Reschedule the timer
    k_work_schedule(&animation_timer, K_MSEC(ANIM_CHECK_INTERVAL_MS));
}
#endif

void draw_animation(lv_obj_t *canvas) {
#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
    lv_obj_t *art = lv_animimg_create(canvas);
    lv_obj_center(art);

    lv_animimg_set_src(art, (const void **)anim_imgs, 16);
    lv_animimg_set_duration(art, CONFIG_NICE_VIEW_GEM_ANIMATION_MS);
    lv_animimg_set_repeat_count(art, LV_ANIM_REPEAT_INFINITE);
    lv_animimg_start(art);

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
    // Clean up any existing animation timer
    if (animation_obj != NULL) {
        animation_cleanup();
    }

    animation_obj = art;
    last_anim_activity_time = k_uptime_get_32();
    anim_stopped = false;

    // Initialize and start the animation timer
    k_work_init_delayable(&animation_timer, animation_timer_handler);
    k_work_schedule(&animation_timer, K_MSEC(ANIM_CHECK_INTERVAL_MS));
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

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
void animation_reset_timeout(void) {
    last_anim_activity_time = k_uptime_get_32();
    if (anim_stopped && animation_obj != NULL) {
        anim_stopped = false;
        // Re-enable animation
        lv_animimg_set_src(animation_obj, (const void **)anim_imgs, 16);
        lv_animimg_set_duration(animation_obj, CONFIG_NICE_VIEW_GEM_ANIMATION_MS);
        lv_animimg_set_repeat_count(animation_obj, LV_ANIM_REPEAT_INFINITE);
        lv_animimg_start(animation_obj);
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

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
void animation_update(void) {
    if (animation_obj == NULL) {
        return;
    }

    uint32_t now = k_uptime_get_32();
    if (!anim_stopped && (now - last_anim_activity_time > ANIM_TIMEOUT_MS)) {
        anim_stopped = true;
        // Stop animation to preserve battery
        lv_obj_invalidate(animation_obj);
        lv_anim_del(animation_obj, NULL);
    }
}
#endif

#if IS_ENABLED(CONFIG_NICE_VIEW_GEM_STOP_ANIM_TIMEOUT) && IS_ENABLED(CONFIG_NICE_VIEW_GEM_ANIMATION)
void animation_cleanup(void) {
    // Cancel the animation timer to prevent memory leaks
    k_work_cancel_delayable(&animation_timer);
    animation_obj = NULL;
}
#endif