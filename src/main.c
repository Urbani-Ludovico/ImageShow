
#include <gtk/gtk.h>

#include "files.h"
#include "loop.h"
#include "window.h"

static void on_activate(GtkApplication* app);

GtkApplication* app;

char* source_path = nullptr;
int refresh_interval = 4000;
char* add_title = nullptr;
int label_size = 24;

extern Files files;


int main(const int argc, char** argv) {
    app = gtk_application_new("com.example.imageshow", G_APPLICATION_DEFAULT_FLAGS);

    const GOptionEntry entries[] = {{"source-path", 'p', 0, G_OPTION_ARG_FILENAME, &source_path, "Source folder path", "FOLDER"}, {"refresh-interval", 'i', 0, G_OPTION_ARG_INT, &refresh_interval, "Refresh rate in milliseconds (>= 100)", "RATE"}, {"title", 't', 0, G_OPTION_ARG_STRING, &add_title, "Add title label: filename, lastdir", "TYPE"}, {"label-size", 's', 0, G_OPTION_ARG_INT, &label_size, "Label size in pt (>= 2)", "24"}, {nullptr},};
    g_application_add_main_option_entries(G_APPLICATION(app), entries);

    g_signal_connect(app, "activate", G_CALLBACK (on_activate), NULL);

    const int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}


static void on_activate(GtkApplication* app) {
    if (get_files() != EXIT_SUCCESS) {
        free_files();
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }

    if (refresh_interval < 100) {
        fprintf(stderr, "Refresh interval too small\n");
        free_files();
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }
    if (label_size < 2) {
        fprintf(stderr, "Label size too small\n");
        free_files();
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }

    if (create_window(app) != EXIT_SUCCESS) {
        free_files();
        g_application_quit(G_APPLICATION(app));
        exit(EXIT_FAILURE);
    }

    shuffle_files();

    update_image(files.files);
    start_stop_loop();
}
