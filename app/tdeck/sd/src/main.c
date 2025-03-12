/*
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/fs/fs.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/sys/iterable_sections.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#include <ff.h>

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

int main() {
  printf("Hello\n");
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

  int res = fs_mount(&mp);

  if (res == FS_RET_OK) {
    printk("Disk mounted.\n");
    /* Try to unmount and remount the disk */
    res = fs_unmount(&mp);
    if (res != FS_RET_OK) {
      printk("Error unmounting disk\n");
      return res;
    }
    res = fs_mount(&mp);
    if (res != FS_RET_OK) {
      printk("Error remounting disk\n");
      return res;
    }
  }
  return 0;
}