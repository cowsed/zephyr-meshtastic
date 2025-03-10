#ifndef ZEPHYR_MESHTASTIC_CONFIG_H
#define ZEPHYR_MESHTASTIC_CONFIG_H
#include "meshtastic/config.pb.h"
#include "meshtastic/mesh.pb.h"

int get_user_config(meshtastic_User *user);
int set_user_config(const meshtastic_User *user);

#endif