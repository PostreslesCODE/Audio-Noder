    #include "MixerFilter.h"

    #include <stdio.h>
    #include <string.h>



struct port_data{
    int index;
};



// Callback processing
static void onProcess(void* userdata, struct spa_io_position* position){
    
    struct mixer_data *d = userdata;
    uint32_t n_samples = position->clock.duration; // -> means this position

    float* out = pw_filter_get_dsp_buffer(d->out_port, n_samples);
    if(!out){
        return;
    }

    memset(out,0,n_samples * sizeof(float));

    for(int i = 0;i<d->n_inputs;i++){

        float* in = pw_filter_get_dsp_buffer(d->in_ports[i],n_samples);
        if(!in){ 
            continue;
        }

        for(uint32_t j = 0;j<n_samples;j++){

            out[j] = in[j] + out[j]; // This means that we are summing up the previous inputs plus the next input port, its like adding up all the sounds from all the mics
        }
    }
};


static const struct pw_filter_events filterEvents = {
    PW_VERSION_FILTER_EVENTS,
    .process = onProcess,
};

struct pw_filter *mixer_filter_create(struct pw_loop *loop, struct mixer_data *data, int n_inputs){

        if(n_inputs > MAX_INPUTS){
            n_inputs = MAX_INPUTS;
        }

        data->n_inputs = n_inputs;

        struct pw_filter *filter = pw_filter_new_simple(
            loop,
            "Noder Node 1",
            pw_properties_new(
                PW_KEY_MEDIA_TYPE, "Audio",
                PW_KEY_MEDIA_ROLE, "DSP",
                PW_KEY_MEDIA_CATEGORY, "Filter",
                PW_KEY_MEDIA_CLASS, "Audio/Filter",
                PW_KEY_NODE_NAME, "noder_mixer",
                NULL
            ),
            &filterEvents,
            data
        );

        if(!filter){
            return NULL;
        }



        // Adding input ports (plural)
        
        char portName[32];
        for(int i = 0;i< n_inputs;i++){
            
            snprintf(portName,sizeof(portName), "mic_in_%d",i);

            data->in_ports[i] = pw_filter_add_port(filter,
                PW_DIRECTION_INPUT,
                PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                sizeof(struct port_data),
                pw_properties_new(
                    PW_KEY_FORMAT_DSP, "32 bit float mono audio",
                    PW_KEY_PORT_NAME, portName,
                    NULL
                ),
            NULL,
            0
            );
        }

        // Adding output port (singular)

        data->out_port = pw_filter_add_port(filter,
            PW_DIRECTION_OUTPUT,
            PW_FILTER_PORT_FLAG_MAP_BUFFERS,
            sizeof(struct port_data),

            pw_properties_new(
                PW_KEY_FORMAT_DSP, "32 bit float mono audio",
                PW_KEY_PORT_NAME, "mixed_out",
                NULL
            ),

            NULL,
            0
        );

        
        // Filter Conection
        
        if(pw_filter_connect(filter,PW_FILTER_FLAG_RT_PROCESS,NULL,0) < 0){
            pw_filter_destroy(filter);
            return NULL;
        }
        printf("\nPipewire filter created at: %p\n", (void*)filter);

        return filter;
    };