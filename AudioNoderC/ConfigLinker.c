// links_config.c
#include "ConfigLinker.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Minimal JSON string extractor — finds value of "key" in a JSON object fragment
static int extract_str(const char *json, const char *key, char *out, size_t out_sz) {
    char search[64];
    snprintf(search, sizeof(search), "\"%s\"", key);
    const char *p = strstr(json, search);
    if (!p) return -1;
    p += strlen(search);
    while (*p && (*p == ' ' || *p == ':')) p++;
    if (*p != '"') return -1;
    p++; // skip opening quote
    size_t i = 0;
    while (*p && *p != '"' && i < out_sz - 1)
        out[i++] = *p++;
    out[i] = '\0';
    return 0;
}

int config_load(const char *path, struct PortPair *pairs, int *count) {
    *count = 0;

    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "[CFG] cannot open %s\n", path); return -1; }

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);

    char *buf = malloc(sz + 1);
    if (!buf) { fclose(f); return -1; }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);

    const char *pairs_start = strstr(buf, "\"pairs\"");
    if (!pairs_start) { free(buf); return -1; }
    pairs_start = strchr(pairs_start, '[');
    if (!pairs_start) { free(buf); return -1; }

    const char *p = pairs_start;
    while (*count < MAX_PAIRS) {
        const char *obj_start = strchr(p, '{');
        if (!obj_start) break;
        const char *obj_end = strchr(obj_start, '}');
        if (!obj_end) break;

        size_t obj_len = obj_end - obj_start + 1;
        char obj[512] = {0};
        if (obj_len >= sizeof(obj)) { p = obj_end + 1; continue; }
        memcpy(obj, obj_start, obj_len);

        struct PortPair *pp = &pairs[*count];
        if (extract_str(obj, "out_node", pp->out_node, sizeof(pp->out_node)) == 0 &&
            extract_str(obj, "out_port", pp->out_port, sizeof(pp->out_port)) == 0 &&
            extract_str(obj, "in_node",  pp->in_node,  sizeof(pp->in_node))  == 0 &&
            extract_str(obj, "in_port",  pp->in_port,  sizeof(pp->in_port))  == 0)
        {
            (*count)++;
        }

        p = obj_end + 1;
    }

    free(buf);
    fprintf(stderr, "[CFG] loaded %d pairs from %s\n", *count, path);
    return *count > 0 ? 0 : -1;
}