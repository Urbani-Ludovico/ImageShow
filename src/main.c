
#include <gtk/gtk.h>

#include "files.h"
#include "window.h"

static void on_activate(GtkApplication* app);

static char* source_path = nullptr;
GOptionEntry entries[] = {
    { "source-path", 'p', 0, G_OPTION_ARG_STRING, &source_path, "Source folder path", "FOLDER" }
};

int main(const int argc, char** argv) {
    GtkApplication* app = gtk_application_new("com.example.imageshow", G_APPLICATION_DEFAULT_FLAGS);

    g_application_add_main_option_entries(G_APPLICATION(app), entries);

    g_signal_connect(app, "activate", G_CALLBACK (on_activate), NULL);

    const int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}


static void on_activate(GtkApplication* app) {
    Files* files;
    if (get_files(&files, source_path) != EXIT_SUCCESS) {
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }

    if (create_window(app) != EXIT_SUCCESS) {
        free_files(files);
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }
}
