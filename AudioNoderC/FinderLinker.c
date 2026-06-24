#include "FinderLinker.h"
#include <stdio.h>
#include <string.h>



static void createLinkPorts(struct FinderLinker *fl, uint32_t outPort, uint32_t inPort, uint32_t outNode,uint16_t inNode){

    char s_on[16], s_op[16], s_in[16], s_ip[16];
    snprintf(s_on, sizeof(s_on), "%u", outNode);
    snprintf(s_op, sizeof(s_op), "%u", outPort);
    snprintf(s_in, sizeof(s_in), "%u", inNode);
    snprintf(s_ip, sizeof(s_ip), "%u", inPort);

    struct pw_properties *props = pw_properties_new(
        PW_KEY_LINK_OUTPUT_NODE, s_on,
        PW_KEY_LINK_OUTPUT_PORT, s_op,
        PW_KEY_LINK_INPUT_NODE,  s_in,
        PW_KEY_LINK_INPUT_PORT,  s_ip,
        NULL
    );
    struct pw_proxy *proxy = pw_core_create_object(
        fl->core, "link-factory",
        PW_TYPE_INTERFACE_Link, PW_VERSION_LINK,
        &props->dict, 0
    );
    pw_properties_free(props);

    if(!proxy){
        fprintf(stderr, "[FL] link creation FAILED for port %u->%u\n", outPort, inPort);
        return;
    }
    fprintf(stderr, "[FL] linked port %u->%u (nodes %u->%u)\n",
            outNode, outPort, inNode, inPort);
};

static void onGlobal(void *data, uint32_t id, uint32_t permissions,
                     const char *type, uint32_t version,
                     const struct spa_dict *props)
{
    struct FinderLinker *fl = data;
    if(!props){return;}

    if(strcmp(type, PW_TYPE_INTERFACE_Port) == 0){
        const char *node_id_str = spa_dict_lookup(props, PW_KEY_NODE_ID);
        const char *port_name   = spa_dict_lookup(props, PW_KEY_PORT_NAME);
        const char *direction   = spa_dict_lookup(props, PW_KEY_PORT_DIRECTION);
        if(!node_id_str || !port_name || !direction){return;}

        uint32_t node_id = (uint32_t)atoi(node_id_str);

        const char *node_name = NULL;
        for(int i = 0; i < fl->node_count; i++)
            if(fl->nodes[i].id == node_id){ node_name = fl->nodes[i].name; break; }
        if(!node_name){return;}

        //fprintf(stderr, "[FL] port id=%u node='%s' dir=%s name='%s'\n", id, node_name, direction, port_name);

        for(int i = 0; i < fl->pair_count; i++){
            struct PortPair *p = &fl->pairs[i];
            if(p->linked){continue;}

            if(!p->out_port_id && strcmp(direction,"out")==0
                && strcmp(node_name, p->out_node)==0
                && strcmp(port_name, p->out_port)==0){
                p->out_port_id = id;
                p->out_node_id = node_id;
            }
            if(!p->in_port_id && strcmp(direction,"in")==0
                && strcmp(node_name, p->in_node)==0
                && strcmp(port_name, p->in_port)==0){
                p->in_port_id = id;
                p->in_node_id = node_id;
            }
            if(p->out_port_id && p->in_port_id){
                createLinkPorts(fl, p->out_port_id, p->in_port_id,
                                    p->out_node_id, p->in_node_id);
                p->linked = true;
            }
        }
        return;
    }

    if(strcmp(type, PW_TYPE_INTERFACE_Node) != 0){return;}

    const char *node_name   = spa_dict_lookup(props, PW_KEY_NODE_NAME);
    const char *media_class = spa_dict_lookup(props, PW_KEY_MEDIA_CLASS);
    if(!node_name){return;}

    //fprintf(stderr, "[FL] node id=%u name='%s' class='%s'\n",id, node_name, media_class ? media_class : "(null)");

    if(fl->node_count >= MAX_NODES){return;}
    fl->nodes[fl->node_count].id = id;
    strncpy(fl->nodes[fl->node_count].name, node_name, 63);
    fl->node_count++;
}

static void onGlobalRemove(void *data, uint32_t id){

    struct FinderLinker *fl = data;

    // Remove from node cache
    for(int i = 0; i < fl->node_count; i++){
        if(fl->nodes[i].id == id){
            // Shift remaining nodes down
            fl->nodes[i] = fl->nodes[fl->node_count - 1];
            fl->node_count--;
            break;
        }
    }

    // Reset any pair whose ports depended on this id
    for(int i = 0; i < fl->pair_count; i++){
        struct PortPair *p = &fl->pairs[i];
        if(p->out_port_id == id || p->in_port_id == id ||
           p->out_node_id == id || p->in_node_id == id){
            p->linked      = false;
            p->out_port_id = 0;
            p->in_port_id  = 0;
            p->out_node_id = 0;
            p->in_node_id  = 0;
        }
    }
}


static const struct pw_registry_events registry_events = {
    PW_VERSION_REGISTRY_EVENTS,
    .global        = onGlobal,
    .global_remove = onGlobalRemove,
};


void FinderLinker_init(struct FinderLinker *fl, struct pw_core *core, struct PortPair *pairs, int pair_count){

    if(fl->registry){
        pw_proxy_destroy((struct pw_proxy *)fl->registry);
        fl->registry = NULL;
    }

    memset(fl,0,sizeof(*fl));
    fl->core = core;
    fl->pairs = pairs;
    fl->pair_count = pair_count;

    for(int i = 0; i < fl->pair_count; i++){
        fl->pairs[i].linked = false;
        fl->pairs[i].out_port_id = 0;
        fl->pairs[i].in_port_id  = 0;
        fl->pairs[i].out_node_id = 0;
        fl->pairs[i].in_node_id  = 0;
    }
    fprintf(stderr, "[FL] Pairs Reset\n");



    fprintf(stderr, "[FL] loaded %d pairs\n", fl->pair_count);
    for(int i = 0; i < fl->pair_count; i++)
        fprintf(stderr, "[FL]   pair %d: %s:%s -> %s:%s\n", i,
                fl->pairs[i].out_node, fl->pairs[i].out_port,
                fl->pairs[i].in_node,  fl->pairs[i].in_port);


    fl->registry = pw_core_get_registry(core, PW_VERSION_REGISTRY,0);
    spa_zero(fl->registry_listener);
    pw_registry_add_listener(fl->registry, &fl->registry_listener, &registry_events, fl);
    fprintf(stderr, "[FL] registry listener attached\n");
};

void FinderLinker_destroy(struct FinderLinker *fl){

    if(fl->registry){
        pw_proxy_destroy((struct pw_proxy *)fl->registry);
        fl->registry = NULL;
    }
}