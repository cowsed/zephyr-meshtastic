/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * Based on ST7789V sample:
 * Copyright (c) 2019 Marc Reilly
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sample, LOG_LEVEL_INF);

#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include "rom.h"

#include <stdint.h>
#include <stdlib.h>

#include "peanut_gb.h"

// #define LCD_WIDTH 240
// #define LCD_HEIGHT 240

struct priv_t {
  /* Pointer to allocated memory holding GB file. */
  uint8_t *rom;
  /* Pointer to allocated memory holding save file. */
  uint8_t *cart_ram;
  /* Pointer to boot ROM binary. */
  uint8_t *bootrom;

  /* Colour palette for each BG, OBJ0, and OBJ1. */
  uint16_t selected_palette[3][4];
  uint16_t fb[LCD_HEIGHT][LCD_WIDTH];
};

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t *const p = (const struct priv_t *const)gb->direct.priv;
  return p->rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
  const struct priv_t *const p = (const struct priv_t *const)gb->direct.priv;
  return p->cart_ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr,
                       const uint8_t val) {
  const struct priv_t *const p = (const struct priv_t *const)gb->direct.priv;
  p->cart_ram[addr] = val;
}

uint8_t gb_bootrom_read(struct gb_s *gb, const uint_fast16_t addr) {
  const struct priv_t *const p = (const struct priv_t *const)gb->direct.priv;
  return p->bootrom[addr];
}

void read_cart_ram_file(const char *save_file_name, uint8_t **dest,
                        const size_t len) {}

void write_cart_ram_file(const char *save_file_name, uint8_t **dest,
                         const size_t len) {}

/**
 * Handles an error reported by the emulator. The emulator context may be used
 * to better understand why the error given in gb_err was reported.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err,
              const uint16_t addr) {
  const char *gb_err_str[GB_INVALID_MAX] = {"UNKNOWN", "INVALID OPCODE",
                                            "INVALID READ", "INVALID WRITE",
                                            "HALT FOREVER"};
  struct priv_t *priv = (struct priv_t *)gb->direct.priv;
  char error_msg[256];
  char location[64] = "";
  uint8_t instr_byte;

#ifdef GOOD
  /* Record save file. */
  write_cart_ram_file("recovery.sav", &priv->cart_ram, gb_get_save_size(gb));
#endif

  if (addr >= 0x4000 && addr < 0x8000) {
    uint32_t rom_addr;
    rom_addr = (uint32_t)addr * (uint32_t)gb->selected_rom_bank;
    snprintf(location, sizeof(location), " (bank %d mode %d, file offset %u)",
             gb->selected_rom_bank, gb->cart_mode_select, rom_addr);
  }

  instr_byte = __gb_read(gb, addr);

  snprintf(error_msg, sizeof(error_msg),
           "Error: %s at 0x%04X%s with instruction %02X.\n"
           "Cart RAM saved to recovery.sav\n"
           "Exiting.\n",
           gb_err_str[gb_err], addr, location, instr_byte);
  printf("%s", error_msg);

  /* Free memory and then exit. */
  free(priv->cart_ram);
  free(priv->rom);
  exit(EXIT_FAILURE);
}

/**
 * Automatically assigns a colour palette to the game using a given game
 * checksum.
 * TODO: Not all checksums are programmed in yet because I'm lazy.
 */
void auto_assign_palette(struct priv_t *priv, uint8_t game_checksum) {
  size_t palette_bytes = 3 * 4 * sizeof(uint16_t);

  switch (game_checksum) {
  /* Balloon Kid and Tetris Blast */
  case 0x71:
  case 0xFF: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Yellow and Tetris */
  case 0x15:
  case 0xDB:
  case 0x95: /* Not officially */
  {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7FE0, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Donkey Kong */
  case 0x19: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E60, 0x7C00, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Blue */
  case 0x61:
  case 0x45:

  /* Pokemon Blue Star */
  case 0xD8: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Red */
  case 0x14: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Pokemon Red Star */
  case 0x8B: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Kirby */
  case 0x27:
  case 0x49:
  case 0x5C:
  case 0xB3: {
    const uint16_t palette[3][4] = {
        {0x7D8A, 0x6800, 0x3000, 0x0000}, /* OBJ0 */
        {0x001F, 0x7FFF, 0x7FEF, 0x021F}, /* OBJ1 */
        {0x527F, 0x7FE0, 0x0180, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Donkey Kong Land [1/2/III] */
  case 0x18:
  case 0x6A:
  case 0x4B:
  case 0x6B: {
    const uint16_t palette[3][4] = {
        {0x7F08, 0x7F40, 0x48E0, 0x2400}, /* OBJ0 */
        {0x7FFF, 0x2EFF, 0x7C00, 0x001F}, /* OBJ1 */
        {0x7FFF, 0x463B, 0x2951, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Link's Awakening */
  case 0x70: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x03E0, 0x1A00, 0x0120}, /* OBJ0 */
        {0x7FFF, 0x329F, 0x001F, 0x001F}, /* OBJ1 */
        {0x7FFF, 0x7E10, 0x48E7, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* Mega Man [1/2/3] & others I don't care about. */
  case 0x01:
  case 0x10:
  case 0x29:
  case 0x52:
  case 0x5D:
  case 0x68:
  case 0x6D:
  case 0xF6: {
    const uint16_t palette[3][4] = {
        {0x7FFF, 0x329F, 0x001F, 0x0000}, /* OBJ0 */
        {0x7FFF, 0x3FE6, 0x0200, 0x0000}, /* OBJ1 */
        {0x7FFF, 0x7EAC, 0x40C0, 0x0000}  /* BG */
    };
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  default: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000}};

    printf("No palette found for 0x%02X.", game_checksum);
    memcpy(priv->selected_palette, palette, palette_bytes);
  }
  }
}

/**
 * Assigns a palette. This is used to allow the user to manually select a
 * different colour palette if one was not found automatically, or if the user
 * prefers a different colour palette.
 * selection is the requestion colour palette. This should be a maximum of
 * NUMBER_OF_PALETTES - 1. The default greyscale palette is selected otherwise.
 */
void manual_assign_palette(struct priv_t *priv, uint8_t selection) {
#define NUMBER_OF_PALETTES 12
  size_t palette_bytes = 3 * 4 * sizeof(uint16_t);

  switch (selection) {
  /* 0x05 (Right) */
  case 0: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x2BE0, 0x7D00, 0x0000},
                                    {0x7FFF, 0x2BE0, 0x7D00, 0x0000},
                                    {0x7FFF, 0x2BE0, 0x7D00, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x07 (A + Down) */
  case 1: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7FE0, 0x7C00, 0x0000},
                                    {0x7FFF, 0x7FE0, 0x7C00, 0x0000},
                                    {0x7FFF, 0x7FE0, 0x7C00, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x12 (Up) */
  case 2: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7EAC, 0x40C0, 0x0000},
                                    {0x7FFF, 0x7EAC, 0x40C0, 0x0000},
                                    {0x7FFF, 0x7EAC, 0x40C0, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x13 (B + Right) */
  case 3: {
    const uint16_t palette[3][4] = {{0x0000, 0x0210, 0x7F60, 0x7FFF},
                                    {0x0000, 0x0210, 0x7F60, 0x7FFF},
                                    {0x0000, 0x0210, 0x7F60, 0x7FFF}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x16 (B + Left, DMG Palette) */
  default:
  case 4: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000},
                                    {0x7FFF, 0x5294, 0x294A, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x17 (Down) */
  case 5: {
    const uint16_t palette[3][4] = {{0x7FF4, 0x7E52, 0x4A5F, 0x0000},
                                    {0x7FF4, 0x7E52, 0x4A5F, 0x0000},
                                    {0x7FF4, 0x7E52, 0x4A5F, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x19 (B + Up) */
  case 6: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7EAC, 0x40C0, 0x0000},
                                    {0x7FFF, 0x7EAC, 0x40C0, 0x0000},
                                    {0x7F98, 0x6670, 0x41A5, 0x2CC1}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x1C (A + Right) */
  case 7: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7E10, 0x48E7, 0x0000},
                                    {0x7FFF, 0x7E10, 0x48E7, 0x0000},
                                    {0x7FFF, 0x3FE6, 0x0198, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x0D (A + Left) */
  case 8: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7E10, 0x48E7, 0x0000},
                                    {0x7FFF, 0x7EAC, 0x40C0, 0x0000},
                                    {0x7FFF, 0x463B, 0x2951, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x10 (A + Up) */
  case 9: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x3FE6, 0x0200, 0x0000},
                                    {0x7FFF, 0x329F, 0x001F, 0x0000},
                                    {0x7FFF, 0x7E10, 0x48E7, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x18 (Left) */
  case 10: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x7E10, 0x48E7, 0x0000},
                                    {0x7FFF, 0x3FE6, 0x0200, 0x0000},
                                    {0x7FFF, 0x329F, 0x001F, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }

  /* 0x1A (B + Down) */
  case 11: {
    const uint16_t palette[3][4] = {{0x7FFF, 0x329F, 0x001F, 0x0000},
                                    {0x7FFF, 0x3FE6, 0x0200, 0x0000},
                                    {0x7FFF, 0x7FE0, 0x3D20, 0x0000}};
    memcpy(priv->selected_palette, palette, palette_bytes);
    break;
  }
  }

  return;
}

/**
 * Draws scanline into framebuffer.
 */
void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[160],
                   const uint8_t line) {
  struct priv_t *priv = (struct priv_t *)gb->direct.priv;

  for (unsigned int x = 0; x < LCD_WIDTH; x++) {
    priv->fb[line][x] = priv->selected_palette[(pixels[x] & LCD_PALETTE_ALL) >>
                                               4][pixels[x] & 3];
  }
}

void copy_fb_to_screen_tex(uint16_t fb[LCD_HEIGHT][LCD_WIDTH], uint8_t *buf,
                           int w, int h) {
  auto to_rgb16 = [](uint16_t color) {
    uint32_t r = ((color >> 10) & 0b11111);
    uint32_t g = ((color >> 5) & 0b11111);
    uint32_t b = (color & 0b11111);
    uint16_t out = (r << 11) | (g << 6) | (b);
    return out;
  };

  for (int y = 0; y < LCD_HEIGHT; y++) {
    for (int x = 0; x < LCD_WIDTH; x++) {
      uint16_t out = to_rgb16(fb[y][x]);
      buf[y * (w) + (x * 2)] = (out >> 8);
      buf[y * (w) + (x * 2) + 1] = (out && 0xff);
    }
  }
}
#include <zephyr/drivers/gpio.h>

static struct gpio_dt_spec stuff_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

int emu_main(const device *display_dev,
             struct display_buffer_descriptor *buf_desc, uint8_t *buf) {

  gpio_pin_configure_dt(&stuff_gpio, GPIO_OUTPUT);

  printk("EMU MAIN\n\n");
  struct gb_s gb;
  struct priv_t priv = {.rom = NULL, .cart_ram = NULL};
  const double target_speed_ms = 1000.0 / VERTICAL_SYNC;
  double speed_compensation = 0.0;
  uint_fast32_t new_ticks, old_ticks;
  enum gb_init_error_e gb_ret;
  unsigned int fast_mode = 1;
  unsigned int fast_mode_timer = 1;
  /* Record save file every 60 seconds. */
  int save_timer = 60;
  /* Must be freed */
  //   char *rom_file_name = NULL;
  char *save_file_name = NULL;
  int ret = EXIT_SUCCESS;
  bool run_game = true;
  uint64_t start_time = 0;

  priv.rom = rom_data;

  /* If no save file is specified, copy save file (with specific name) to
   * allocated memory. */

  printk("EMU MAIN\n\n");

  /* Initialise emulator context. */
  gb_ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write,
                   &gb_error, &priv);

  printk("INITTTED\n\n");

  switch (gb_ret) {
  case GB_INIT_NO_ERROR:
    break;

  case GB_INIT_CARTRIDGE_UNSUPPORTED:
    printf("Unsupported cartridge.\n");
    ret = EXIT_FAILURE;
    goto out;

  case GB_INIT_INVALID_CHECKSUM:
    printf("Invalid ROM: Checksum failure.\n");
    ret = EXIT_FAILURE;
    goto out;

  default:
    printf("Unknown error: %d", gb_ret);
    ret = EXIT_FAILURE;
    goto out;
  }

  printf("No dmg_boot.bin file found; disabling boot ROM\n");

  /* Load Save File. */
  // read_cart_ram_file(save_file_name, &priv.cart_ram,
  // gb_get_save_size(&gb));

  /* Set the RTC of the game cartridge. Only used by games that support it. */
  {
    // time_t rawtime;
    // time(&rawtime);
    // struct tm *timeinfo;
    // timeinfo = localtime(&rawtime);

    /* You could potentially force the game to allow the player to
     * reset the time by setting the RTC to invalid values.
     *
     * Using memset(&gb->cart_rtc, 0xFF, sizeof(gb->cart_rtc)) for
     * example causes Pokemon Gold/Silver to say "TIME NOT SET",
     * allowing the player to set the time without having some dumb
     * password.
     *
     * The memset has to be done directly to gb->cart_rtc because
     * gb_set_rtc() processes the input values, which may cause
     * games to not detect invalid values.
     */

    /* Set RTC. Only games that specify support for RTC will use
     * these values. */
    // gb_set_rtc(&gb, timeinfo);
  }

  gb_init_lcd(&gb, lcd_draw_line);
  printk("Initted LCD\n");

  /* Open the first available controller. */

  /* 12 for "Peanut-SDL: " and a maximum of 16 for the title. */

  /* Use integer scale. */

  auto_assign_palette(&priv, gb_colour_hash(&gb));

  while (run_game) {
    // printk("run game\n");
    start_time = k_uptime_get();
    int delay;
    static double rtc_timer = 0;
    // static unsigned int selected_palette = 3;
    // static unsigned int dump_bmp = 0;

    {
      //   // A
      gb.direct.joypad_bits.a = gpio_pin_get_dt(&stuff_gpio);
      //   if (main_controller.ButtonA.pressing()) {
      //     gb.direct.joypad_bits.a = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.a = !Pressed;
      //   }
      //   // B
      //   if (main_controller.ButtonB.pressing()) {
      //     gb.direct.joypad_bits.b = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.b = !Pressed;
      //   }
      //   if (main_controller.ButtonDown.pressing()) {
      //     gb.direct.joypad_bits.down = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.down = !Pressed;
      //   }
      //   if (main_controller.ButtonUp.pressing()) {
      //     gb.direct.joypad_bits.up = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.up = !Pressed;
      //   }

      //   if (main_controller.ButtonLeft.pressing()) {
      //     gb.direct.joypad_bits.left = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.left = !Pressed;
      //   }
      //   if (main_controller.ButtonRight.pressing()) {
      //     gb.direct.joypad_bits.right = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.right = !Pressed;
      //   }

      //   if (main_controller.ButtonL1.pressing()) {
      //     gb.direct.joypad_bits.start = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.start = !Pressed;
      //   }
      //   if (main_controller.ButtonL2.pressing()) {
      //     gb.direct.joypad_bits.select = Pressed;
      //   } else {
      //     gb.direct.joypad_bits.select = !Pressed;
      //   }
      //
    }

    /* Calculate the time taken to draw frame, then later add a
     * delay to cap at 60 fps. */
    old_ticks = k_uptime_ticks();

    /* Execute CPU cycles until the screen has to be redrawn. */
    gb_run_frame(&gb);
    // printk("Ran Frame\n");

    /* Tick the internal RTC when 1 second has passed. */
    rtc_timer += target_speed_ms / (double)fast_mode;

    if (rtc_timer >= 1000.0) {
      printk("rtc timer >=1000");
      rtc_timer -= 1000.0;
      gb_tick_rtc(&gb);
    }

    /* Skip frames during fast mode. */
    if (fast_mode_timer > 1) {
      printk("FFast mode\n");
      fast_mode_timer--;
      /* We continue here since the rest of the logic in the
       * loop is for drawing the screen and delaying. */
      continue;
    }

    fast_mode_timer = fast_mode;

    // printk("Copying\n");
    copy_fb_to_screen_tex(priv.fb, buf, 320, 240);
    // printk("Copyed\n");
    /* Use a delay that will draw the screen at a rate of 59.7275 Hz. */
    new_ticks = k_uptime_ticks();

    /* Since we can only delay for a maximum resolution of 1ms, we
     * can accumulate the error and compensate for the delay
     * accuracy when the delay compensation surpasses 1ms. */
    speed_compensation += target_speed_ms - (new_ticks - old_ticks);

    /* We cast the delay compensation value to an integer, since it
     * is the type used by SDL_Delay. This is where delay accuracy
     * is lost. */
    delay = (int)(speed_compensation);

    /* We then subtract the actual delay value by the requested
     * delay value. */
    speed_compensation -= delay;

    /* Only run delay logic if required. */
    if (delay > 0) {
      uint_fast32_t delay_ticks = k_uptime_ticks();
      uint_fast32_t after_delay_ticks;

      /* Tick the internal RTC when 1 second has passed. */
      rtc_timer += delay;

      if (rtc_timer >= 1000) {
        rtc_timer -= 1000;
        gb_tick_rtc(&gb);

        /* If 60 seconds has passed, record save file.
         * We do this because the external audio library
         * used contains asserts that will abort the
         * program without save.
         * TODO: Remove use of assert in audio library
         * in release build. */
        /* TODO: Remove all workarounds due to faulty
         * external libraries. */
        --save_timer;

        if (!save_timer) {
          write_cart_ram_file(save_file_name, &priv.cart_ram,
                              gb_get_save_size(&gb));
          save_timer = 60;
        }
      }

      /* This will delay for at least the number of
       * milliseconds requested, so we have to compensate for
       * error here too. */
      // double pre_delay_ms = k_uptime_get() - start_time;
      //   printk("Delaying for delay %d", delay);
      k_msleep(10);
    }
    //   vexDelay(delay);

    //   Brain.Screen.printAt(300, 100, "emu time: %d", pre_delay_ms);
    display_write(display_dev, 0, 0, buf_desc, buf);
    // ++grey_count;
    // k_msleep(grey_scale_sleep);

    // after_delay_ticks = k_uptime_get();
    // speed_compensation +=
    // (double)delay - (int)(after_delay_ticks - delay_ticks);
  }

out:
  return ret;
}

int main(void) {
  size_t x;
  size_t y;
  size_t rect_w;
  size_t rect_h;
  size_t h_step;
  size_t scale;
  size_t grey_count;
  uint8_t bg_color;
  uint8_t *buf;
  int32_t grey_scale_sleep;
  const struct device *display_dev;
  struct display_capabilities capabilities;
  struct display_buffer_descriptor buf_desc;
  size_t buf_size = 0;
  //   fill_buffer fill_buffer_fnc = NULL;

  display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
  if (!device_is_ready(display_dev)) {
    LOG_ERR("Device %s not found. Aborting sample.", display_dev->name);
#ifdef CONFIG_ARCH_POSIX
    posix_exit_main(1);
#else
    return 0;
#endif
  }

  LOG_INF("Display sample for %s", display_dev->name);
  display_get_capabilities(display_dev, &capabilities);

  if ((capabilities.x_resolution < 3 * rect_w) ||
      (capabilities.y_resolution < 3 * rect_h) ||
      (capabilities.x_resolution < 8 * rect_h)) {
    rect_w = capabilities.x_resolution * 40 / 100;
    rect_h = capabilities.y_resolution * 40 / 100;
    h_step = capabilities.y_resolution * 20 / 100;
    scale = 1;
  } else {
    h_step = rect_h;
    scale = (capabilities.x_resolution / 8) / rect_h;
  }

  rect_w *= scale;
  rect_h *= scale;

  if (capabilities.screen_info & SCREEN_INFO_EPD) {
    grey_scale_sleep = 10000;
  } else {
    grey_scale_sleep = 100;
  }

  if (capabilities.screen_info & SCREEN_INFO_X_ALIGNMENT_WIDTH) {
    rect_w = capabilities.x_resolution;
  }

  rect_w = LCD_WIDTH;
  rect_h = LCD_HEIGHT;
  buf_size = rect_w * rect_h;
  h_step = rect_h;

  if (buf_size < (capabilities.x_resolution * h_step)) {
    buf_size = capabilities.x_resolution * h_step;
  }

  switch (capabilities.current_pixel_format) {
  case PIXEL_FORMAT_ARGB_8888:
    bg_color = 0x00u;
    // fill_buffer_fnc = fill_buffer_argb8888;
    buf_size *= 4;
    break;
  case PIXEL_FORMAT_RGB_888:
    bg_color = 0xFFu;
    // fill_buffer_fnc = fill_buffer_rgb888;
    buf_size *= 3;
    break;
  case PIXEL_FORMAT_RGB_565:
    bg_color = 0xff; // 0xFFu;
    // fill_buffer_fnc = fill_buffer_rgb565;
    buf_size *= 2;
    break;
  case PIXEL_FORMAT_BGR_565:
    bg_color = 0xFFu;
    // fill_buffer_fnc = fill_buffer_bgr565;
    buf_size *= 2;
    break;
  case PIXEL_FORMAT_MONO01:
    bg_color = 0xFFu;
    // fill_buffer_fnc = fill_buffer_mono01;
    buf_size = DIV_ROUND_UP(DIV_ROUND_UP(buf_size, NUM_BITS(uint8_t)),
                            sizeof(uint8_t));
    break;
  case PIXEL_FORMAT_MONO10:
    bg_color = 0x00u;
    // fill_buffer_fnc = fill_buffer_mono10;
    buf_size = DIV_ROUND_UP(DIV_ROUND_UP(buf_size, NUM_BITS(uint8_t)),
                            sizeof(uint8_t));
    break;
  default:
    LOG_ERR("Unsupported pixel format. Aborting sample.");
    return 0;
  }

  LOG_INF("buf size: %d", buf_size);
  buf = (uint8_t *)k_malloc(buf_size);

  if (buf == NULL) {
    LOG_ERR("Could not allocate memory. Aborting sample.");
    return 0;
  }

  (void)memset(buf, bg_color, buf_size);

  buf_desc.buf_size = buf_size;
  buf_desc.pitch = capabilities.x_resolution;
  buf_desc.width = capabilities.x_resolution;
  buf_desc.height = h_step;
  printk("p: %d, w: %d, h: %d\n", rect_w, rect_w, rect_h);

  /*
   * The following writes will only render parts of the image,
   * so turn this option on.
   * This allows double-buffered displays to hold the pixels
   * back until the image is complete.
   */
  buf_desc.frame_incomplete = true;

  buf_desc.pitch = rect_w;
  buf_desc.width = rect_w;
  buf_desc.height = rect_h;

  //   fill_buffer_fnc(TOP_LEFT, 0, buf, buf_size);

  /*
   * This is the last write of the frame, so turn this off.
   * Double-buffered displays will now present the new image
   * to the user.
   */
  buf_desc.frame_incomplete = false;

  //   fill_buffer_fnc(BOTTOM_RIGHT, 0, buf, buf_size);
  x = capabilities.x_resolution - rect_w;
  y = capabilities.y_resolution - rect_h;
  display_write(display_dev, x, y, &buf_desc, buf);

  display_blanking_off(display_dev);

  grey_count = 0;
  x = 0;
  y = capabilities.y_resolution - rect_h;

  LOG_INF("Display starts");
  emu_main(display_dev, &buf_desc, buf);
  return 0;
}
