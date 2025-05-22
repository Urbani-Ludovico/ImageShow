
#include "loop.h"

extern GPtrArray* windows_data;
extern GPtrArray* files;
extern int refresh_interval;

bool timeout_exists = false;
guint timeout_id;

static void loop_step(gpointer);

void start_stop_loop() {
    if (timeout_exists) {
        g_source_remove(timeout_id);
        timeout_exists = false;
        // g_simple_action_set_state(window_data.menu_autoplay_action, g_variant_new_boolean(FALSE));
    } else {
        timeout_id = g_timeout_add(refresh_interval, loop_step, NULL);
        timeout_exists = true;
        // g_simple_action_set_state(window_data.menu_autoplay_action, g_variant_new_boolean(TRUE));
    }
}


static void loop_step(gpointer) {
    for (int i = 0; i < windows_data->len; i++) {
        next_image(g_ptr_array_index(windows_data, i));
    }
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