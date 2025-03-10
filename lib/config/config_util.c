#include "config_util.h"

struct read_config_string_args {
  uint8_t *buf;
  size_t len;
  size_t *num_read;
};
int read_config_string_callback_fn(const char *key, size_t len,
                                   settings_read_cb read_cb, void *cb_arg,
                                   void *param) {
  struct read_config_string_args *readstr =
      (struct read_config_string_args *)param;
  int ret = read_cb(cb_arg, readstr->buf, readstr->len);
  *(readstr->num_read) = ret;
  return ret;
}

int read_config_string(const char *key, uint8_t *buf, size_t buf_len,
                       size_t *num_read) {
  struct read_config_string_args args = {buf, buf_len, num_read};
  return settings_load_subtree_direct(key, read_config_string_callback_fn,
                                      &args);
}
