#include "config/config.h"

#include "zephyr/settings/settings.h"

#include "config_util.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(meshtastic_config);
#define MESHTASTIC_CONFIG_ROOT "meshtastic/config"
#define MESHTASTIC_CONFIG_VERSION_KEY (MESHTASTIC_CONFIG_ROOT "/version")
#define MESHTASTIC_CONFIG_USER_KEY (MESHTASTIC_CONFIG_ROOT "/user")

typedef uint8_t macaddr[6];
void get_mac_addr(macaddr *m) {
  *m[0] = 0;
  *m[1] = 1;
  *m[2] = 2;
  *m[3] = 3;
  *m[4] = 4;
  *m[5] = 5;
}

int get_user_config(meshtastic_User *user) {
  char buf[BASE64_OVERHEAD(meshtastic_User_size)] = {0};
  // settings
  int num_read = 0;
  int rc = read_config_string(MESHTASTIC_CONFIG_USER_KEY, buf,
                              BASE64_OVERHEAD(meshtastic_User_size), &num_read);

  if (rc != 0) {
    LOG_ERR("Couldn't load user configuration from settings");
    return rc;
  }

  PROTO64_DECODE(buf, num_read, user, meshtastic_User);
  if (user_rc != 0) {
    LOG_ERR("Failed to decode user");
    return user_rc;
  }

  return 0;
}
int set_user_config(const meshtastic_User *user) {
  PROTO64_ENCODE(user, meshtastic_User);
  if (user_rc != 0) {
    LOG_ERR("Encoding Failed: %d", user_rc);
    return user_rc;
  }
  return settings_save_one(MESHTASTIC_CONFIG_USER_KEY, user_encoded,
                           user_num_written);
}

int write_initial_mststc_config(uint8_t uniqueId[16]) {
  LOG_INF("Writing initial meshtastic configuration");
  int rc = 0;
  rc = settings_save_one(MESHTASTIC_CONFIG_VERSION_KEY, "2.6.1",
                         sizeof("2.6.1"));
  if (rc != 0) {
    LOG_ERR("Failed to write meshtastic version key. Impassable error on "
            "storage backend: %d",
            rc);
    return rc;
  }
  meshtastic_User usr = {
      .id = {0},
      .long_name = "meshtastic user",
      .short_name = "8884",
      .hw_model = meshtastic_HardwareModel_UNSET,
      .is_licensed = false,
      .role = meshtastic_Config_DeviceConfig_Role_CLIENT,
      .public_key = {0},
  };
  memcpy(&usr.id, uniqueId, sizeof(usr.id));

  set_user_config(&usr);
  meshtastic_User usr2 = {0};
  get_user_config(&usr2);
  LOG_INF("FInished Meshtastic init");
  return 0;
}

int meshtastic_config_subsys_init(void) {
  LOG_INF("Meshtastic Config Init");
  int err = settings_subsys_init();
  if (err != 0) {
    LOG_ERR("Failed to initialize settings subsystem for meshtastic config: %d",
            err);
    return err;
  }

  char buf[10];
  size_t num_read = 0;
  read_config_string(MESHTASTIC_CONFIG_VERSION_KEY, buf, 10, &num_read);
  LOG_INF("Read: %.*s", 10, buf);

  if (num_read > 0) {
    // found version
    LOG_INF("GOUND VERSION");
  } else {
    LOG_INF("Didn't find config version key. Writing initial config");

    uint8_t buf[16] = {
        0x88, 0x84, 0x88, 0x84, 0x88, 0x84, 0x88, 0x84,
        0x88, 0x84, 0x88, 0x84, 0x88, 0x84, 0x88, 0x0,
    };
    write_initial_mststc_config(buf);
  }

  return 0;
}
