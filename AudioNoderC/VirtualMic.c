    #include "VirtualMic.h"

    #include <stdio.h>
    
int micID = 0;
    

static void onProxyError(void* data, int seq, int res, const char* message){
    printf("\n[PROXY ERROR] seq=%d res=%d message=%s\n", seq, res, message);
};


static const struct pw_proxy_events proxyEvents = {
    PW_VERSION_PROXY_EVENTS,
    .error = onProxyError,
};



struct pw_proxy *virtual_mic_create(struct pw_core *core, struct spa_hook *error_listener){

    
    char node_name[64];
    snprintf(node_name,sizeof(node_name),"noder_mic_%d",micID);

    // Properties for mic node
    struct pw_properties *nullSinkProps = pw_properties_new(

        "factory.name", "support.null-audio-sink",
        "library.name", "audioconvert/libspa-audioconvert",
        PW_KEY_MEDIA_CLASS, "Audio/Source/Virtual",
        PW_KEY_NODE_NAME, node_name,
        "audio.position", "MONO",
        NULL

    );


    // Mic node
    struct pw_proxy *nullSinkProxy = pw_core_create_object(
        core,
        "adapter",
        PW_TYPE_INTERFACE_Node,
        PW_VERSION_NODE,
        &nullSinkProps->dict,
        0
    );

    pw_properties_free(nullSinkProps);

    if(nullSinkProxy){
        pw_proxy_add_listener(nullSinkProxy, error_listener, &proxyEvents, NULL);
    }
    printf("\nNull sink proxy created at: %p\n", (void*)nullSinkProxy);

    micID ++;
    return nullSinkProxy;
};