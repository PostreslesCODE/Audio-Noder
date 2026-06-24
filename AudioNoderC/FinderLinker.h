#ifndef FINDERLINKER_H
#define FINDERLINKER_H

#include <pipewire/pipewire.h>
#define MAX_REAL_MICS 4
#define MAX_PORTS 128
#define MAX_LINKS 64
#define MAX_NODES 256


struct PortPair{

    char out_node[64];
    char out_port[64];
    char in_node[64];
    char in_port[64];
    uint32_t    out_port_id;
    uint32_t    out_node_id;   
    uint32_t    in_port_id;
    uint32_t    in_node_id;   
    bool        linked;

};

struct NodeInfo{

    uint32_t id;
    char name[64];
};

struct FinderLinker{


    struct pw_core *core;
    struct pw_registry *registry;
    struct spa_hook registry_listener;

    struct PortPair *pairs;
    int pair_count;

    struct NodeInfo nodes[MAX_NODES];
    int node_count;

};

typedef struct PortPair     PortPair;
typedef struct NodeInfo     NodeInfo;
typedef struct FinderLinker FinderLinker;

void FinderLinker_init(struct FinderLinker *fl, struct pw_core *core, struct PortPair *pairs, int pair_count);
void FinderLinker_destroy(struct FinderLinker *lf);


#endif