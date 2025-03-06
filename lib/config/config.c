#include "config/config.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(meshtastic_config);

int set_meshtastic_name(const char *short_name, const char *long_name) {
  LOG_INF("Setting meshtastic shortname: '%s'", short_name);
  return 0;
}