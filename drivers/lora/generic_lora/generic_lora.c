#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(generic_lora);

#define DT_DRV_COMPAT generic_lora

int generic_lora_send(const struct device *dev, uint8_t *data,
                      uint32_t data_len) {
  return -ENODEV;
}

int generic_lora_send_async(const struct device *dev, uint8_t *data,
                            uint32_t data_len, struct k_poll_signal *async) {
  return -ENODEV;
}

int generic_lora_recv(const struct device *dev, uint8_t *data, uint8_t size,
                      k_timeout_t timeout, int16_t *rssi, int8_t *snr) {
  return -ENODEV;
}

int generic_lora_recv_async(const struct device *dev, lora_recv_cb cb,
                            void *user_data) {
  return -ENODEV;
}

int generic_lora_config(const struct device *dev,
                        struct lora_modem_config *config) {
  return -ENODEV;
}

int generic_lora_test_cw(const struct device *dev, uint32_t frequency,
                         int8_t tx_power, uint16_t duration) {
  return -ENODEV;
}

int generic_lora_init(const struct device *dev) {
  LOG_INF("Init generic Lora");
  return 0;
}

struct generic_lora_config {};
struct generic_lora_data {};

static const struct generic_lora_config dev_config;
static struct generic_lora_data dev_data;

static DEVICE_API(lora, generic_lora_api) = {
    .config = generic_lora_config,
    .send = generic_lora_send,
    .send_async = generic_lora_send_async,
    .recv = generic_lora_recv,
    .recv_async = generic_lora_recv_async,
    .test_cw = generic_lora_test_cw,
};

DEVICE_DT_INST_DEFINE(0, &generic_lora_init, NULL, &dev_data, &dev_config,
                      POST_KERNEL, CONFIG_LORA_INIT_PRIORITY,
                      &generic_lora_api);
