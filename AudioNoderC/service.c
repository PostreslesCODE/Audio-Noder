#include "service.h"
#include "MixerFilter.h"
#include "VirtualMic.h"
#include "FinderLinker.h"

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

// Pipewire
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/pod/builder.h>
#include <pipewire/filter.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static struct pw_main_loop* globalLoop = NULL;

static struct PortPair my_pairs[] = {

    { "gx_head_fx", "out_0", "noder_mixer", "mic_in_0" },
    { "gx_head_fx", "out_1", "noder_mixer", "mic_in_0" },
    { "easyeffects_source", "capture_FL", "noder_mixer", "mic_in_2" },
    { "easyeffects_source", "capture_FR", "noder_mixer", "mic_in_2" },
    { "pwsp-virtual-mic", "capture_FL", "noder_mixer", "mic_in_1" },
    { "pwsp-virtual-mic", "capture_FR", "noder_mixer", "mic_in_1" },
    { "noder_mixer",        "mixed_out",  "noder_mic",   "input_MONO" },

};


void* serviceThread(void* arg){

    serviceStart();
    return NULL;
};    



void serviceStart(){

    struct pw_main_loop *loop    = NULL;
    struct pw_filter    *filter  = NULL;
    struct pw_context   *context = NULL;
    struct pw_core      *core    = NULL;
    struct pw_proxy     *mic_proxy = NULL;
    struct spa_hook mic_error_listener = {0};
    struct mixer_data mixer = {0};
    //struct link_finder lf = {0};
    int loopReturn = -1;

    printf("\nService starting\n");

    pw_init(NULL,NULL);

    loop = pw_main_loop_new(NULL);

    if(!loop){
        printf("\nFail in loop creation\n");
        goto cleanup;
    }
    
    globalLoop = loop;


    filter = mixer_filter_create(pw_main_loop_get_loop(loop),&mixer,4);

    if(!filter){
        printf("\nFail in filter creation\n");
        goto cleanup;
    }

    context = pw_context_new(pw_main_loop_get_loop(loop), NULL,0);
    
    core = pw_context_connect(context,NULL,0);

    if(!core){
        printf("\nFail in core creation\n");
        goto cleanup;
    }

    struct FinderLinker fl;
    FinderLinker_init(&fl,core,my_pairs,ARRAY_SIZE(my_pairs));

    mic_proxy = virtual_mic_create(core,&mic_error_listener);

    if(!mic_proxy){
        printf("\nFail in mic creation\n");
        goto cleanup;
    }


    // Starting the actual node and sink

    loopReturn = pw_main_loop_run(loop);
    

    cleanup:
    // Destroying the node and sink
    if(mic_proxy) {pw_proxy_destroy(mic_proxy);}
    FinderLinker_destroy(&fl);
    if(core) {pw_core_disconnect(core);}
    if(context) {pw_context_destroy(context);}
    if(filter) {pw_filter_destroy(filter);}
    if(loop) {pw_main_loop_destroy(loop);}

    globalLoop = NULL;
    pw_deinit();


    printf("\nMain loop exited with %d\n", loopReturn);


    
    
};

void serviceStop(){

        if(globalLoop){
            pw_main_loop_quit(globalLoop);
            
            printf("\n Loop quit\n");

        }
};