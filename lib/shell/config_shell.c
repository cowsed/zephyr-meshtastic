#include <string.h>
#include <zephyr/shell/shell.h>

static int cmd_set_meshtastic_name(const struct shell *sh, size_t argc,
                                   char **argv) {
  if (argc < 3) {
    shell_print(sh,
                "Usage: meshtastic_config set_name shortname(<=4 charactes) "
                "longname(<=)");
    return -1;
  }

  //   int shortname_len = strnlen(argv[0], 5);
  // int longname_len = strnlen(argv[0], 5);

  shell_print(sh, "setting meshtastic name");

  return 0;
}

static int cmd_get_meshtastic_name(const struct shell *sh, size_t argc,
                                   char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(sh, "getting meshtastic name");

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_meshtastic_config,
    SHELL_CMD(set_name, NULL, "Set meshtastic name: [short_name] [long_name].",
              cmd_set_meshtastic_name),
    SHELL_CMD(get_name, NULL, "Get meshtastic name: shortname longname.",
              cmd_get_meshtastic_name),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(meshtastic_config, &sub_meshtastic_config,
                   "Meshtastic Config Commands", NULL);
