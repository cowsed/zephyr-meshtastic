/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/drivers/i2s.h>

#include <ff.h>
#include <lvgl.h>
#include <stdio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/fs/fs.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>

#include <zephyr/sys/iterable_sections.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#include "fish.c"

#define SAMPLE_NO 256
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
  i2s_cfg.frame_clk_freq = 48000;
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

#define DISK_DRIVE_NAME "SD"

#define DISK_MOUNT_PT "/" DISK_DRIVE_NAME ":"

static FATFS fat_fs;
/* mounting info */
static struct fs_mount_t mp = {
    .type = FS_FATFS,
    .fs_data = &fat_fs,
};
#define FS_RET_OK FR_OK

static const char *disk_mount_pt = DISK_MOUNT_PT;

int readblock(struct fs_file_t *fp, void *into) {
  int16_t buf[SAMPLE_NO] = {0};
  int num_read = fs_read(fp, buf, sizeof(buf));
  int16_t *outp = into;
  int att = 0;
  for (int i = 0; i < SAMPLE_NO; i++) {
    outp[i * 2] = buf[i] >> att;
    outp[i * 2 + 1] = buf[i] >> att;
  }
  return num_read;
}

int play_music() {
  printf("Starting playback\n");
  struct fs_file_t fil;
  fs_file_t_init(&fil);
  int ret = fs_open(&fil, "/SD:/Audio/hottogo.48000.wav", FS_O_READ);
  if (ret != 0) {
    printf("Error opening file: %d\n", ret);
  }

  const struct device *dev_i2s = DEVICE_DT_GET(DT_ALIAS(i2s_tx));
  setup_i2s(dev_i2s);
  printf("I2S ready\n");

  void *my_block;
  ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
  if (ret != 0) {
    printf("error allocating\n");
  }
  int num_read = readblock(&fil, my_block);
  printf("num read: %d\n", num_read);

  /* Send first block */
  ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
  if (ret < 0) {
    printf("Could not write TX buffer: err %d\n", ret);
    return ret;
  }

  /* Trigger the I2S transmission */
  ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
  if (ret < 0) {
    printf("Could not trigger I2S tx: err %d\n", ret);
    return ret;
  }
  printf("Started\n");
  //
  while (num_read == SAMPLE_NO * 2) {
    void *my_block;
    int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
    if (ret < 0) {
      printf("Failed to allocate TX block\n");
      return ret;
    }
    num_read = readblock(&fil, my_block);

    ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
    if (ret < 0) {
      printf("Could not write TX buffer: err %d\n", ret);
    }
  }
  //
  /* Drain TX queue */
  //  i think this just blocks until it runs out of data then goes back to
  // the
  //  stopped state
  ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
  if (ret < 0) {
    printf("Could not trigger I2S tx\n");
    return ret;
  }
  return 0;
}

uint8_t rgbto565(uint8_t r, uint8_t b, uint8_t g) { return 0; }

void copy_fb_to_screen_tex(uint8_t fb[240][320 * 3], uint8_t *buf, int w,
                           int h) {

  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      uint8_t r = fb[y][x * 3];
      uint8_t g = fb[y][x * 3 + 1];
      uint8_t b = fb[y][x * 3 + 2];
      uint16_t out = rgbto565(r, g, b);
      out = 0;
      buf[y * (w) + (x * 2)] = (out >> 8);
      buf[y * (w) + (x * 2) + 1] = (out && 0xff);
    }
  }
}

int main() {

  /* raw disk i/o */
  do {
    static const char *disk_pdrv = DISK_DRIVE_NAME;
    uint64_t memory_size_mb;
    uint32_t block_count;
    uint32_t block_size;

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_CTRL_INIT, NULL) != 0) {
      LOG_ERR("Storage init ERROR!");
      break;
    }

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT,
                          &block_count)) {
      LOG_ERR("Unable to get sector count");
      break;
    }
    LOG_INF("Block count %u", block_count);

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) {
      LOG_ERR("Unable to get sector size");
      break;
    }
    printk("Sector size %u\n", block_size);

    memory_size_mb = (uint64_t)block_count * block_size;
    printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

    if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_CTRL_DEINIT, NULL) != 0) {
      LOG_ERR("Storage deinit ERROR!");
      break;
    }
  } while (0);

  mp.mnt_point = disk_mount_pt;

  printf("mounting\n");

  int res = fs_mount(&mp);

  if (res == FS_RET_OK) {
    printk("Disk mounted.\n");
  } else {
    printk("Disk not mounted :(\n");
  }

  const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(display_dev)) {
    LOG_ERR("Device %s not found. Aborting sample.", display_dev->name);
    return 0;
  }

  // return 0;

  lv_obj_t *img1 = lv_image_create(lv_screen_active());
  lv_image_set_src(img1, &fish);
  lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

  lv_timer_handler();
  display_blanking_off(display_dev);

  // while (1) {
  // if ((count % 100) == 0U) {
  // sprintf(count_str, "%d", count / 100U);
  // lv_label_set_text(count_label, count_str);
  // }
  // lv_timer_handler();
  // ++count;
  // k_sleep(K_MSEC(10));
  // }
  //
  // size_t buf_size = 2 * fish_png_height * fish_png_width;
  // uint8_t *buf = k_malloc(buf_size);
  // if (buf == NULL) {
  //   LOG_ERR("Could not allocate memory. Aborting sample.");
  //   return 0;
  // }
  // printf("Going\n");
  // struct display_buffer_descriptor buf_desc = {
  //     .buf_size = buf_size,
  //     .width = fish_png_width,
  //     .height = fish_png_width,
  //     .pitch = fish_png_width * 2,
  //     .frame_incomplete = false,
  // };

  // copy_fb_to_screen_tex(fish_png_data, buf, 320, 240);
  // printf("Copied\n");
  // res = display_write(display_dev, 0, 0, &buf_desc, buf);
  // printf("Wrote: %d\n", res);

  // display_blanking_off(display_dev);
  // res = display_write(display_dev, 0, 0, &buf_desc, buf);

  play_music();
  printf("Done\n");

  return 0;
}