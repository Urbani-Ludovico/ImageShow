
#ifndef CONFIGS_H
#define CONFIGS_H

typedef struct Configs_ {
    char* source_path;
    float interval;
} Configs;

int get_configs(Configs** configs);

#endif
