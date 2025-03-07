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

#define DEFAULT_RADIO_NODE DT_ALIAS(meshtastic_radio)
BUILD_ASSERT(
    DT_NODE_HAS_STATUS_OKAY(DEFAULT_RADIO_NODE),
    "No default LoRa radio specified in DT (Need alias 'meshtastic-radio')");

const struct device *const mesh_dev = DEVICE_DT_GET(DEFAULT_RADIO_NODE);

struct level0_msg {
  int a;
  int b;
};

ZBUS_CHAN_DEFINE(meshtastic_level0_chan, /* Name */
                 struct level0_msg,      /* Message type */

                 NULL,                 /* Validator */
                 NULL,                 /* User data */
                 ZBUS_OBSERVERS_EMPTY, /* observers */
                 ZBUS_MSG_INIT(.a = 0, .b = 1));

int init_meshtastic() {
  LOG_INF("Meshtastic Init");
  return 0;
}

SYS_INIT(init_meshtastic, POST_KERNEL, CONFIG_LORA_INIT_PRIORITY);