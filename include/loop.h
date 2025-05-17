
#ifndef LOOP_H
#define LOOP_H

#include <gtk/gtk.h>
#include "files.h"

typedef struct LoopData_ {
    GtkStack* stack;
    GtkPicture* image1;
    GtkPicture* image2;
    Files* files;
} LoopData;

gboolean update_image(gpointer user_data);

#endif
