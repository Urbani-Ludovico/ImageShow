
#include <gtk/gtk.h>

static void on_activate(GtkApplication* app);


int main(const int argc, char** argv) {
    GtkApplication* app = gtk_application_new("com.example.GtkApplication", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK (on_activate), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}


static void on_activate(GtkApplication* app) {}
