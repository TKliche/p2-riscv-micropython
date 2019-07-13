// automatically generated by spin2cpp v3.9.28-beta-9a18aa43 on Sat Jul 13 06:25:05 2019
// command line: /home/ersmith/Parallax/spin2cpp/build/spin2cpp -I.. --p2 --ccode vgatext.spin 

#ifndef vgatext_Class_Defined__
#define vgatext_Class_Defined__

#include <stdint.h>
#include "vga_tile_driver.h"

#define VGATEXT_PIXEL_CLOCK_FREQ (40000000)
// (8*100 == 800)
#define VGATEXT_COLS (100)
// (40*15 == 600)
#define VGATEXT_ROWS (40)
#define VGATEXT_FONT_HEIGHT (15)
// bytes per character: use 4 for 8bpp colors, 8 for 24bpp colors
#define VGATEXT_CELL_SIZE (4)
#define VGATEXT_STATE_NORMAL (0)
#define VGATEXT_STATE_ESCAPE (1)
#define VGATEXT_STATE_START_CSI (2)
#define VGATEXT_STATE_IN_CSI (3)
#define VGATEXT_STATE_IN_PRIV (4)
#define VGATEXT_MAX_ESC_ARGS (8)
#define VGATEXT_CHAR_HIDDEN (1)
#define VGATEXT_CHAR_UNDERLINE (2)
#define VGATEXT_CHAR_STRIKETHRU (4)
#define VGATEXT_CHAR_BLINK_HIDDEN (16)
#define VGATEXT_CHAR_BLINK_UNDERLINE (32)
#define VGATEXT_CHAR_BLINK_STRIKETHRU (64)
#define VGATEXT_CHAR_BLINK_BG (128)
#define VGATEXT_CHAR_INVERSE (256)
#define VGATEXT_CHAR_BOLD (512)
#define VGATEXT_DEFAULT_FG_COLOR (-256)
#define VGATEXT_DEFAULT_BG_COLOR (0)
#define VGATEXT_PIXSHIFT (31)
#ifndef Tuple2__
  struct tuple2__ { int32_t v0;  int32_t v1; };
# define Tuple2__ struct tuple2__
#endif


typedef struct vgatext {
  volatile int32_t 	params[40];
  volatile int32_t 	screen_buffer[((VGATEXT_COLS * VGATEXT_ROWS) * (VGATEXT_CELL_SIZE / 4))];
  vga_tile_driver 	vga;
// cursor position
  int32_t 	curx, cury;
  int32_t 	savex, savey;
// current color
  int32_t 	bgcolor, fgcolor;
  int32_t 	state;
  int32_t 	screenptr;
  int32_t 	args[VGATEXT_MAX_ESC_ARGS];
  int32_t 	argidx;
  int32_t 	char_effects;
  int32_t 	scroll_first, scroll_last;
  char 	cursor_visible;
  char 	cursor_enabled;
  char 	cursor_effect;
  char 	saved_cursor_data;
  char 	buf[32];
} vgatext;

  int32_t vgatext_start(vgatext *self, int32_t pinbase);
  void vgatext_stop(vgatext *self);
  int32_t vgatext_tx(vgatext *self, int32_t c);
  void vgatext_glyphat(vgatext *self, int32_t x, int32_t y, int32_t ch, int32_t fgcol, int32_t bgcol, int32_t effect);
  void vgatext_showcursor(vgatext *self);
  void vgatext_hidecursor(vgatext *self);
  void vgatext_cls(vgatext *self);
  int32_t vgatext_getrgbcolor(int32_t r, int32_t g, int32_t b);
  void vgatext_str(vgatext *self, const char *s);
  void vgatext_nl(vgatext *self);
#endif
