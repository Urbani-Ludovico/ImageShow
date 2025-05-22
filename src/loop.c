
#include "loop.h"

extern GPtrArray* windows_data;
extern GPtrArray* files;
extern int refresh_interval;

bool timeout_exists = false;
guint timeout_id;

static gboolean loop_step(gpointer);


void start_stop_loop() {
    if (timeout_exists) {
        stop_loop();
    } else {
        start_loop();
    }
}


void start_loop() {
    if (!timeout_exists) {
        timeout_id = g_timeout_add(refresh_interval, loop_step, NULL);
        timeout_exists = true;
        for (guint i = 0; i < windows_data->len; i++) {
            g_simple_action_set_state(((WindowData*)g_ptr_array_index(windows_data, i))->menu_autoplay_action, g_variant_new_boolean(true));
        }
    }
}


void stop_loop() {
    if (timeout_exists) {
        g_source_remove(timeout_id);
        timeout_exists = false;
        for (guint i = 0; i < windows_data->len; i++) {
            g_simple_action_set_state(((WindowData*)g_ptr_array_index(windows_data, i))->menu_autoplay_action, g_variant_new_boolean(false));
        }
    }
}


static gboolean loop_step(gpointer) {
    for (guint i = 0; i < windows_data->len; i++) {
        auto const window_data = (WindowData*)g_ptr_array_index(windows_data, i);
        if (!window_data->next_step_skip && window_data->autoplay) {
            next_image(window_data);
        }
        window_data->next_step_skip = false;
    }
    return true;
}


void next_image(WindowData* window_data) {
    window_data->file_index = (window_data->file_index + 1) % files->len;
    update_window_image(window_data);
}


void prev_image(WindowData* window_data) {
    if (window_data->file_index == 0) {
        window_data->file_index = files->len - 1;
    } else {
        window_data->file_index--;
    }
    update_window_image(window_data);
}
