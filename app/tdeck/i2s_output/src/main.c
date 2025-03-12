/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/iterable_sections.h>

#define SAMPLE_NO 128

int16_t source[] = {
#include "cout.inc"
};

static void fill_buf(int16_t *tx_block, int source_block_index, int att) {
  att = 2;

  for (int i = 0; i < SAMPLE_NO; i++) {
    size_t source_index = source_block_index * SAMPLE_NO + i;
    int16_t sample =
        source[source_index % (sizeof(source) / sizeof(source[0]))];
    tx_block[2 * i] = sample / (1 << att);
    tx_block[2 * i + 1] = sample / (1 << att); // data[r_idx] / (1 << att);
  }
}

#define NUM_BLOCKS 20
#define BLOCK_SIZE (2 * 2 * SAMPLE_NO)

K_MEM_SLAB_DEFINE(tx_0_mem_slab, BLOCK_SIZE, NUM_BLOCKS, 2);

int main(void) {
  struct i2s_config i2s_cfg;
  int ret;
  const struct device *dev_i2s = DEVICE_DT_GET(DT_ALIAS(i2s_tx));

  if (!device_is_ready(dev_i2s)) {
    printf("I2S device not ready\n");
    return -ENODEV;
  }
  printf("I2S device ready\n");
  /* Configure I2S stream */
  i2s_cfg.word_size = 16U;
  i2s_cfg.channels = 2U;
  i2s_cfg.format = I2S_FMT_DATA_FORMAT_I2S;
  i2s_cfg.frame_clk_freq = 8000;
  i2s_cfg.block_size = BLOCK_SIZE;
  i2s_cfg.timeout = 2000;
  /* Configure the Transmit port as Master */
  i2s_cfg.options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER;
  i2s_cfg.mem_slab = &tx_0_mem_slab;
  ret = i2s_configure(dev_i2s, I2S_DIR_TX, &i2s_cfg);
  if (ret < 0) {
    printf("Failed to configure I2S stream\n");
    return ret;
  }
  printf("I2S stream ready\n");
  int num_samples = (int)(sizeof(source) / sizeof(int16_t));
  printf("Sampoles: %d\n", num_samples);
  int source_block_ind = 0;
  void *my_block;
  ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
  fill_buf((uint16_t *)my_block, source_block_ind, 1);
  source_block_ind++;

  /* Send first block */
  ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
  if (ret < 0) {
    printf("Could not write TX buffer: err %d\n", ret);
    return ret;
  }
  printf("Wrote one block\n");
  /* Trigger the I2S transmission */
  ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
  if (ret < 0) {
    printf("Could not trigger I2S tx: err %d\n", ret);
    return ret;
  }
  printf("Started\n");

  for (int i = 0; i < 10000; i++) {
    void *my_block;
    int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
    if (ret < 0) {
      printf("Failed to allocate TX block\n");
      return ret;
    }
    fill_buf((uint16_t *)my_block, source_block_ind, 1);
    source_block_ind++;

    ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
    if (ret < 0) {
      printf("Could not write TX buffer: err %d\n", ret);
    }
  }

  /* Drain TX queue */
  //  i think this just blocks until it runs out of data then goes back to the
  //  stopped state
  ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
  if (ret < 0) {
    printf("Could not trigger I2S tx\n");
    return ret;
  }

  printf("Finished\n");
  return 0;
}
