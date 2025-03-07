/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <stdlib.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/version.h>


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(meshtastic_shell);

extern void foo(void);

static int cmd_demo_ping(const struct shell *sh, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(sh, "pong");

  return 0;
}

static int cmd_demo_board(const struct shell *sh, size_t argc, char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(sh, CONFIG_BOARD);

  return 0;
}

static int cmd_demo_params(const struct shell *sh, size_t argc, char **argv) {
  shell_print(sh, "argc = %zd", argc);
  for (size_t cnt = 0; cnt < argc; cnt++) {
    shell_print(sh, "  argv[%zd] = %s", cnt, argv[cnt]);
  }

  return 0;
}

static int cmd_demo_hexdump(const struct shell *sh, size_t argc, char **argv) {
  shell_print(sh, "argc = %zd", argc);
  for (size_t cnt = 0; cnt < argc; cnt++) {
    shell_print(sh, "argv[%zd]", cnt);
    shell_hexdump(sh, argv[cnt], strlen(argv[cnt]));
  }

  return 0;
}

static int cmd_dict(const struct shell *sh, size_t argc, char **argv,
                    void *data) {
  int val = (intptr_t)data;

  shell_print(sh, "(syntax, value) : (%s, %d)", argv[0], val);

  return 0;
}

SHELL_SUBCMD_DICT_SET_CREATE(sub_dict_cmds, cmd_dict, (value_0, 0, "value 0"),
                             (value_1, 1, "value 1"), (value_2, 2, "value 2"),
                             (value_3, 3, "value 3"));

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_demo,
    SHELL_CMD(dictionary, &sub_dict_cmds, "Dictionary commands", NULL),
    SHELL_CMD(hexdump, NULL, "Hexdump params command.", cmd_demo_hexdump),
    SHELL_CMD(params, NULL, "Print params command.", cmd_demo_params),
    SHELL_CMD(ping, NULL, "Ping command.", cmd_demo_ping),
    SHELL_CMD(board, NULL, "Show board name command.", cmd_demo_board),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(demo, &sub_demo, "Demo commands", NULL);

static int cmd_meshtastic_mock_level0(const struct shell *sh, size_t argc,
                                      char **argv) {
  shell_print(sh, "argc = %zd", argc);
  for (size_t cnt = 0; cnt < argc; cnt++) {
    shell_print(sh, "argv[%zd]", cnt);
    shell_hexdump(sh, argv[cnt], strlen(argv[cnt]));
  }

  return 0;
}

// Mock Commands

static int cmd_mock_lvl0_recv(const struct shell *sh, size_t argc,
                              char **argv) {
  shell_print(sh, "level0 mock receive");
  return 0;
}
static int cmd_mock_lvl1_recv(const struct shell *sh, size_t argc,
                              char **argv) {
  shell_print(sh, "level1 mock receive");
  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_meshtastic_mock,
    SHELL_CMD(lvl0_recv, NULL, "Mock level 0 receive.", cmd_mock_lvl0_recv),
    SHELL_CMD(lvl1_recv, NULL, "Mock level 1 receive.", cmd_mock_lvl1_recv),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(meshtastic_mock, &sub_meshtastic_mock, "Demo commands",
                   NULL);
