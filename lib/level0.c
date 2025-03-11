#include "meshtastic/mesh.pb.h"
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
LOG_MODULE_REGISTER(meshtastic);

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

// #define DEFAULT_RADIO_NODE DT_ALIAS(meshtastic_radio)
// BUILD_ASSERT(
// DT_NODE_HAS_STATUS_OKAY(DEFAULT_RADIO_NODE),
// "No default LoRa radio specified in DT (Need alias 'meshtastic-radio')");

// const struct device *const mesh_dev = DEVICE_DT_GET(DEFAULT_RADIO_NODE);

// from meshtastic_config.c
int meshtastic_config_subsys_init(void);

int init_meshtastic() {
  LOG_INF("Meshtastic Init");
  meshtastic_config_subsys_init();
  return 0;
}

SYS_INIT(init_meshtastic, APPLICATION, 0);