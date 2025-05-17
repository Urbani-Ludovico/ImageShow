
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

static void on_escape_pressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);


int create_window(GtkApplication* app, GtkStack** out_stack, GtkPicture** out_image1, GtkPicture** out_image2) {
    auto const window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window, "Random Image Show");
    gtk_window_set_default_size(window, 800, 600);

    GtkEventController* key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_escape_pressed), window);
    gtk_widget_add_controller(GTK_WIDGET(window), key_controller);

    auto const stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_window_set_child(window, GTK_WIDGET(stack));

    GtkPicture* image1 = GTK_PICTURE(gtk_picture_new());
    gtk_picture_set_content_fit(image1, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(image1), true);
    gtk_widget_set_vexpand(GTK_WIDGET(image1), true);
    gtk_stack_add_child(stack, GTK_WIDGET(image1));

    GtkPicture* image2 = GTK_PICTURE(gtk_picture_new());
    gtk_picture_set_content_fit(image2, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(image2), true);
    gtk_widget_set_vexpand(GTK_WIDGET(image2), true);
    gtk_stack_add_child(stack, GTK_WIDGET(image2));

    gtk_window_present(window);

    *out_stack = stack;
    *out_image1 = image1;
    *out_image2 = image2;

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
