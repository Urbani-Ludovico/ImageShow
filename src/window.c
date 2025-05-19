
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

WindowData window_data = {nullptr, nullptr, nullptr, nullptr};

extern unsigned int refresh_interval;

static void on_escape_pressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state);


int create_window(GtkApplication* app) {
    window_data.window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window_data.window, "Random Image Show");
    gtk_window_set_default_size(window_data.window, 800, 600);

    GtkEventController* key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_escape_pressed), NULL);
    gtk_widget_add_controller(GTK_WIDGET(window_data.window), key_controller);

    window_data.stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(window_data.stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(window_data.stack, refresh_interval >= 3000 ? 1000 : refresh_interval / 3);
    gtk_window_set_child(window_data.window, GTK_WIDGET(window_data.stack));

    window_data.image1 = GTK_PICTURE(gtk_picture_new());
    gtk_picture_set_content_fit(window_data.image1, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(window_data.image1), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window_data.image1), true);
    gtk_stack_add_child(window_data.stack, GTK_WIDGET(window_data.image1));

    window_data.image2 = GTK_PICTURE(gtk_picture_new());
    gtk_picture_set_content_fit(window_data.image2, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(window_data.image2), true);
    gtk_widget_set_vexpand(GTK_WIDGET(window_data.image2), true);
    gtk_stack_add_child(window_data.stack, GTK_WIDGET(window_data.image2));

    gtk_application_inhibit(app, window_data.window, GTK_APPLICATION_INHIBIT_SUSPEND | GTK_APPLICATION_INHIBIT_IDLE, "Keeping system awake for important task");

    gtk_window_present(window_data.window);

    return EXIT_SUCCESS;
}


static void on_escape_pressed(GtkEventControllerKey*, const guint keyval, guint, GdkModifierType) {
    if (keyval == GDK_KEY_Escape) {
        if (gtk_window_is_fullscreen(window_data.window)) {
            gtk_window_unfullscreen(window_data.window);
        } else {
            gtk_window_fullscreen(window_data.window);
        }
    }
}
