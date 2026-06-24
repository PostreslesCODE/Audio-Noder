#ifndef CONFIGLINKER_H
#define CONFIGLINKER_H

#include "FinderLinker.h"

#define MAX_PAIRS 64

typedef struct {
    PortPair pairs[MAX_PAIRS];
    int      count;
} LinksConfig;

int config_load(const char *path, struct PortPair *pairs, int *count);

#endif