/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/iterable_sections.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define MINIMP3_NONSTANDARD_BUT_LOGICAL
#define MINIMP3_ONLY_MP3
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

extern unsigned char *somewhatgraphicultramodern_mp3;
extern unsigned int somewhatgraphicultramodern_mp3_len;

#define SAMPLE_NO 128
#define NUM_BLOCKS 20
#define BLOCK_SIZE (2 * 2 * SAMPLE_NO)

K_MEM_SLAB_DEFINE(tx_0_mem_slab, BLOCK_SIZE, NUM_BLOCKS, 2);

int setup_i2s(const struct device *dev_i2s) {
  struct i2s_config i2s_cfg;
  int ret;

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
  return 0;
}

static mp3dec_t mp3d;
short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];

int main(void) {
  const struct device *dev_i2s = DEVICE_DT_GET(DT_ALIAS(i2s_tx));
  setup_i2s(dev_i2s);
  printf("I2S ready\n");

  mp3dec_init(&mp3d);
  printf("mp3dec ready");

  uint8_t *input_buf = somewhatgraphicultramodern_mp3;
  unsigned int buf_size = somewhatgraphicultramodern_mp3_len;

  mp3dec_frame_info_t info;
  /*unsigned char *input_buf; - input byte stream*/
  int samples = mp3dec_decode_frame(&mp3d, input_buf, buf_size, pcm, &info);
  printf("Decoded %d samples\n\n", samples);
  return 0;
  //   int num_samples = (int)(sizeof(source) / sizeof(int16_t));
  //   printf("Sampoles: %d\n", num_samples);

  //   int source_block_ind = 0;
  //   void *my_block;
  //   int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
  //   fill_buf((uint16_t *)my_block, source_block_ind, 1);
  //   source_block_ind++;

  //   /* Send first block */
  //   ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
  //   if (ret < 0) {
  //     printf("Could not write TX buffer: err %d\n", ret);
  //     return ret;
  //   }
  //   printf("Wrote one block\n");
  //   /* Trigger the I2S transmission */
  //   ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
  //   if (ret < 0) {
  //     printf("Could not trigger I2S tx: err %d\n", ret);
  //     return ret;
  //   }
  //   printf("Started\n");

  //   for (int i = 0; i < 10000; i++) {
  //     void *my_block;
  //     int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
  //     if (ret < 0) {
  //       printf("Failed to allocate TX block\n");
  //       return ret;
  //     }
  //     fill_buf((uint16_t *)my_block, source_block_ind, 1);
  //     source_block_ind++;

  //     ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
  //     if (ret < 0) {
  //       printf("Could not write TX buffer: err %d\n", ret);
  //     }
  //   }

  //   /* Drain TX queue */
  //   //  i think this just blocks until it runs out of data then goes back to
  //   the
  //   //  stopped state
  //   ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
  //   if (ret < 0) {
  //     printf("Could not trigger I2S tx\n");
  //     return ret;
  //   }

  printf("Finished\n");
  return 0;
}
