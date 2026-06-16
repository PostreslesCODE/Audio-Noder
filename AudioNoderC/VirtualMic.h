#ifndef VIRTUALMIC_H
#define VIRTUALMIC_H

#include <pipewire/pipewire.h>

struct pw_proxy *virtual_mic_create(struct pw_core *core, struct spa_hook *error_listener);

#endif