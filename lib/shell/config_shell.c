#include "config/config.h"
#include <string.h>
#include <zephyr/shell/shell.h>
size_t strnlen(const char *s, size_t maxlen) {
  for (size_t i = 0; i < maxlen; ++i) {
    if (s[i] == '\0') {
      return i;
    }
  }
  return maxlen;
}
char *hardware_model_names[256] = {
    [meshtastic_HardwareModel_UNSET] = "UNSET",
    [meshtastic_HardwareModel_TLORA_V2] = "TLORA_V2",
    [meshtastic_HardwareModel_TLORA_V1] = "TLORA_V1",
    [meshtastic_HardwareModel_TLORA_V2_1_1P6] = "TLORA_V2_1_1P6",
    [meshtastic_HardwareModel_TBEAM] = "TBEAM",
    [meshtastic_HardwareModel_HELTEC_V2_0] = "HELTEC_V2_0",
    [meshtastic_HardwareModel_TBEAM_V0P7] = "TBEAM_V0P7",
    [meshtastic_HardwareModel_T_ECHO] = "T_ECHO",
    [meshtastic_HardwareModel_TLORA_V1_1P3] = "TLORA_V1_1P3",
    [meshtastic_HardwareModel_RAK4631] = "RAK4631",
    [meshtastic_HardwareModel_HELTEC_V2_1] = "HELTEC_V2_1",
    [meshtastic_HardwareModel_HELTEC_V1] = "HELTEC_V1",
    [meshtastic_HardwareModel_LILYGO_TBEAM_S3_CORE] = "LILYGO_TBEAM_S3_CORE",
    [meshtastic_HardwareModel_RAK11200] = "RAK11200",
    [meshtastic_HardwareModel_NANO_G1] = "NANO_G1",
    [meshtastic_HardwareModel_TLORA_V2_1_1P8] = "TLORA_V2_1_1P8",
    [meshtastic_HardwareModel_TLORA_T3_S3] = "TLORA_T3_S3",
    [meshtastic_HardwareModel_NANO_G1_EXPLORER] = "NANO_G1_EXPLORER",
    [meshtastic_HardwareModel_NANO_G2_ULTRA] = "NANO_G2_ULTRA",
    [meshtastic_HardwareModel_LORA_TYPE] = "LORA_TYPE",
    [meshtastic_HardwareModel_WIPHONE] = "WIPHONE",
    [meshtastic_HardwareModel_WIO_WM1110] = "WIO_WM1110",
    [meshtastic_HardwareModel_RAK2560] = "RAK2560",
    [meshtastic_HardwareModel_HELTEC_HRU_3601] = "HELTEC_HRU_3601",
    [meshtastic_HardwareModel_HELTEC_WIRELESS_BRIDGE] =
        "HELTEC_WIRELESS_BRIDGE",
    [meshtastic_HardwareModel_STATION_G1] = "STATION_G1",
    [meshtastic_HardwareModel_RAK11310] = "RAK11310",
    [meshtastic_HardwareModel_SENSELORA_RP2040] = "SENSELORA_RP2040",
    [meshtastic_HardwareModel_SENSELORA_S3] = "SENSELORA_S3",
    [meshtastic_HardwareModel_CANARYONE] = "CANARYONE",
    [meshtastic_HardwareModel_RP2040_LORA] = "RP2040_LORA",
    [meshtastic_HardwareModel_STATION_G2] = "STATION_G2",
    [meshtastic_HardwareModel_LORA_RELAY_V1] = "LORA_RELAY_V1",
    [meshtastic_HardwareModel_NRF52840DK] = "NRF52840DK",
    [meshtastic_HardwareModel_PPR] = "PPR",
    [meshtastic_HardwareModel_GENIEBLOCKS] = "GENIEBLOCKS",
    [meshtastic_HardwareModel_NRF52_UNKNOWN] = "NRF52_UNKNOWN",
    [meshtastic_HardwareModel_PORTDUINO] = "PORTDUINO",
    [meshtastic_HardwareModel_ANDROID_SIM] = "ANDROID_SIM",
    [meshtastic_HardwareModel_DIY_V1] = "DIY_V1",
    [meshtastic_HardwareModel_NRF52840_PCA10059] = "NRF52840_PCA10059",
    [meshtastic_HardwareModel_DR_DEV] = "DR_DEV",
    [meshtastic_HardwareModel_M5STACK] = "M5STACK",
    [meshtastic_HardwareModel_HELTEC_V3] = "HELTEC_V3",
    [meshtastic_HardwareModel_HELTEC_WSL_V3] = "HELTEC_WSL_V3",
    [meshtastic_HardwareModel_BETAFPV_2400_TX] = "BETAFPV_2400_TX",
    [meshtastic_HardwareModel_BETAFPV_900_NANO_TX] = "BETAFPV_900_NANO_TX",
    [meshtastic_HardwareModel_RPI_PICO] = "RPI_PICO",
    [meshtastic_HardwareModel_HELTEC_WIRELESS_TRACKER] =
        "HELTEC_WIRELESS_TRACKER",
    [meshtastic_HardwareModel_HELTEC_WIRELESS_PAPER] = "HELTEC_WIRELESS_PAPER",
    [meshtastic_HardwareModel_T_DECK] = "T_DECK",
    [meshtastic_HardwareModel_T_WATCH_S3] = "T_WATCH_S3",
    [meshtastic_HardwareModel_PICOMPUTER_S3] = "PICOMPUTER_S3",
    [meshtastic_HardwareModel_HELTEC_HT62] = "HELTEC_HT62",
    [meshtastic_HardwareModel_EBYTE_ESP32_S3] = "EBYTE_ESP32_S3",
    [meshtastic_HardwareModel_ESP32_S3_PICO] = "ESP32_S3_PICO",
    [meshtastic_HardwareModel_CHATTER_2] = "CHATTER_2",
    [meshtastic_HardwareModel_HELTEC_WIRELESS_PAPER_V1_0] =
        "HELTEC_WIRELESS_PAPER_V1_0",
    [meshtastic_HardwareModel_HELTEC_WIRELESS_TRACKER_V1_0] =
        "HELTEC_WIRELESS_TRACKER_V1_0",
    [meshtastic_HardwareModel_UNPHONE] = "UNPHONE",
    [meshtastic_HardwareModel_TD_LORAC] = "TD_LORAC",
    [meshtastic_HardwareModel_CDEBYTE_EORA_S3] = "CDEBYTE_EORA_S3",
    [meshtastic_HardwareModel_TWC_MESH_V4] = "TWC_MESH_V4",
    [meshtastic_HardwareModel_NRF52_PROMICRO_DIY] = "NRF52_PROMICRO_DIY",
    [meshtastic_HardwareModel_RADIOMASTER_900_BANDIT_NANO] =
        "RADIOMASTER_900_BANDIT_NANO",
    [meshtastic_HardwareModel_HELTEC_CAPSULE_SENSOR_V3] =
        "HELTEC_CAPSULE_SENSOR_V3",
    [meshtastic_HardwareModel_HELTEC_VISION_MASTER_T190] =
        "HELTEC_VISION_MASTER_T190",
    [meshtastic_HardwareModel_HELTEC_VISION_MASTER_E213] =
        "HELTEC_VISION_MASTER_E213",
    [meshtastic_HardwareModel_HELTEC_VISION_MASTER_E290] =
        "HELTEC_VISION_MASTER_E290",
    [meshtastic_HardwareModel_HELTEC_MESH_NODE_T114] = "HELTEC_MESH_NODE_T114",
    [meshtastic_HardwareModel_SENSECAP_INDICATOR] = "SENSECAP_INDICATOR",
    [meshtastic_HardwareModel_TRACKER_T1000_E] = "TRACKER_T1000_E",
    [meshtastic_HardwareModel_RAK3172] = "RAK3172",
    [meshtastic_HardwareModel_WIO_E5] = "WIO_E5",
    [meshtastic_HardwareModel_RADIOMASTER_900_BANDIT] =
        "RADIOMASTER_900_BANDIT",
    [meshtastic_HardwareModel_ME25LS01_4Y10TD] = "ME25LS01_4Y10TD",
    [meshtastic_HardwareModel_RP2040_FEATHER_RFM95] = "RP2040_FEATHER_RFM95",
    [meshtastic_HardwareModel_M5STACK_COREBASIC] = "M5STACK_COREBASIC",
    [meshtastic_HardwareModel_M5STACK_CORE2] = "M5STACK_CORE2",
    [meshtastic_HardwareModel_RPI_PICO2] = "RPI_PICO2",
    [meshtastic_HardwareModel_M5STACK_CORES3] = "M5STACK_CORES3",
    [meshtastic_HardwareModel_SEEED_XIAO_S3] = "SEEED_XIAO_S3",
    [meshtastic_HardwareModel_MS24SF1] = "MS24SF1",
    [meshtastic_HardwareModel_TLORA_C6] = "TLORA_C6",
    [meshtastic_HardwareModel_WISMESH_TAP] = "WISMESH_TAP",
    [meshtastic_HardwareModel_ROUTASTIC] = "ROUTASTIC",
    [meshtastic_HardwareModel_MESH_TAB] = "MESH_TAB",
    [meshtastic_HardwareModel_MESHLINK] = "MESHLINK",
    [meshtastic_HardwareModel_XIAO_NRF52_KIT] = "XIAO_NRF52_KIT",
    [meshtastic_HardwareModel_PRIVATE_HW] = "PRIVATE_HW",
};

char *device_role_names[256] = {
    [meshtastic_Config_DeviceConfig_Role_CLIENT] = "CLIENT",
    [meshtastic_Config_DeviceConfig_Role_CLIENT_MUTE] = "CLIENT_MUTE",
    [meshtastic_Config_DeviceConfig_Role_ROUTER] = "ROUTER",
    [meshtastic_Config_DeviceConfig_Role_ROUTER_CLIENT] = "ROUTER_CLIENT",
    [meshtastic_Config_DeviceConfig_Role_REPEATER] = "REPEATER",
    [meshtastic_Config_DeviceConfig_Role_TRACKER] = "TRACKER",
    [meshtastic_Config_DeviceConfig_Role_SENSOR] = "SENSOR",
    [meshtastic_Config_DeviceConfig_Role_TAK] = "TAK",
    [meshtastic_Config_DeviceConfig_Role_CLIENT_HIDDEN] = "CLIENT_HIDDEN",
    [meshtastic_Config_DeviceConfig_Role_LOST_AND_FOUND] = "LOST_AND_FOUND",
    [meshtastic_Config_DeviceConfig_Role_TAK_TRACKER] = "TAK_TRACKER",
    [meshtastic_Config_DeviceConfig_Role_ROUTER_LATE] = "ROUTER_LATE",
};

void shell_print_user(const struct shell *sh, const meshtastic_User *usr) {
  shell_print(sh, "User:");
  shell_print(sh, "  ID(hex):    %.*s", sizeof(usr->id), usr->id);
  shell_print(sh, "  Short Name: %.*s", sizeof(usr->short_name),
              usr->short_name);
  shell_print(sh, "  Long Name:  %.*s", sizeof(usr->long_name), usr->long_name);
  shell_print(sh, "  Hardware:   %s", hardware_model_names[usr->hw_model]);
  shell_print(sh, "  Licensed:   %s", (usr->is_licensed ? "Yes" : "No"));
  shell_print(sh, "  Role:       %s", device_role_names[usr->role]);
}

static int cmd_set_meshtastic_name(const struct shell *sh, size_t argc,
                                   char **argv) {
  if (argc < 3) {
    shell_print(sh,
                "Usage: meshtastic_config set_name shortname(<=4 charactes) "
                "longname(<=)");
    return -1;
  }
  char *shortname = argv[1];
  if (strnlen(shortname, 5) > 4) {
    shell_print(sh, "shortname must be 4 characters or less");
    return -1;
  }

  char *longname = argv[2];
  if (strnlen(longname, 41) > 40) {
    shell_print(sh, "longname must be 40 characters or less");
    return -1;
  }

  meshtastic_User user = {0};
  get_user_config(&user);

  strncpy(user.short_name, shortname, sizeof(user.short_name));
  strncpy(user.long_name, longname, sizeof(user.long_name));

  shell_print_user(sh, &user);

  return set_user_config(&user);
}

static int cmd_get_meshtastic_user(const struct shell *sh, size_t argc,
                                   char **argv) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  meshtastic_User user = {0};
  get_user_config(&user);
  shell_print_user(sh, &user);

  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_meshtastic_config,
    SHELL_CMD(set_usernames, NULL,
              "Set meshtastic name: [short_name] [long_name].",
              cmd_set_meshtastic_name),
    SHELL_CMD(get_user, NULL, "Print meshtastic user config",
              cmd_get_meshtastic_user),
    SHELL_SUBCMD_SET_END /* Array terminated. */
);
SHELL_CMD_REGISTER(meshtastic_config, &sub_meshtastic_config,
                   "Meshtastic Config Commands", NULL);
