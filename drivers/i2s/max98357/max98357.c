/*
 * Copyright (c) 2025 Launch Initiative
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT adi_max98357

#include <zephyr/kernel.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MAX98357, CONFIG_SENSOR_LOG_LEVEL);

static int adi_max98357_configure(const struct device *dev, enum i2s_dir dir,
			     const struct i2s_config *i2s_cfg)
{
	return -ENOTSUP;
}

static const struct i2s_config *adi_max98357_config_get(const struct device *dev, enum i2s_dir dir)
{
	return NULL;
}

static int adi_max98357_trigger(const struct device *dev, enum i2s_dir dir, enum i2s_trigger_cmd cmd)
{
	return -ENOTSUP;
}

static int adi_max98357_read(const struct device *dev, void **mem_block, size_t *size)
{
	return -ENOTSUP;
}

static int adi_max98357_write(const struct device *dev, void *mem_block, size_t size)
{
	return -ENOTSUP;
}

static DEVICE_API(i2s, adi_max98357_driver_api) = {
	.configure = adi_max98357_configure,
	.config_get = adi_max98357_config_get,
	.trigger = adi_max98357_trigger,
	.read = adi_max98357_read,
	.write = adi_max98357_write,
};

static int adi_max98357_init(const struct device *dev)
{
	return 0;
}

#define ADI_MAX98357_INIT(index)                                                                        \
	DEVICE_DT_INST_DEFINE(index, &adi_max98357_init, NULL, NULL, NULL, POST_KERNEL,                 \
			      CONFIG_I2S_INIT_PRIORITY, &adi_max98357_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ADI_MAX98357_INIT)
