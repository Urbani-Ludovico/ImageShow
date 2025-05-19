
#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

typedef struct WindowData_ {
    GtkWindow* window;
    GtkStack* stack;
    GtkPicture* image1;
    GtkPicture* image2;
} WindowData;

int create_window(GtkApplication* app);

#endif
