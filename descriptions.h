/* command descriptions and declaration */

command_func write_function;
#define WRITE_DESCRIPTION \
"Syntax:\n\
	write <data>\n\
Description:\n\
	Writes data to cpad memory at actual cursor position. Data can be a\n\
	sequence of up to 30 bytes. Cursor is updated automatically.\n"

command_func writetext_function;
#define WRITETEXT_DESCRIPTION \
"Syntax:\n\
	writetext <text>\n\
Description:\n\
	Writes text at the actual cursor position.\n"

command_func writeimage_function;
#define WRITEIMAGE_DESCRIPTION \
"Syntax:\n\
	writeimage <filename>\n\
Description:\n\
	Writes an image at the actual cursor position using imagemagick.\n\
	The image is automatically resized and dithered.\n"

command_func read_function;
#define READ_DESCRIPTION \
"Syntax:\n\
	read\n\
Description:\n\
	Reads 30 bytes (one image line) from cpad memory. Cursor is increased\n\
	by 31 (?).\n"

command_func readtext_function;
#define READTEXT_DESCRIPTION \
"Syntax:\n\
	readtext\n\
Description:\n\
	Same as read, but outputs characters instead or hex numbers.\n"

command_func screenshot_function;
#define SCREENSHOT_DESCRIPTION \
"Syntax:\n\
	screenshot <filename>\n\
Description:\n\
	Saves image at the actual cursor position using imagemagick.\n"

command_func cursor_function;
#define CURSOR_DESCRIPTION \
"Syntax:\n\
	cursor [new position]\n\
Description:\n\
	Get or set cursor position.\n"

command_func display_function;
#define DISPLAY_DESCRIPTION \
"Syntax:\n\
	display [on/off] [cursor=state] [screen1=state] ... [screen3=state]\n\
Description:\n\
	The whole display can be turned on or off. The state of the cursor\n\
	or one of the screens can be set to on, off, flash1 or flash2. Flash1\n\
	means flashing at 2 Hz, flash2 means for the cursor flashing at 1 Hz\n\
	and for one of the screens flashing at 16 Hz. If a screen4 is present,\n\
	it has the same attributes as screen2.\n"

command_func scroll_function;
#define SCROLL_DESCRIPTION \
"Syntax:\n\
	scroll [height1=lines] [height2=lines] [screen1=addr]...[screen4=addr]\n\
Description:\n\
	Set start address and height of the screens. Height of screen3 and\n\
	screen4 can not be set manually.\n"

command_func scrollh_function;
#define SCROLLH_DESCRIPTION \
"Syntax:\n\
	scrollh [pixels]\n\
Description:\n\
	The scroll commands allows only horizontal scrolling by whole bytes\n\
	(8 pixels). This command allows smooth horizontal scrolling by single\n\
	pixels. Pixels can be 0 - 7.\n"

command_func cursorform_function;
#define CURSORFORM_DESCRIPTION \
"Syntax:\n\
	cursorform [width=pixel] [heigth=pixel] [shape=underscore/block]\n\
Description:\n\
	Set cursor size and shape.\n"

command_func cursordir_function;
#define CURSORDIR_DESCRIPTION \
"Syntax:\n\
	cursordir [right/left/up/down]\n\
Description:\n\
	Set direction of automatic cursor movement.\n"

command_func cgram_function;
#define CGRAM_DESCRIPTION \
"Syntax:\n\
	cgram [addr]\n\
Description:\n\
	Set character generator RAM start address.\n"

command_func overlay_function;
#define OVERLAY_DESCRIPTION \
"Syntax:\n\
	overlay [layers=2/3] [compose=OR/AND/XOR/POR]\n\
		[screen1=text/graph] [screen3=text/graph]\n\
Description:\n\
	Sets the number of layers and their composition method. POR means\n\
	\'Priority-OR\'. The mode of the screens can be set to text or graph.\n\
	Screen2 and screen4 are allways in graph mode. If in two-layer mode,\n\
	screen1 and screen3 are on one layer (screen3 is beneath screen1), and\n\
	screen2 and screen4 (if present) are on the second layer. Three-layer\n\
	mode is only possible, if screen4 is not present. Then screen1,\n\
	screen2 and screen3 are all on different layers. Screen1 and screen2\n\
	are then composed according to the parameter above, screen3 is\n\
	composed with OR. In mixed text graph mode, layers should be set to 2.\n"

command_func sysset_function;
#define SYSSET_DESCRIPTION \
"Syntax:\n\
	sysset [bytes_per_line=bytes] [htotal=bytes] [lines_per_frame=lines]\n\
	       [horiz_addr_range=bytes] [panel=single/dual]\n\
	       [ac_waveform=16line/2frame] [char_width=pixel]\n\
	       [char_height=pixel] [char_bitmap_height=8/16]\n\
	       [cgrom=intern/extern] [d6corr=on/off] [origin_comp=on/off]\n\
Description:\n\
	htotal is the total number of bytes per line, including horizontal\n\
	blanking. horiz_addr_range can be used to set up a virtual screen.\n\
	The cpad has one LCD panel, so panel should allways be single (Note:\n\
	screen4 is only available in dual-panel mode). The sed1335 data sheet\n\
	says, ac_waveform is usually set to 2frame. Hight and width of\n\
	characters can be set with char_width and char_height. The height of\n\
	the character bitmaps can be set to 8 bit or to 16 bit. cgrom\n\
	specifies wich character generator ROM to use. d6corr selects the\n\
	memory configuration for user-definable characters. If turned on, the\n\
	character generator RAM is contiguous, otherwise it is not. If\n\
	origin_comp is on, the text layer is shifted one line down relative\n\
	to the graph layer. This makes inverse characters at the top of the\n\
	page better readable.\n\
	This command disables the display.\n"

command_func sleep_function;
#define SLEEP_DESCRIPTION \
"Syntax:\n\
	sleep\n\
Description:\n\
	Enter standby mode.\n"

command_func wakeup_function;
#define WAKEUP_DESCRIPTION \
"Syntax:\n\
	wakeup\n\
Description:\n\
	Return from standby mode.\n"

command_func flash_function;
#define FLASH_DESCRIPTION \
"Syntax:\n\
	flash <time>\n\
Description:\n\
	Flash the backlight, time in 10 ms.\n"

command_func light_function;
#define LIGHT_DESCRIPTION \
"Syntax:\n\
	light [on/off]\n\
Description:\n\
	Reads or sets the backlight state.\n\
	ATTENTION: The backlight has a finite lifespan, so you should better\n\
	use flash to set the backlight state.\n"

command_func lcd_function;
#define LCD_DESCRIPTION \
"Syntax:\n\
	lcd [on/off]\n\
Description:\n\
	Reads or sets lcd state.\n"

command_func eeprom_function;
#define EEPROM_DESCRIPTION \
"Syntax:\n\
	eeprom\n\
Description:\n\
	Reads eeprom.\n"

command_func reset_function;
#define RESET_DESCRIPTION \
"Syntax:\n\
	reset\n\
Description:\n\
	Calls usb_reset_device.\n"

command_func exit_function;
#define EXIT_DESCRIPTION \
"Syntax:\n\
	exit\n\
Description:\n\
	Leaves the Program.\n"

command_func help_function;
#define HELP_DESCRIPTION \
"Syntax:\n\
	help [command]\n\
Description:\n\
	Gives help to a command. If command is ommitted, then a list of\n\
	possible commands is shown.\n"
