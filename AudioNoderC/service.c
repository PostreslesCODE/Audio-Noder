#include "service.h"
#include "MixerFilter.h"
#include "VirtualMic.h"
#include "FinderLinker.h"

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include "ConfigLinker.h"

// Pipewire
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <spa/pod/builder.h>
#include <pipewire/filter.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static struct pw_main_loop* globalLoop = NULL;


void* serviceThread(void* arg){

    serviceStart();
    return NULL;
};    



void serviceStart(){

    PortPair my_pairs[MAX_LINKS];
    int pair_count = 0;

    config_load("config.json",my_pairs,&pair_count);

    fprintf(stderr, "[DBG] pair_count = %d\n", pair_count);
    for(int i = 0; i < pair_count; i++)
    fprintf(stderr, "[DBG] pair %d: %s:%s -> %s:%s\n", i,
        my_pairs[i].out_node, my_pairs[i].out_port,
        my_pairs[i].in_node,  my_pairs[i].in_port);

    struct pw_main_loop *loop    = NULL;
    struct pw_filter    *filter  = NULL;
    struct pw_context   *context = NULL;
    struct pw_core      *core    = NULL;
    struct pw_proxy     *mic_proxy1 = NULL;
    struct pw_proxy     *mic_proxy2 = NULL;
    struct spa_hook mic_error_listener1 = {0};
    struct spa_hook mic_error_listener2 = {0};
    struct mixer_data mixer = {0};
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
    FinderLinker_init(&fl,core,my_pairs,pair_count);

    mic_proxy1 = virtual_mic_create(core,&mic_error_listener1);
    mic_proxy2 = virtual_mic_create(core,&mic_error_listener2);

    if(!mic_proxy1){
        printf("\nFail in mic creation\n");
        goto cleanup;
    }

    if(!mic_proxy2){
        printf("\nFail in mic creation\n");
        goto cleanup;
    }


    // Starting the actual node and sink

    loopReturn = pw_main_loop_run(loop);
    

    cleanup:
    // Destroying the node and sink
    if(mic_proxy1) {pw_proxy_destroy(mic_proxy1);}
    if(mic_proxy2) {pw_proxy_destroy(mic_proxy2);}
    FinderLinker_destroy(&fl);
    if(core) {pw_core_disconnect(core);}
    if(context) {pw_context_destroy(context);}
    if(filter) {pw_filter_destroy(filter);}
    if(loop) {pw_main_loop_destroy(loop);}
    micID = 0;
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