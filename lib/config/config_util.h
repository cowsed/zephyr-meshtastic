#ifndef ZEPHYR_MESHTASTIC_CONFIG_UTIL_H
#define ZEPHYR_MESHTASTIC_CONFIG_UTIL_H

#include "zephyr/settings/settings.h"
#include <pb_decode.h>
#include <pb_encode.h>
#include <zephyr/sys/base64.h>

#define BASE64_OVERHEAD(n) ((n * 4) / 3 + 1)

#define PROTO64_DECODE(inbuffer, insize, data, type)                           \
  char data##_protobuf[type##_size] = {0};                                     \
  int data##_rc = 0;                                                           \
  do {                                                                         \
    size_t num_writ = 0;                                                       \
    data##_rc = base64_decode(data##_protobuf, type##_size, &num_writ,         \
                              inbuffer, insize);                               \
    if (data##_rc != 0) {                                                      \
      LOG_ERR("Failed to base64_decode " #type ": %d", data##_rc);             \
      break;                                                                   \
    }                                                                          \
    pb_istream_t istream = pb_istream_from_buffer(data##_protobuf, num_writ);  \
    bool out = pb_decode(&istream, type##_fields, data);                       \
    if (!out) {                                                                \
      LOG_ERR("Failed to decoded protobuf of " #type ": %s",                   \
              PB_GET_ERROR(&istream));                                         \
      data##_rc = -1;                                                          \
    }                                                                          \
                                                                               \
  } while (0);

#define PROTO64_ENCODE(data, type)                                             \
  int data##_rc = 0;                                                           \
  char data##_encoded[BASE64_OVERHEAD(type##_size)];                           \
  char data##_protobuf[type##_size];                                           \
  pb_ostream_t data##stream =                                                  \
      pb_ostream_from_buffer(data##_protobuf, type##_size);                    \
  int data##_num_written = 0;                                                  \
  do {                                                                         \
    data##_rc = pb_encode(&data##stream, type##_fields, user);                 \
    int reallen = data##stream.bytes_written;                                  \
    if (!(data##_rc)) {                                                        \
      LOG_ERR("Protobuf Encoding Failed: %s", PB_GET_ERROR(&data##stream));    \
      break;                                                                   \
    }                                                                          \
    data##_rc = base64_encode(data##_encoded, BASE64_OVERHEAD(type##_size),    \
                              &data##_num_written, data##_protobuf, reallen);  \
    if ((data##_rc) != 0) {                                                    \
      LOG_ERR("B64 Encoding Failed: %d", (data##_rc));                         \
      break;                                                                   \
    }                                                                          \
  } while (0);

int read_config_string(const char *key, uint8_t *buf, size_t buf_len,
                       size_t *num_read);

#endif