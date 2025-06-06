
#include "window.h"

#include <gtk/gtkapplicationwindow.h>

#include "loop.h"
#include "files.h"

extern GtkApplication* app;
extern GPtrArray* windows_data;
extern GPtrArray* files;
extern int refresh_interval;
extern int label_size;


static void fullscreen_action(gpointer user_data);

static void new_window_action(gpointer);

static void close_action(gpointer user_data);

static void quit_action(gpointer user_data);

static void autoplay_action(gpointer);

static void this_autoplay_action(gpointer user_data);

static void prev_image_action(gpointer user_data);

static void next_image_action(gpointer user_data);

static void shuffle_files_action(gpointer user_data);

static void zoomin_title_action(gpointer user_data);

static void zoomout_title_action(gpointer user_data);

void create_window_page(const WindowData* window_data, GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label);

void window_title_update_font(const WindowData* window_data);

static void on_window_close(GtkWindow* window, gpointer);


int create_window() {
    WindowData* window_data = malloc(sizeof(WindowData));
    g_ptr_array_add(windows_data, window_data);

    window_data->window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_window_set_title(window_data->window, "Random Image Show");
    gtk_window_set_default_size(window_data->window, 800, 600);

    window_data->label_size = label_size;

    //
    // Files
    //
    window_data->files_order = malloc(sizeof(unsigned int) * files->len);
    for (unsigned int i = 0; i < files->len; i++) {
        window_data->files_order[i] = i;
    }
    shuffle_window_files(window_data);

    window_data->file_index = 0;

    window_data->autoplay = true;
    window_data->next_step_skip = false;

    //
    // Signals
    //

    g_signal_connect(window_data->window, "close-request", G_CALLBACK(on_window_close), NULL);

    //
    // Stack
    //
    window_data->stack = GTK_STACK(gtk_stack_new());
    gtk_stack_set_transition_type(window_data->stack, GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_stack_set_transition_duration(window_data->stack, refresh_interval >= 3000 ? 1000 : refresh_interval / 3);
    gtk_window_set_child(window_data->window, GTK_WIDGET(window_data->stack));

    //
    // Pages
    //
    create_window_page(window_data, &window_data->overlay1, &window_data->image1, &window_data->label1);
    create_window_page(window_data, &window_data->overlay2, &window_data->image2, &window_data->label2);

    //
    // Menu
    //
    window_data->menu_bar = g_menu_new();

    window_data->file_menu = g_menu_new();
    g_menu_append(window_data->file_menu, "Fullscreen", "win.fullscreen");
    g_menu_append(window_data->file_menu, "New window", "win.new");
    g_menu_append(window_data->file_menu, "Close window", "win.close");
    g_menu_append(window_data->file_menu, "Quit", "win.quit");

    window_data->presentation_menu = g_menu_new();
    g_menu_append(window_data->presentation_menu, "Previous image", "win.prev");
    g_menu_append(window_data->presentation_menu, "Next image", "win.next");
    g_menu_append(window_data->presentation_menu, "Auto play", "win.autoplay");
    g_menu_append(window_data->presentation_menu, "Auto play this window", "win.thisautoplay");
    g_menu_append(window_data->presentation_menu, "Shuffle files", "win.shuffle");

    window_data->window_menu = g_menu_new();
    g_menu_append(window_data->window_menu, "Zoom in title", "win.zoomin_title");
    g_menu_append(window_data->window_menu, "Zoom out title", "win.zoomout_title");

    g_menu_append_submenu(window_data->menu_bar, "File", G_MENU_MODEL(window_data->file_menu));
    g_menu_append_submenu(window_data->menu_bar, "Presentation", G_MENU_MODEL(window_data->presentation_menu));
    g_menu_append_submenu(window_data->menu_bar, "Window", G_MENU_MODEL(window_data->window_menu));

    // Create the popover menu
    window_data->menu_button = GTK_MENU_BUTTON(gtk_menu_button_new());
    window_data->menu_popover = GTK_POPOVER_MENU(gtk_popover_menu_new_from_model(G_MENU_MODEL(window_data->menu_bar)));
    gtk_menu_button_set_popover(window_data->menu_button, GTK_WIDGET(window_data->menu_popover));
    gtk_menu_button_set_icon_name(window_data->menu_button, "open-menu-symbolic");

    // Create a header bar and add the menu button
    window_data->header_bar = GTK_HEADER_BAR(gtk_header_bar_new());
    gtk_header_bar_pack_end(window_data->header_bar, GTK_WIDGET(window_data->menu_button));
    gtk_window_set_titlebar(window_data->window, GTK_WIDGET(window_data->header_bar));

    // Create actions
    window_data->menu_fullscreen_action = g_simple_action_new("fullscreen", nullptr);
    window_data->menu_new_action = g_simple_action_new("new", nullptr);
    window_data->menu_close_action = g_simple_action_new("close", nullptr);
    window_data->menu_quit_action = g_simple_action_new("quit", nullptr);
    window_data->menu_prev_action = g_simple_action_new("prev", nullptr);
    window_data->menu_next_action = g_simple_action_new("next", nullptr);
    window_data->menu_autoplay_action = g_simple_action_new_stateful("autoplay", nullptr, g_variant_new_boolean(FALSE));
    window_data->menu_thisautoplay_action = g_simple_action_new_stateful("thisautoplay", nullptr, g_variant_new_boolean(TRUE));
    window_data->menu_shuffle_action = g_simple_action_new("shuffle", nullptr);
    window_data->menu_zoomin_title_action = g_simple_action_new("zoomin_title", nullptr);
    window_data->menu_zoomout_title_action = g_simple_action_new("zoomout_title", nullptr);

    // Connect action signals
    g_signal_connect_swapped(window_data->menu_fullscreen_action, "activate", G_CALLBACK(fullscreen_action), window_data);
    g_signal_connect_swapped(window_data->menu_new_action, "activate", G_CALLBACK(new_window_action), NULL);
    g_signal_connect_swapped(window_data->menu_close_action, "activate", G_CALLBACK(close_action), window_data);
    g_signal_connect_swapped(window_data->menu_quit_action, "activate", G_CALLBACK(quit_action), NULL);
    g_signal_connect_swapped(window_data->menu_prev_action, "activate", G_CALLBACK(prev_image_action), window_data);
    g_signal_connect_swapped(window_data->menu_next_action, "activate", G_CALLBACK(next_image_action), window_data);
    g_signal_connect_swapped(window_data->menu_autoplay_action, "activate", G_CALLBACK(autoplay_action), NULL);
    g_signal_connect_swapped(window_data->menu_thisautoplay_action, "activate", G_CALLBACK(this_autoplay_action), window_data);
    g_signal_connect_swapped(window_data->menu_shuffle_action, "activate", G_CALLBACK(shuffle_files_action), window_data);
    g_signal_connect_swapped(window_data->menu_zoomin_title_action, "activate", G_CALLBACK(zoomin_title_action), window_data);
    g_signal_connect_swapped(window_data->menu_zoomout_title_action, "activate", G_CALLBACK(zoomout_title_action), window_data);

    // Add actions to window and application
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_fullscreen_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_new_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_close_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_quit_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_prev_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_next_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_autoplay_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_thisautoplay_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_shuffle_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_zoomin_title_action));
    g_action_map_add_action(G_ACTION_MAP(window_data->window), G_ACTION(window_data->menu_zoomout_title_action));

    // Set up accelerators (keyboard shortcuts)
    const gchar* fullscreen_accels[] = {"Escape", nullptr};
    gtk_application_set_accels_for_action(app, "win.fullscreen", fullscreen_accels);
    const gchar* new_accels[] = {"<Ctrl>N", nullptr};
    gtk_application_set_accels_for_action(app, "win.new", new_accels);
    const gchar* close_accels[] = {"<Ctrl>W", nullptr};
    gtk_application_set_accels_for_action(app, "win.close", close_accels);
    const gchar* quit_accels[] = {"<Ctrl>Q", nullptr};
    gtk_application_set_accels_for_action(app, "win.quit", quit_accels);
    const gchar* left_accels[] = {"Left", nullptr};
    gtk_application_set_accels_for_action(app, "win.prev", left_accels);
    const gchar* right_accels[] = {"Right", nullptr};
    gtk_application_set_accels_for_action(app, "win.next", right_accels);
    const gchar* autoplay_accels[] = {"<Ctrl>space", nullptr};
    gtk_application_set_accels_for_action(app, "win.autoplay", autoplay_accels);
    const gchar* thisautoplay_accels[] = {"space", nullptr};
    gtk_application_set_accels_for_action(app, "win.thisautoplay", thisautoplay_accels);
    const gchar* shuffle_accels[] = {"<Ctrl>R", nullptr};
    gtk_application_set_accels_for_action(app, "win.shuffle", shuffle_accels);
    const gchar* zoomin_title_accels[] = {"<Ctrl>plus", nullptr};
    gtk_application_set_accels_for_action(app, "win.zoomin_title", zoomin_title_accels);
    const gchar* zoomout_title_accels[] = {"<Ctrl>minus", nullptr};
    gtk_application_set_accels_for_action(app, "win.zoomout_title", zoomout_title_accels);

    //
    // Css
    //
    window_title_update_font(window_data);

    //
    // Prevent suspend
    //
    gtk_application_inhibit(app, window_data->window, GTK_APPLICATION_INHIBIT_SUSPEND | GTK_APPLICATION_INHIBIT_IDLE, "Keeping system awake for image show");

    //
    // Show
    //
    gtk_window_present(window_data->window);

    update_window_image(window_data);

    return EXIT_SUCCESS;
}


void create_window_page(const WindowData* window_data, GtkOverlay** out_overlay, GtkPicture** out_image, GtkLabel** out_label) {
    // Overlay
    auto const overlay = GTK_OVERLAY(gtk_overlay_new());
    *out_overlay = overlay;
    gtk_stack_add_child(window_data->stack, GTK_WIDGET(overlay));

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


void window_title_update_font(const WindowData* window_data) {
    GtkCssProvider* provider = gtk_css_provider_new();

    char* css = g_strdup_printf("label.image-title { font-size: %dpt; text-shadow: 0px 0px %dpx black; }", window_data->label_size, window_data->label_size > 10 ? 4 : 1);

    gtk_css_provider_load_from_string(provider, css);
    free(css);

    GtkCssProvider* old = g_object_get_data(G_OBJECT(window_data->label1), "font-provider");
    if (old) {
        GdkDisplay* display = gtk_widget_get_display(GTK_WIDGET(window_data->label1));
        gtk_style_context_remove_provider_for_display(display, GTK_STYLE_PROVIDER(old));
        g_object_unref(old);
    }
    old = g_object_get_data(G_OBJECT(window_data->label2), "font-provider");
    if (old) {
        GdkDisplay* display = gtk_widget_get_display(GTK_WIDGET(window_data->label2));
        gtk_style_context_remove_provider_for_display(display, GTK_STYLE_PROVIDER(old));
        g_object_unref(old);
    }

    GdkDisplay* display = gtk_widget_get_display(GTK_WIDGET(window_data->label1));
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_set_data_full(G_OBJECT(window_data->label1), "font-provider", provider, g_object_unref);

    display = gtk_widget_get_display(GTK_WIDGET(window_data->label2));
    gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_set_data_full(G_OBJECT(window_data->label2), "font-provider", provider, g_object_unref);
}


static void fullscreen_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    if (gtk_window_is_fullscreen(window_data->window)) {
        gtk_window_unfullscreen(window_data->window);
    } else {
        gtk_window_fullscreen(window_data->window);
    }
}


static void new_window_action(gpointer) {
    create_window();
}


static void close_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    gtk_window_close(window_data->window);
}


static void quit_action(gpointer) {
    g_application_quit(G_APPLICATION(app));
}


static void prev_image_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    window_data->next_step_skip = true;
    prev_image(window_data);
}


static void next_image_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    window_data->next_step_skip = true;
    next_image(window_data);
}


static void autoplay_action(gpointer) {
    start_stop_loop();
}


static void this_autoplay_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    window_data->autoplay = !window_data->autoplay;
    g_simple_action_set_state(window_data->menu_thisautoplay_action, g_variant_new_boolean(window_data->autoplay));
}


static void shuffle_files_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    shuffle_window_files(window_data);
}


static void zoomin_title_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    if (window_data->label_size < 256) {
        window_data->label_size++;
    }
    window_title_update_font(window_data);
}


static void zoomout_title_action(const gpointer user_data) {
    auto const window_data = (WindowData*)user_data;
    if (window_data->label_size > 4) {
        window_data->label_size--;
    }
    window_title_update_font(window_data);
}


static void on_window_close(GtkWindow* window, gpointer) {
    g_ptr_array_remove(windows_data, window);

    gtk_window_destroy(window);
}


void update_window_image(WindowData* window_data) {
    GtkOverlay* next_overlay;
    GtkPicture* next_picture;
    GtkLabel* next_label;
    if (gtk_stack_get_visible_child(window_data->stack) == GTK_WIDGET(window_data->overlay1)) {
        next_overlay = window_data->overlay2;
        next_picture = window_data->image2;
        next_label = window_data->label2;
    } else {
        next_overlay = window_data->overlay1;
        next_picture = window_data->image1;
        next_label = window_data->label1;
    }

    if (window_data->file_index >= files->len) {
        window_data->file_index = 0;
    }
    auto const file = (File*)g_ptr_array_index(files, window_data->files_order[window_data->file_index]);

    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(file->path, nullptr);
    pixbuf = gdk_pixbuf_apply_embedded_orientation(pixbuf);

    GdkPaintable* paintable = GDK_PAINTABLE(gdk_texture_new_for_pixbuf(pixbuf));
    g_object_unref(pixbuf);

    gtk_picture_set_paintable(next_picture, paintable);
    g_object_unref(paintable);

    if (file->title != nullptr) {
        gtk_label_set_text(next_label, file->title);
    } else {
        gtk_label_set_text(next_label, "");
    }

    gtk_stack_set_visible_child(window_data->stack, GTK_WIDGET(next_overlay));
}


void free_windows() {
    for (guint i = 0; i < windows_data->len; i++) {
        free(((WindowData*)g_ptr_array_index(windows_data, i))->files_order);
    }
    g_ptr_array_free(windows_data, true);
}
