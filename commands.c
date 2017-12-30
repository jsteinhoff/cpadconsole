/* the commands */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <wand/magick_wand.h>

#include "cpadconsole.h"
#include "cpad.h"
#include "sed1335.h"


/* list of all commands */

typedef int (command_func)(int argc, char **argv);
#include "descriptions.h"

struct {
	char *name;
	command_func *function;
	char *description;
} command_list [] = {
	{ "write",	write_function,		WRITE_DESCRIPTION	},
	{ "writetext",	writetext_function,	WRITETEXT_DESCRIPTION	},
	{ "writeimage",	writeimage_function,	WRITEIMAGE_DESCRIPTION	},
	{ "read",	read_function,		READ_DESCRIPTION	},
	{ "readtext",	readtext_function,	READTEXT_DESCRIPTION	},
	{ "screenshot",	screenshot_function,	SCREENSHOT_DESCRIPTION	},
	{ "cursor",	cursor_function,	CURSOR_DESCRIPTION	},
	{ "flash",	flash_function,		FLASH_DESCRIPTION	},
	{ "light",	light_function,		LIGHT_DESCRIPTION	},
	{ "display",	display_function,	DISPLAY_DESCRIPTION	},
	{ "scroll",	scroll_function,	SCROLL_DESCRIPTION	},
	{ "scrollh",	scrollh_function,	SCROLLH_DESCRIPTION	},
	{ "cursorform",	cursorform_function,	CURSORFORM_DESCRIPTION	},
	{ "cursordir",	cursordir_function,	CURSORDIR_DESCRIPTION	},
	{ "cgram",	cgram_function,		CGRAM_DESCRIPTION	},
	{ "overlay",	overlay_function,	OVERLAY_DESCRIPTION	},
	{ "sysset",	sysset_function,	SYSSET_DESCRIPTION	},
	{ "sleep",	sleep_function,		SLEEP_DESCRIPTION	},
	{ "wakeup",	wakeup_function,	WAKEUP_DESCRIPTION	},
	{ "lcd",	lcd_function,		LCD_DESCRIPTION		},
	{ "eeprom",	eeprom_function,	EEPROM_DESCRIPTION	},
	{ "reset",	reset_function,		RESET_DESCRIPTION	},
	{ "exit",	exit_function,		EXIT_DESCRIPTION	},
	{ "help",	help_function,		HELP_DESCRIPTION	},
	{ 0, 0, 0 }
};


/* call a command */

void call_command(int argc, char **argv)
{
	int i;

	for (i=0; command_list[i].name; i++)
		if (!strcmp(argv[0], command_list[i].name))
			break;

	if (!command_list[i].name) {
		err("No command with this name.");
		return;
	}

	if (argc > MAXPARAM) {
		err("Syntax");
		info(command_list[i].description);
		return;
	}

	switch (command_list[i].function(argc, argv)) {
	case 1:
		perror("Error");
		return;
	case 2:
		err("Syntax");
		info(command_list[i].description);
		return;
	}
}


/* helper functions */

unsigned char buffer[33] = {};

int inline cpad_read()
{
	return read(cpad, buffer, 32);
}

int inline cpad_write(void *data, size_t size)
{
	int write_res, read_res;

	write_res = write(cpad, data, size);
	if (write_res <= 0)
		return write_res;
	read_res = cpad_read();
	if (read_res < 0)
		return read_res;

	return write_res;
}

#define CPAD_WRITE(data) cpad_write((void *) &data, sizeof(data))

int inline cpad_ioctl(int num, void *param)
{
	return ioctl(cpad, num, param);
}

int multi_compare(char *str, char *list[], int length)
{
	int i;

	for (i=0; i<length; i++) {
		if (!strcmp(str, list[i])) {
			return i;
		} else if (i == length-1) {
			break;
		}
	}
	return -1;
}

#define COMPARE(str, list) multi_compare(str, list, sizeof(list)/sizeof(char *))

char *on_off[] = { "off", "on" };

int inline str_on_off(char *str)
{
	return COMPARE(str, on_off);
}

#define set_bit(bit, data) do { data |= 1 << (bit); } while(0)
#define clear_bit(bit, data) do { data &= ~(1 << (bit)); } while(0)
#define test_bit(bit, data) (data & (1 << (bit)))


/* code of the commands */

int write_function(int argc, char **argv)
{
	int i, val;

	if ((argc > 31) || (argc == 1))
		return 2;

	for (i=1; i<argc; i++) {
		if (sscanf(argv[i], "%i", &val) != 1)
			return 2;
		mem_write.data[i-1] = val;
	}

	return (cpad_write(&mem_write, argc) <= 0) ? 1 : 0;
}

int writetext_function(int argc, char **argv)
{
	unsigned char *send_buffer, *pointer;
	int retval = 0;
	int i, res;

	if (argc != 2)
		return 2;

	i = strlen(argv[1]);
	send_buffer = malloc(i+2);
	if (!send_buffer)
		return 1;

	strcpy(send_buffer+1, argv[1]);
	pointer = send_buffer;
	while (i) {
		pointer[0] = MWRITE_1335;
		res = cpad_write(pointer, i+1);
		if (res <= 0) {
			retval = 1;
			goto error;
		}
		pointer += res-1;
		i -= res-1;
	}
error:
	free(send_buffer);
	return retval;
}

/* the following code is based on the C API example from imagemagick.org */
int writeimage_function(int argc, char **argv)
{
	MagickWand *wand;
	unsigned char *image, *send_buffer, *pointer;
	int retval = 1;
	int i, res;

	if (argc != 2)
		return 2;

	image = malloc(240*160);
	send_buffer = malloc(240*160/8+1);
	wand = NewMagickWand();
	if (!(wand && image && send_buffer)) {
		goto error;
	}
	if (!MagickReadImage(wand, argv[1])) {
		goto error;
	}

	MagickResizeImage(wand, 240, 160, LanczosFilter, 1.0);
	MagickSetImageColorspace(wand, GRAYColorspace);
	MagickPosterizeImage(wand, 2, 1);
	MagickGetImagePixels(wand, 0, 0, 240, 160, "I", CharPixel, image);

	for (i=0; i<240*160; i++)
		if (image[i])
			clear_bit(7-i%8, send_buffer[i/8+1]);
		else
			set_bit(7-i%8, send_buffer[i/8+1]);

	i = 240*160/8;
	pointer = send_buffer;
	while (i) {
		pointer[0] = MWRITE_1335;
		res = cpad_write(pointer, i+1);
		if (res <= 0)
			goto error;
		pointer += res-1;
		i -= res-1;
	}
	retval = 0;
error:
	free(image);
	free(send_buffer);
	DestroyMagickWand(wand);
	return retval;
}

int read_function(int argc, char **argv)
{
	int i;

	if (argc != 1)
		return 2;

	if (CPAD_WRITE(mem_read) <= 0)
		return 1;
	info("read:");
	for (i=2; i<32; i++)
		info(" %02x", buffer[i]);
	info("\n");
	return 0;
}

int readtext_function(int argc, char **argv)
{
	int i;

	if (argc != 1)
		return 2;

	if (CPAD_WRITE(mem_read) <=0)
		return 1;
	info("read: \"");
	for (i=2; i<32; i++) {
		buffer[i] = buffer[i] ? buffer[i] : 0x20;
		info("%c", (int) buffer[i]);
	}
	info("\"\n");
	return 0;
}

int screenshot_function(int argc, char **argv)
{
	MagickWand *wand;
	unsigned char *image;
	int retval = 1;
	int y, x, cursor;

	if (argc != 2)
		return 2;

	image = malloc(240*160);
	wand = NewMagickWand();
	if (!(wand && image))
		goto error;

	if (CPAD_WRITE(get_cursor) <= 0)
		goto error;
	cursor = (((int) buffer[2]) << 8) + buffer[3];
	for (y=0; y<160; y++) {
		set_cursor.pos.low = cursor & 0xff;
		set_cursor.pos.high = cursor >> 8;
		if (CPAD_WRITE(set_cursor) <= 0)
			goto error;
		if (CPAD_WRITE(mem_read) <= 0)
			goto error;
		for (x=0; x<240; x++)
			image[y*240+x] = test_bit(7-x%8, buffer[x/8+2]) ? 0 : 0xff;
		cursor += 240/8;
	}

	MagickSetSize(wand, 240, 160);
	MagickReadImage(wand, "xc:white");
	MagickSetImagePixels(wand, 0, 0, 240, 160, "I", CharPixel, image);
	if (MagickWriteImage(wand, argv[1]))
		retval = 0;
error:
	free(image);
	DestroyMagickWand(wand);
	return retval;
}

int cursor_function(int argc, char **argv)
{
	int pos;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		if (sscanf(argv[1], "%i", &pos) != 1)
			return 2;
		set_cursor.pos.low = pos & 0xff;
		set_cursor.pos.high = pos >> 8;
		if (CPAD_WRITE(set_cursor) <= 0)
			return 1;
	}
	if (CPAD_WRITE(get_cursor) <= 0)
		return 1;
	info("cursor: %i\n", (((int) buffer[2]) << 8) + buffer[3]);
	return 0;
}

int display_function(int argc, char **argv)
{
	int i = 0;
	int state, n;
	char str[8];
	char *display_str[] = { "off", "on", "flash1", "flash2" };
	char *cursor_desc[] = { "off", "on", "flash at 2 Hz", "flash at 1 Hz" };
	char *screen_desc[] = { "off", "on", "flash at 2 Hz", "flash at 16 Hz" };

	switch ((argc == 1) ? -1 : str_on_off(argv[1])) {
	case -1:
		i = 1;
		break;
	case 1:
		display.on = 1;
		i = 2;
		break;
	case 0:
		display.on = 0;
		i = 2;
		break;
	}

	for (; i<argc; i++) {
		if (sscanf(argv[i], "cursor=%8s", str) == 1) {
			state = COMPARE(str, display_str);
			if (state < 0)
				return 2;
			display.cursor = state;
		} else if (sscanf(argv[i], "screen%i=%8s", &n, str) == 2) {
			state = COMPARE(str, display_str);
			if (state < 0)
				return 2;
			switch (n) {
			case 1:
				display.screen1 = state;
				break;
			case 2:
				display.screen2 = state;
				break;
			case 3:
				display.screen3 = state;
				break;
			default:
				return 2;
			}
		} else {
			return 2;
		}
	}

	if (CPAD_WRITE(display) <= 0)
		return 1;
	info("display: %s\n", display.on ? "on" : "off");
	info("cursor : %s\n", cursor_desc[display.cursor]);
	info("screen1: %s\n", screen_desc[display.screen1]);
	info("screen2: %s\n", screen_desc[display.screen2]);
	info("screen3: %s\n", screen_desc[display.screen3]);
	return 0;
}

int scroll_function(int argc, char **argv)
{
	int i, n, param;
	unsigned char low, high;

	for (i=1; i<argc; i++) {
		if (sscanf(argv[i], "height%i=%i", &n, &param) == 2) {
			switch (n) {
			case 1:
				scroll.screen1_lines = param-1;
				break;
			case 2:
				scroll.screen2_lines = param-1;
				break;
			default:
				return 2;
			}
		} else if (sscanf(argv[i], "screen%i=%i", &n, &param) == 2) {
			param += ADDR_BASE;
			low = param & 0xff;
			high = param >> 8;
			switch (n) {
			case 1:
				scroll.screen1_addr.low = low;
				scroll.screen1_addr.high = high;
				break;
			case 2:
				scroll.screen2_addr.low = low;
				scroll.screen2_addr.high = high;
				break;
			case 3:
				scroll.screen3_addr.low = low;
				scroll.screen3_addr.high = high;
				break;
			case 4:
				scroll.screen4_addr.low = low;
				scroll.screen4_addr.high = high;
				break;
			default:
				return 2;
			}
		} else {
			return 2;
		}
	}

	if (CPAD_WRITE(scroll) <= 0)
		return 1;
	info("height of screen block 1: %i lines\n",
		(int) scroll.screen1_lines+1);
	info("height of screen block 2: %i lines\n",
		(int) scroll.screen2_lines+1);
	info("start address of screen block 1: %i\n",
		(((int) scroll.screen1_addr.high) << 8) +
		scroll.screen1_addr.low - ADDR_BASE);
	info("start address of screen block 2: %i\n",
		(((int) scroll.screen2_addr.high) << 8) +
		scroll.screen2_addr.low - ADDR_BASE);
	info("start address of screen block 3: %i\n",
		(((int) scroll.screen3_addr.high) << 8) +
		scroll.screen3_addr.low - ADDR_BASE);
	info("start address of screen block 4: %i\n",
		(((int) scroll.screen4_addr.high) << 8) +
		scroll.screen4_addr.low - ADDR_BASE);
	return 0;
}

int scrollh_function(int argc, char **argv)
{
	int pixels;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		if (sscanf(argv[1], "%i", &pixels) != 1)
			return 2;
		scrollh.pixels = pixels;
	}

	if (CPAD_WRITE(scrollh) <= 0)
		return 1;
	info("horizontal pixel shift: %i\n", scrollh.pixels);
	return 0;
}

int cursorform_function(int argc, char **argv)
{
	int i, n;
	char str[16];
	char *shape_str[] = { "underscore", "block" };

	for (i=1; i<argc; i++) {
		if (sscanf(argv[i], "width=%i", &n) == 1) {
			cursorform.width = n-1;
		} else if (sscanf(argv[i], "height=%i", &n) == 1) {
			cursorform.height = n-1;
		} else if (sscanf(argv[i], "shape=%16s", str) == 1) {
			n = COMPARE(str, shape_str);
			if (n < 0)
				return 2;
			cursorform.shape = n;
		} else {
			return 2;
		}
	}

	if (CPAD_WRITE(cursorform) <= 1)
		return 1;
	info("cursor width : %i\n", cursorform.width+1);
	info("cursor height: %i\n", cursorform.height+1);
	info("cursor shape : %s\n", shape_str[cursorform.shape]);
	return 0;
}

int cursordir_function(int argc, char **argv)
{
	int n;
	char *dir_str[] = { "right", "left", "up", "down" };

	if (argc > 2)
		return 2;

	if (argc == 2) {
		n = COMPARE(argv[1], dir_str);
		if (n < 0)
			return 2;
		cursordir.dir = n;
	}

	if (CPAD_WRITE(cursordir) <= 0)
		return 1;
	info("cursor movement direction: %s\n", dir_str[cursordir.dir]);
	return 0;
}

int cgram_function(int argc, char **argv)
{
	int addr;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		if (sscanf(argv[1], "%i", &addr) != 1)
			return 2;
		cgram.addr.low = addr & 0xff;
		cgram.addr.high = addr >> 8;
	}

	if (CPAD_WRITE(cgram) <= 0)
		return 1;
	info("character generator RAM start address: %i\n",
		(((int) cgram.addr.high) << 8) + cgram.addr.low);
	return 0;
}

int overlay_function(int argc, char **argv)
{
	int i, n, res;
	char str[8];
	char *comp_str[] = { "OR", "XOR", "AND", "POR" };
	char *mode_str[] = { "text", "graph" };
	char *layers_desc[] = { "two", "three" };

	for (i=1; i<argc; i++) {
		if (sscanf(argv[i], "compose=%8s", str) == 1) {
			res = COMPARE(str, comp_str);
			if (res < 0)
				return 2;
			overlay.composition = res;
		} else if (sscanf(argv[i], "screen%i=%8s", &n, str) == 2) {
			res = COMPARE(str, mode_str);
			if (res < 0)
				return 2;
			switch (n) {
			case 1:
				overlay.mode1 = res;
				break;
			case 3:
				overlay.mode3 = res;
				break;
			default:
				return 2;
			}
		} else if (sscanf(argv[i], "layers=%i", &n) == 1) {
			switch (n) {
			case 2:
				overlay.layers = LAYERS_TWO;
				break;
			case 3:
				overlay.layers = LAYERS_THREE;
				break;
			default:
				return 2;
			}
		} else {
			return 2;
		}
	}

	if (CPAD_WRITE(overlay) <= 0)
		return 1;
	info("screen composition method: %s\n", comp_str[overlay.composition]);
	info("screen1 mode: %s\n", mode_str[overlay.mode1]);
	info("screen3 mode: %s\n", mode_str[overlay.mode3]);
	info("layers: %s-layer composition\n", layers_desc[overlay.layers]);
	return 0;
}

int sysset_function(int argc, char **argv)
{
	int i, n;
	char str[8];
	char *cgrom_str[] = { "intern", "extern" };
	char *panel_str[] = { "single", "dual" };
	char *waveform_str[] = { "16line", "2frame" };

	for (i=1; i<argc; i++) {
		if (sscanf(argv[i], "bytes_per_line=%i", &n) == 1) {
			sysset.bytes_per_line = n-1;
		} else if (sscanf(argv[i], "htotal=%i", &n) == 1) {
			sysset.htotal = n-1;
		} else if (sscanf(argv[i], "lines_per_frame=%i", &n) == 1) {
			sysset.lines_per_frame = n-1;
		} else if (sscanf(argv[i], "horiz_addr_range=%i", &n) == 1) {
			sysset.horiz_addr_range.low = n & 0xff;
			sysset.horiz_addr_range.high = n >> 8;
		} else if (sscanf(argv[i], "char_width=%i", &n) == 1) {
			sysset.char_width = n-1;
		} else if (sscanf(argv[i], "char_height=%i", &n) == 1) {
			sysset.char_height = n-1;
		} else if (sscanf(argv[i], "cgrom=%8s", str) == 1) {
			n = COMPARE(str, cgrom_str);
			if (n < 0)
				return 2;
			sysset.cgrom = n;
		} else if (sscanf(argv[i], "panel=%8s", str) == 1) {
			n = COMPARE(str, panel_str);
			if (n < 0)
				return 2;
			sysset.panel = n;
		} else if (sscanf(argv[i], "ac_waveform=%8s", str) == 1) {
			n = COMPARE(str, waveform_str);
			if (n < 0)
				return 2;
			sysset.ac_waveform = n;
		} else if (sscanf(argv[i], "d6corr=%8s", str) == 1) {
			n = str_on_off(str);
			if (n < 0)
				return 2;
			sysset.d6corr = n ? 1 : 0;
		} else if (sscanf(argv[i], "origin_comp=%8s", str) == 1) {
			n = str_on_off(str);
			if (n < 0)
				return 2;
			sysset.d6corr = n ? 0 : 1;
		} else if (sscanf(argv[i], "char_bitmap_height=%i", &n) == 1) {
			switch (n) {
			case 8:
				sysset.char_bitmap_height = CHEIGHT8;
				break;
			case 16:
				sysset.char_bitmap_height = CHEIGHT16;
				break;
			default:
				return 2;
			}
		} else {
			return 2;
		}
	}

	if (CPAD_WRITE(sysset) <= 0)
		return 1;
	display.on = 0;
	info("bytes per line: %i\n", sysset.bytes_per_line+1);
	info("total bytes per line (includes blanking): %i\n",
		sysset.htotal+1);
	info("lines per frame: %i\n", sysset.lines_per_frame+1);
	info("horizontal address range: %i\n",
		(((int) sysset.horiz_addr_range.high) << 8) +
		sysset.horiz_addr_range.low);
	info("panel: %s\n", panel_str[sysset.panel]);
	info("AC frame drive waveform period: %s\n",
		waveform_str[sysset.ac_waveform]);
	info("character width: %i\n", sysset.char_width+1);
	info("character height: %i\n", sysset.char_height+1);
	info("character bitmap height: %i bit\n",
		sysset.char_bitmap_height ? 16 : 8);
	info("character generation ROM: %s\n", cgrom_str[sysset.cgrom]);
	info("D6 correction: %s\n", on_off[sysset.d6corr]);
	info("screen top-line correction (origin_comp): %s\n",
		sysset.origin_comp ? "off" : "on");
	return 0;
}

int sleep_function(int argc, char **argv)
{
	if (argc != 1)
		return 2;
	return (CPAD_WRITE(sleep1335) <= 0) ? 1 : 0;
}

int wakeup_function(int argc, char **argv)
{
	if (argc != 1)
		return 2;
	if (cpad_write(&sysset, 2) <= 0)
		return 1;
	return (cpad_write(&display, 1) <= 0) ? 1 : 0;
}

int lcd_function(int argc, char **argv)
{
	unsigned char state;
	int res;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		res = str_on_off(argv[1]);
		if (res < 0)
			return 2;
		state = res;
		if (cpad_ioctl(CPAD_WLCD, &state))
			return 1;
	}
	if (cpad_ioctl(CPAD_RLCD, 0))
		return 1;
	if (cpad_read() <= 0)
		return 1;
	info("lcd: %s\n", buffer[2] ? "on" : "off");
	return 0;
}

int light_function(int argc, char **argv)
{
	unsigned char state;
	int res;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		res = str_on_off(argv[1]);
		if (res < 0)
			return 2;
		state = res;
		if (cpad_ioctl(CPAD_WLIGHT, &state))
			return 1;
	}
	if (cpad_ioctl(CPAD_RLIGHT, 0))
		return 1;
	if (cpad_read() <= 0)
		return 1;
	info("backlight: %s\n", buffer[2] ? "on" : "off");
	return 0;
}

int flash_function(int argc, char **argv)
{
	int time;

	if (argc != 2)
		return 2;

	if (sscanf(argv[1], "%i", &time) != 1)
		return 2;

	return cpad_ioctl(CPAD_FLASH, &time) ? 1 : 0;
}

int eeprom_function(int argc, char **argv)
{
	int length, i;

	if (argc != 1)
		return 2;

	if (cpad_ioctl(CPAD_REEPROM, 0))
		return 1;

	length = cpad_read();
	if (length <= 0)
		return 1;

	info("eeprom:");
	for (i=2; i<length; i++)
		info(" %02x", buffer[i]);
	info("\n");

	return 0;
}

int reset_function(int argc, char **argv)
{
	if (argc != 1)
		return 2;

	return cpad_ioctl(CPAD_RESET, 0) ? 1 : 0;
}

int exit_function(int argc, char **argv)
{
	if (argc != 1)
		return 2;
	leave(0);
	return 0;
}

int help_function(int argc, char **argv)
{
	int i;

	if (argc > 2)
		return 2;

	if (argc == 2) {
		for (i=0; command_list[i].name; i++)
			if (!strcmp(argv[1], command_list[i].name))
				break;

		if (command_list[i].name) {
			info(command_list[i].description);
		} else {
			err("Unknown command, no help available.");
		}
	} else {
		info(	"Syntax:\n"
			"	help [command]\n"
		     	"Possible commands:");
		for (i=0; command_list[i].name; i++)
			info(" %s", command_list[i].name);
		info("\n");
		info("More detailed informations on the display controller commands are in the sed1335\n"
		"data sheet.\n");
	}
	return 0;
}
