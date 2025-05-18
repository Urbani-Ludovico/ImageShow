
#include "loop.h"
#include "window.h"

FilesNode* get_next_image(Files* files);


gboolean update_image(const gpointer user_data) {
    const LoopData* data = user_data;

    const FilesNode* next_node = get_next_image(data->files);

    GtkPicture* next_picture = gtk_stack_get_visible_child(data->stack) == GTK_WIDGET(data->image1) ? data->image2 : data->image1;
    gtk_image_set_from_file(GTK_IMAGE(next_picture), next_node->path);

    gtk_stack_set_visible_child(data->stack, GTK_WIDGET(next_picture));

    return TRUE;
}


FilesNode* get_next_image(Files* files) {
    if (files->count < 2) {
        return files->files;
    }

    // Random rotations
    unsigned int step = rand() % (files->count - files->seen_count); // NOLINT(cert-msc30-c, cert-msc50-cpp)
    while (step-- > 0) {
        files->files = files->files->next;
    }

    // Extract node
    FilesNode* node = files->files->next;
    files->files->next = node->next;

    // Seen list
    node->next = nullptr;
    if (files->seen_last == nullptr) {
        files->seen = node;
    } else {
        files->seen_last->next = node;
    }
    files->seen_last = node;
    files->seen_count++;
    while (files->seen_count > (files->count >= 4 ? files->count / 4 : 1)) {
        FilesNode* tmp = files->seen;
        files->seen = files->seen->next;

        tmp->next = files->files->next;
        files->files->next = tmp;
        files->files = tmp->next;
        files->seen_count--;
    }

    return node;
}
