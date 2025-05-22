
#ifndef LOOP_H
#define LOOP_H

#include <gtk/gtk.h>

#include "files.h"

void start_stop_loop();

void next_image();
void prev_image();

void update_image(const FilesNode* next_node);

#endif
