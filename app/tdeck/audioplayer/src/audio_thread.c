#include <stddef.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/fs/fs.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/disk_access.h>
LOG_MODULE_REGISTER(audio, LOG_LEVEL_INF);

static const char *current_file_name = NULL;
static size_t file_size = 0;
static size_t samples_per_second = 0;
static struct fs_file_t current_file;
static bool paused = false;
static bool newfile = false;

bool has_current() { return current_file_name != NULL; }

#define SAMPLE_NO 128
#define NUM_BLOCKS 40

#define BLOCK_SIZE (2 * 2 * SAMPLE_NO)

K_MEM_SLAB_DEFINE(tx_0_mem_slab, BLOCK_SIZE, NUM_BLOCKS, 2);

int setup_i2s(const struct device *dev_i2s, size_t clk_freq) {
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
  i2s_cfg.frame_clk_freq = clk_freq;
  i2s_cfg.block_size = BLOCK_SIZE;
  i2s_cfg.timeout = 15000;
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

int readblock(struct fs_file_t *fp, void *into) {
  int16_t buf[SAMPLE_NO] = {0};
  int num_read = fs_read(fp, buf, sizeof(buf));
  int16_t *outp = into;
  int att = 5;
  for (int i = 0; i < SAMPLE_NO; i++) {
    outp[i * 2] = buf[i] >> att;
    outp[i * 2 + 1] = buf[i] >> att;
  }
  return num_read;
}

int play_file(const char *filename) { return open_current(filename); }

void pause() {}

void resume() {}

#define AUDIO_STACK_SIZE 4096
#define AUDIO_PRIORITY 5

int close_current() {
  if (!has_current()) {
    return 0;
  }
  int ret = fs_close(&current_file);
  file_size = 0;
  samples_per_second = 0;
  paused = false;
  current_file_name = NULL;
  return ret;
}

int open_current(const char *fname) {
  int ret = fs_open(&current_file, fname, FS_O_READ);
  if (ret != 0) {
    return ret;
  }
  current_file_name = fname;
  newfile = true;
  fs_seek(&current_file, 0, FS_SEEK_END);
  size_t size = fs_tell(&current_file);
  file_size = size;
  fs_seek(&current_file, 0, FS_SEEK_SET);
  return 0;
}

extern int audio_entry_point(void *, void *, void *) {
  fs_file_t_init(&current_file);
  const struct device *dev_i2s = DEVICE_DT_GET(DT_ALIAS(i2s_tx));
  setup_i2s(dev_i2s, 8000);
  while (true) {
    if (!has_current()) {
      LOG_INF("No Current File");
      k_msleep(100);
      continue;
    }
    if (!paused) {
      void *my_block;
      int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
      if (ret != 0) {
        LOG_ERR("error allocating\n");
      }
      int num_read = readblock(&current_file, my_block);
      if (num_read != SAMPLE_NO * 2) {
        LOG_INF("Read %d of %d", num_read, SAMPLE_NO * 2);
        ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
        if (ret < 0) {
          LOG_ERR("Could not trigger I2S tx\n");
          continue;
        }
        close_current();
      }
      /* Send block */
      ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
      if (ret < 0) {
        printf("Could not write TX buffer: err %d\n", ret);
        // ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
        // if (ret < 0) {
        //   printf("Could not trigger I2S tx\n");
        // return ret;
        // }
        // newfile = true;
      }

      if (newfile) {
        /* Trigger the I2S transmission */
        ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
        newfile = false;
        if (ret < 0) {
          newfile = true;
          printf("Could not trigger I2S tx: err %d\n", ret);
          continue;
        }
      }
    }
  }
}

//   printf("Starting playback\n");
//   struct fs_file_t fil;
//   int ret = fs_open(&fil, "/SD:/Audio/spinningfish.raw", FS_O_READ);
//   if (ret != 0) {
//     printf("Error opening file: %d\n", ret);
//     return 0;
//   }

//   printf("I2S ready\n");
//   printf("num read: %d\n", num_read);

//   /* Send first block */
//   ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
//   if (ret < 0) {
//     printf("Could not write TX buffer: err %d\n", ret);
//     return ret;
//   }

//   /* Trigger the I2S transmission */
//   ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
//   if (ret < 0) {
//     printf("Could not trigger I2S tx: err %d\n", ret);
//     return ret;
//   }
//   printf("Started\n");
//   //
//   while (num_read == SAMPLE_NO * 2) {
//     void *my_block;
//     int ret = k_mem_slab_alloc(&tx_0_mem_slab, &my_block, K_FOREVER);
//     if (ret < 0) {
//       printf("Failed to allocate TX block\n");
//       return ret;
//     }
//     num_read = readblock(&fil, my_block);

//     ret = i2s_write(dev_i2s, my_block, BLOCK_SIZE);
//     if (ret < 0) {
//       printf("Could not write TX buffer: err %d\n", ret);
//     }
//   }
//   //
//   /* Drain TX queue */
//   //  i think this just blocks until it runs out of data then goes back to
//   // the
//   //  stopped state
//   ret = i2s_trigger(dev_i2s, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
//   if (ret < 0) {
//     printf("Could not trigger I2S tx\n");
//     return ret;
//   }
//   printf("Done\n");
//   return 0;
// }

K_THREAD_DEFINE(audio_tid, AUDIO_STACK_SIZE, audio_entry_point, NULL, NULL,
                NULL, AUDIO_PRIORITY, 0, 1000);