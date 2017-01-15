#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "js.h"
#include "tft.h"
#include "font.h"

#define WIDTH 320
#define HEIGHT 240
#define TILE 16
#define SCORE 3

#define COLS (WIDTH / TILE)
#define ROWS ((HEIGHT / TILE) - SCORE)

#define SKY 0x5ebf
#define BLACK       0x0000
#define BLUE	      0x001f
#define CYAN		    0x07ff
#define DARKCYAN    0x03EF
#define DARKGREEN	  0x03E0
#define DARKGREY    0x7BEF
#define GRAY1		    0x8410
#define GRAY2		    0x4208
#define GRAY3		    0x2104
#define GREEN       0x07e0
#define LIGHTGREEN  0xAFE5
#define LIGHTGREY   0xC618
#define MAGENTA     0xF81F
#define MAROON      0x7800
#define NAVY        0x000F
#define OLIVE       0x7BE0
#define ORANGE      0xFD20
#define PURPLE      0x780F
#define RED         0xf800
#define WHITE       0xffff
#define YELLOW      0xffe0


static unsigned short screen[WIDTH * HEIGHT];

static unsigned short colors[256] = {
	PURPLE,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	WHITE,
	LIGHTGREEN,
	SKY,
};

static int worldpos = 0;

static unsigned char world[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

};

static unsigned char tile0[256] = { };
static unsigned char tile1[256] = {
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x2, 0x1, 0x1,
	0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1,
};

static unsigned char tile2[256] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x5, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
};

static int scrollpos;

static void set_scrollpos(int pos)
{
	while (pos < 0)
		pos += WIDTH;

	scrollpos = pos % WIDTH;
}

static void draw_tile(int col, int row, int index)
{
	unsigned char buf[256], *data;
	int i, x, y;

	switch (index) {
	case 0: data = tile0; break;
	case 2: data = tile2; break;
	default:
		data = tile1; break;
	}

	memset(buf, 7, sizeof(buf));

	for (i = 0; i < sizeof(buf); i++)
		if (data[i])
			buf[i] = data[i];

	row *= TILE;
	col *= TILE;

	i = 0;
	for (y = 0; y < TILE; y++) {
		for (x = 0; x < TILE; x++) {
			screen[(y + row) * WIDTH + col + x] = colors[buf[i++]];
		}
	}
}

int main(int argc, char **argv)
{
	int i, x, y, p;
	int state;

#if 0
	for (y = 0; y < HEIGHT; y++)
		for (x = 0; x < WIDTH; x++)
			screen[y * WIDTH + x] = SKY;

	for (x = 0; x < COLS; x++)
		for (y = 0; y < ROWS; y++)
			draw_tile(x, SCORE+y, world[worldpos + y + x*ROWS]);
#else
	tft_init();
#endif
	
	i = 0;
	p = 0;
	x = 0;
	font_puts("GAME", 0, 0, 10, 4, 0xf800, TRANSP);
	font_puts("OVER....1.1.1.", 0, 40, 10, 4, 0xf81f, TRANSP);
	font_puts("peter", 0, 100, 10, 4, 0xf800, TRANSP);
	font_puts("hello world", 0, 200, 4, 5, 0x1f, SKY);

	tft_cfg_scroll(32, 200);
	do {
		state = js_state();

		if (i < 500)
			p++;
		else
			p -= 3;

//		if (state & JS_UP)
		tft_scroll(i % WIDTH);

		if (state & (JS_LEFT | JS_RIGHT)) {
			if (state & JS_RIGHT)
				worldpos += ROWS;
			else
				worldpos -= ROWS;

			if (worldpos < 0)
				worldpos = 0;
			for (x = 0; x < COLS; x++)
				for (y = 0; y < ROWS; y++)
					draw_tile(x, SCORE+y, world[worldpos + y + x*ROWS]);

//			SDL_UpdateTexture(texture, NULL, screen, WIDTH * sizeof(screen[0]));
		}
		tft_update();
		usleep(15000);

		i++;
	} while (!(state & JS_QUIT));

	return 0;
}
