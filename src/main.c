
#include <gtk/gtk.h>

#include "configs.h"

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


static void on_activate([[maybe_unused]] GtkApplication* app) {
    printf("%s\n", source_path);
}
