
#include "loop.h"
#include "window.h"

#include <files.h>

extern int refresh_interval;
extern WindowData window_data;
extern Files files;

bool timeout_exists = false;
guint timeout_id;


void start_stop_loop() {
    if (timeout_exists) {
        g_source_remove(timeout_id);
        timeout_exists = false;
        g_simple_action_set_state(window_data.menu_autoplay_action, g_variant_new_boolean(FALSE));
    } else {
        timeout_id = g_timeout_add(refresh_interval, next_image_action, NULL);
        timeout_exists = true;
        g_simple_action_set_state(window_data.menu_autoplay_action, g_variant_new_boolean(TRUE));
    }
}


void next_image() {
    FilesNode* next_node = files.files->next;
    files.files = next_node;
    update_image(next_node);
}


void prev_image() {
    FilesNode* next_node = files.files->prev;
    files.files = next_node;
    update_image(next_node);
}


void update_image(const FilesNode* next_node) {
    GtkOverlay* next_overlay;
    GtkPicture* next_picture;
    GtkLabel* next_label;
    if (gtk_stack_get_visible_child(window_data.stack) == GTK_WIDGET(window_data.overlay1)) {
        next_overlay = window_data.overlay2;
        next_picture = window_data.image2;
        next_label = window_data.label2;
    } else {
        next_overlay = window_data.overlay1;
        next_picture = window_data.image1;
        next_label = window_data.label1;
    }

    gtk_picture_set_filename(next_picture, next_node->path);

    if (next_node->title != nullptr) {
        gtk_label_set_text(next_label, next_node->title);
    } else {
        gtk_label_set_text(next_label, "");
    }

    gtk_stack_set_visible_child(window_data.stack, GTK_WIDGET(next_overlay));
}
