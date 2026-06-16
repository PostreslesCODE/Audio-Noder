#ifndef MIXERFILTER_h
#define MIXERFILTER_h

#include <pipewire/pipewire.h>
#include <pipewire/filter.h>

#define MAX_INPUTS 4

struct mixer_data{

    void *in_ports[MAX_INPUTS];
    void *out_port;
    int n_inputs;

};

struct pw_filter *mixer_filter_create(struct pw_loop *loop, struct mixer_data *data, int n_inputs);


#endif