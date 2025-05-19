
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

WindowData window_data = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

extern unsigned int refresh_interval;

static void on_escape_pressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state);

void create_window_page(GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label);

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

    create_window_page(&window_data.overlay1, &window_data.image1, &window_data.label1);
    create_window_page(&window_data.overlay2, &window_data.image2, &window_data.label2);

    gtk_application_inhibit(app, window_data.window, GTK_APPLICATION_INHIBIT_SUSPEND | GTK_APPLICATION_INHIBIT_IDLE, "Keeping system awake for important task");

    gtk_window_present(window_data.window);

    return EXIT_SUCCESS;
}

void create_window_page(GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label) {
    auto const overlay = GTK_OVERLAY(gtk_overlay_new());
    *out_overlay = overlay;
    gtk_stack_add_child(window_data.stack, GTK_WIDGET(overlay));

    auto const image = GTK_PICTURE(gtk_picture_new());
    *out_image = image;
    gtk_picture_set_content_fit(image, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(image), true);
    gtk_widget_set_vexpand(GTK_WIDGET(image), true);
    gtk_overlay_set_child(overlay, GTK_WIDGET(image));

    auto const label = GTK_LABEL(gtk_label_new(""));
    *out_label = label;
    gtk_widget_set_halign(GTK_WIDGET(label), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(label), GTK_ALIGN_END);
    gtk_widget_set_margin_start(GTK_WIDGET(label), 10);
    gtk_widget_set_margin_end(GTK_WIDGET(label), 10);
    gtk_widget_set_margin_top(GTK_WIDGET(label), 10);
    gtk_widget_set_margin_bottom(GTK_WIDGET(label), 10);
    gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
    gtk_overlay_add_overlay(overlay, GTK_WIDGET(label));
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
