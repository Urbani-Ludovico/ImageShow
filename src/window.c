
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

static void on_escape_pressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);


int create_window(GtkApplication* app) {
    auto const window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window, "Random Image Show");
    gtk_window_set_default_size(window, 800, 600);

    GtkEventController* key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_escape_pressed), window);
    gtk_widget_add_controller(GTK_WIDGET(window), key_controller);

    gtk_window_present(window);

    return EXIT_SUCCESS;
}


static void on_escape_pressed(GtkEventControllerKey*, const guint keyval, guint, GdkModifierType, const gpointer user_data) {
    GtkWindow* window = user_data;

    if (keyval == GDK_KEY_Escape) {
        if (gtk_window_is_fullscreen(window)) {
            gtk_window_unfullscreen(window);
        } else {
            gtk_window_fullscreen(window);
        }
    }
}
