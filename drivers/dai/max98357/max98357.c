
#include <errno.h>

#include <zephyr/audio/codec.h>
#include <zephyr/device.h>
#include <zephyr/devicetree/clocks.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/dai.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/iterable_sections.h>

#define DT_DRV_COMPAT adi_max98357
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MAX98357);

struct max98357_config {
  const struct gpio_dt_spec *sd_mode;
  const struct device *i2s_dev;
  struct k_mem_slab *tx_slab;
  struct dai_properties props;
};

static int max_dai_probe(const struct device *dev) {
  const struct max98357_config *cfg = dev->config;
  if (cfg->sd_mode != NULL) {
    // Turn on SD_MODE GPIO
  } else {
    LOG_INF("No SD Mode GPIO");
  }
  return -ENODEV;
  // int ret;

  // // Configure I2S
  // struct i2s_config i2s_cfg;

  // if (!device_is_ready(cfg->i2s_dev)) {
  //   LOG_ERR("I2S bus is not ready");
  //   return -ENODEV;
  // }
  // /* Configure I2S stream */
  // i2s_cfg.word_size = 16U;
  // i2s_cfg.channels = 2U;
  // i2s_cfg.format = I2S_FMT_DATA_FORMAT_I2S;
  // i2s_cfg.frame_clk_freq = 8000;
  // i2s_cfg.block_size = BLOCK_SIZE;
  // i2s_cfg.timeout = 2000;
  // /* Configure the Transmit port as Master */
  // i2s_cfg.options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER;
  // i2s_cfg.mem_slab = &tx_0_mem_slab;

  // ret = i2s_configure(cfg->i2s_dev, I2S_DIR_TX, &i2s_cfg);
  // if (ret < 0) {
  //   LOG_ERR("Failed to configure I2S stream\n");
  //   return ret;
  // }
}

static int max_dai_remove(const struct device *dev) {
  // Turn off SD_MODE GPIO
  return -ENOTSUP;
}

static int max_dai_trigger(const struct device *dev, enum dai_dir dir,
                           enum dai_trigger_cmd cmd) {
  return -ENOTSUP;
}

static int max_dai_config_get(const struct device *dev, struct dai_config *cfg,
                              enum dai_dir dir) {
  return -ENOTSUP;
}

static int max_dai_config_set(const struct device *dev,
                              const struct dai_config *cfg,
                              const void *bespoke_cfg) {
  return -ENOTSUP;
}

static const struct dai_properties *
max_dai_get_properties(const struct device *dev, enum dai_dir dir,
                       int stream_id) {
  const struct max98357_config *cfg = dev->config;
  return &cfg->props;
}

static DEVICE_API(dai, max98375_dai_api) = {
    .config_set = max_dai_config_set,
    .config_get = max_dai_config_get,
    .trigger = max_dai_trigger,
    .get_properties = max_dai_get_properties,
    .probe = max_dai_probe,
    .remove = max_dai_remove,
};

#define SAMPLE_NO 128
#define MAX_NUM_BLOCKS 20
#define MAX_BLOCK_SIZE (2 * 2 * SAMPLE_NO)

#define MAX98357_INIT(n)                                                       \
  K_MEM_SLAB_DEFINE(max98357_tx_mem_slab##n, MAX_NUM_BLOCKS, MAX_BLOCK_SIZE,   \
                    2);                                                        \
                                                                               \
  static const struct max98357_config max98357_config_##n = {                  \
      .sd_mode = DT_PROP_OR(n, sd_mode_gpios, NULL),                           \
      .tx_slab = &max98357_tx_mem_slab##n,                                     \
      .i2s_dev = DT_PARENT(DT_INST(n, DT_DRV_COMPAT)),                         \
      .props = {},                                                             \
  };                                                                           \
                                                                               \
  DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, &max98357_config_##n,             \
                        POST_KERNEL, CONFIG_AUDIO_CODEC_INIT_PRIORITY,         \
                        &max98375_dai_api);

DT_INST_FOREACH_STATUS_OKAY(MAX98357_INIT);
