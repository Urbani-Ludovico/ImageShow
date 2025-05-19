
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

#include "loop.h"

WindowData window_data;

extern GtkApplication* app;
extern int refresh_interval;
extern int label_size;

static void on_escape_pressed(GtkEventControllerKey* controller, guint keyval, guint keycode, GdkModifierType state);

void create_window_page(GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label);

static void fullscreen(gpointer);


int create_window(GtkApplication* app) {
    window_data.window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window_data.window, "Random Image Show");
    gtk_window_set_default_size(window_data.window, 800, 600);

    //
    // Key press
    //
    GtkEventController* key_controller = gtk_event_controller_key_new();
    g_signal_connect(key_controller, "key-pressed", G_CALLBACK(on_escape_pressed), NULL);
    gtk_widget_add_controller(GTK_WIDGET(window_data.window), key_controller);

    //
    // Stack
    //
    window_data.stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(window_data.stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(window_data.stack, refresh_interval >= 3000 ? 1000 : refresh_interval / 3);
    gtk_window_set_child(window_data.window, GTK_WIDGET(window_data.stack));

    //
    // Pages
    //
    create_window_page(&window_data.overlay1, &window_data.image1, &window_data.label1);
    create_window_page(&window_data.overlay2, &window_data.image2, &window_data.label2);

    //
    // Menu
    //
    window_data.menu_bar = g_menu_new();

    window_data.file_menu = g_menu_new();
    g_menu_append(window_data.file_menu, "Fullscreen", "app.fullscreen");
    g_menu_append(window_data.file_menu, "Quit", "app.quit");

    window_data.presentation_menu = g_menu_new();

    g_menu_append_submenu(window_data.menu_bar, "File", G_MENU_MODEL(window_data.file_menu));
    g_menu_append_submenu(window_data.menu_bar, "Presentation", G_MENU_MODEL(window_data.presentation_menu));

    // Create the popover menu
    window_data.menu_button = GTK_MENU_BUTTON(gtk_menu_button_new());
    window_data.menu_popover = GTK_POPOVER_MENU(gtk_popover_menu_new_from_model(G_MENU_MODEL(window_data.menu_bar)));
    gtk_menu_button_set_popover(window_data.menu_button, GTK_WIDGET(window_data.menu_popover));
    gtk_menu_button_set_icon_name(window_data.menu_button, "open-menu-symbolic");

    // Create a header bar and add the menu button
    window_data.header_bar = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_pack_end(window_data.header_bar, GTK_WIDGET(window_data.menu_button));
    gtk_window_set_titlebar(window_data.window, GTK_WIDGET(window_data.header_bar));

    // Create actions
    window_data.menu_fullscreen_action = g_simple_action_new("fullscreen", nullptr);
    window_data.menu_quit_action = g_simple_action_new("quit", nullptr);

    // Connect action signals
    g_signal_connect_swapped(window_data.menu_fullscreen_action, "activate", G_CALLBACK(fullscreen), NULL);
    g_signal_connect_swapped(window_data.menu_quit_action, "activate", G_CALLBACK(g_application_quit), NULL);

    // Add actions to window and application
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(window_data.menu_quit_action));
    g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(window_data.menu_fullscreen_action));

    //
    // Css
    //
    char* css = g_strdup_printf("label.image-title { font-size: %dpt; text-shadow: 0px 0px %dpx black; }", label_size, label_size > 10 ? 4 : 1);
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    free(css);
    gtk_style_context_add_provider_for_display(gtk_widget_get_display(GTK_WIDGET(window_data.window)), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Prevent suspend
    gtk_application_inhibit(app, window_data.window, GTK_APPLICATION_INHIBIT_SUSPEND | GTK_APPLICATION_INHIBIT_IDLE, "Keeping system awake for important task");

    gtk_window_present(window_data.window);

    return EXIT_SUCCESS;
}


void create_window_page(GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label) {
    // Overlay
    auto const overlay = GTK_OVERLAY(gtk_overlay_new());
    *out_overlay = overlay;
    gtk_stack_add_child(window_data.stack, GTK_WIDGET(overlay));

    // Image
    auto const image = GTK_PICTURE(gtk_picture_new());
    *out_image = image;
    gtk_picture_set_content_fit(image, GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_hexpand(GTK_WIDGET(image), true);
    gtk_widget_set_vexpand(GTK_WIDGET(image), true);
    gtk_overlay_set_child(overlay, GTK_WIDGET(image));

    // Label
    auto const label = GTK_LABEL(gtk_label_new(""));
    gtk_widget_add_css_class(GTK_WIDGET(label), "image-title");
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


static void on_escape_pressed(GtkEventControllerKey*, const guint keyval, guint, const GdkModifierType state) {
    if (keyval == GDK_KEY_Escape) {
        fullscreen(nullptr);
    } else if (keyval == GDK_KEY_space) {
        start_stop_loop();
    } else if (keyval == GDK_KEY_Left) {
        prev_image();
    } else if (keyval == GDK_KEY_Right) {
        next_image();
    } else if ((keyval == GDK_KEY_q || keyval == GDK_KEY_Q) && (state & GDK_CONTROL_MASK)) {
        g_application_quit(G_APPLICATION(app));
    }
}

static void fullscreen(gpointer) {
    if (gtk_window_is_fullscreen(window_data.window)) {
        gtk_window_unfullscreen(window_data.window);
    } else {
        gtk_window_fullscreen(window_data.window);
    }
}