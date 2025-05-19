
#include "loop.h"
#include "window.h"

#include <files.h>

extern int refresh_interval;
extern WindowData window_data;
extern Files files;

bool timeout_exists = false;
guint timeout_id;

void start_loop() {
    if (timeout_exists) {
        g_source_remove(timeout_id);
    }

    update_image(nullptr);
    timeout_id = g_timeout_add(refresh_interval, update_image, NULL);
    timeout_exists = true;
}

FilesNode* get_next_image();

gboolean update_image(gpointer) {
    const FilesNode* next_node = get_next_image();

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

    return TRUE;
}


FilesNode* get_next_image() {
    if (files.count < 2) {
        return files.files;
    }

    // Random rotations
    unsigned int step = rand() % (files.count - files.seen_count); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    while (step-- > 0) {
        files.files = files.files->next;
    }

    // Extract node
    FilesNode* node = files.files->next;
    files.files->next = node->next;

    // Seen list
    node->next = nullptr;
    if (files.seen_last == nullptr) {
        files.seen = node;
    } else {
        files.seen_last->next = node;
    }
    files.seen_last = node;
    files.seen_count++;
    while (files.seen_count > (files.count >= 4 ? files.count / 4 : 1)) {
        FilesNode* tmp = files.seen;
        files.seen = files.seen->next;

        tmp->next = files.files->next;
        files.files->next = tmp;
        files.files = tmp->next;
        files.seen_count--;
    }

    return node;
}
