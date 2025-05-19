
#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

typedef struct WindowData_ {
    GtkWindow* window;

    GtkStack* stack;

    GtkOverlay* overlay1;
    GtkPicture* image1;
    GtkLabel* label1;

    GtkOverlay* overlay2;
    GtkPicture* image2;
    GtkLabel* label2;
} WindowData;

int create_window(GtkApplication* app);

#endif
