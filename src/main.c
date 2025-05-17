
#include <gtk/gtk.h>

#include "configs.h"

static void on_activate(GtkApplication* app);


int main(const int argc, char** argv) {
    GtkApplication* app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK (on_activate), NULL);

    const int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}


static void on_activate(GtkApplication* app) {
    Configs* configs = malloc(sizeof(Configs));
    if (get_configs(&configs) != EXIT_SUCCESS) {
        g_application_quit(G_APPLICATION(app));
    }
}
