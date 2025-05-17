
#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>

int create_window(GtkApplication *app, GtkStack** out_stack, GtkPicture** out_image1, GtkPicture** out_image2);

#endif
