
#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

typedef struct WindowData_ {
    GtkWindow* window;
    GtkHeaderBar* header_bar;
    GActionMap* window_action_map;

    GMenu* menu_bar;
    GtkMenuButton* menu_button;
    GtkPopoverMenu* menu_popover;
    GMenu* file_menu;
    GMenu* presentation_menu;
    GMenu* window_menu;
    GSimpleAction* menu_fullscreen_action;
    GSimpleAction* menu_new_action;
    GSimpleAction* menu_close_action;
    GSimpleAction* menu_quit_action;
    GSimpleAction* menu_prev_action;
    GSimpleAction* menu_next_action;
    GSimpleAction* menu_autoplay_action;
    GSimpleAction* menu_thisautoplay_action;
    GSimpleAction* menu_shuffle_action;
    GSimpleAction* menu_zoomin_title_action;
    GSimpleAction* menu_zoomout_title_action;

    GtkStack* stack;

    GtkOverlay* overlay1;
    GtkPicture* image1;
    GtkLabel* label1;
    unsigned int label_size;

    GtkOverlay* overlay2;
    GtkPicture* image2;
    GtkLabel* label2;

    unsigned int* files_order;
    unsigned int file_index;

    bool autoplay;
    bool next_step_skip;
} WindowData;

int create_window();

void update_window_image(WindowData* window_data);

void free_windows();

#endif
