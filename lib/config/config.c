#include "config/config.h"

#include "zephyr/settings/settings.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(meshtastic_config);

int get_user_config(meshtastic_User *user) {
  // settings
  return 0;
}
int set_user_config(const meshtastic_User *user) { return 0; }

int meshtastic_config_subsys_init(void) {
  int err = settings_subsys_init();
  if (err != 0) {
    LOG_ERR("Failed to initialize settings subsystem for meshtastic config: %d",
            err);
    return err;
  }
  return 0;
}

SYS_INIT(meshtastic_config_subsys_init, APPLICATION, 0);