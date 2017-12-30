/* 1335 data structures */

#include "cpad.h"

/* List of command, as reported in the sed1335 data sheet
 * macros for the hex values are already defined in cpad.h
 *
 * <command>	<hex>	<command description>			<No. of Bytes>
 *
 * System control:
 * SYSSET	0x40	Initialize device and display 			8
 * SLEEP	0x53	Enter standby mode 				0
 *
 * Display control:
 * DISPOFF	0x58	Disable display, set flashing mode		1
 * DISPON	0x59	Enable diaplay, set flashing mode		1
 * SCROLL	0x44	Set display start address and display regions	10
 * CSRF		0x5d	Set cursor type					2
 * CGRAMADR	0x5c	Set start address of character generator RAM	2
 * CSRDIR_RIGHT	0x4c	Set direction of cursor movement		0
 * CSRDIR_LEFT	0x4d	"						0
 * CSRDIR_UP	0x4e	"						0
 * CSRDIR_DOWN	0x4f	"						0
 * HDOTSCR	0x5a	Set horizontal scroll position			1
 * OVLAY	0x5b	Set display overlay format			1
 *
 * Drawing control:
 * CSRW		0x46	Set cursor address				2
 * CSRR		0x47	Read cursor address				2
 *
 * Memory control:
 * MWRITE	0x42	Write to display memory			0 to 160*30
 * MREAD	0x43	Read from display memory			--
*/

/* The following data structures help to set the params for the sed1335
 * commands. Just fill the structure and then write the whole struct
 * to the cpad character device.
 */

#define packed __attribute__((packed))

struct twobytes {
	unsigned char low;
	unsigned char high;
} packed;



/* system control */

#define CGROM_INTERN	0
#define CGROM_EXTERN	1
#define D6_CORR_OFF	0
#define D6_CORR_ON	1
#define CHEIGHT8	0
#define CHEIGHT16	1
#define SINGLE_PANEL	0
#define DUAL_PANEL	1
#define OCOMP_ON	0
#define OCOMP_OFF	1
#define AC_16LINE	0
#define AC_2FRAME	1
struct {
	unsigned char cmd;
	unsigned char cgrom : 1;
	unsigned char d6corr : 1;
	unsigned char char_bitmap_height : 1;
	unsigned char panel : 1;
	unsigned char allwaysone : 1;
	unsigned char origin_comp : 1;
	unsigned char : 2;
	unsigned char char_width : 4;	/* width - 1 */
	unsigned char : 3;
	unsigned char ac_waveform : 1;
	unsigned char char_height : 4;	/* height - 1 */
	unsigned char : 4;
	unsigned char bytes_per_line;	/* minus one */
	unsigned char htotal;		/* minus one */
	unsigned char lines_per_frame;	/* minus one */
	struct twobytes horiz_addr_range;
} packed sysset = { SYSSET_1335, CGROM_INTERN, D6_CORR_OFF, CHEIGHT8,
		SINGLE_PANEL, 1, OCOMP_OFF, 7, AC_2FRAME, 0, 29, 41, 159,
		{30,0} };

const unsigned char sleep1335 = SLEEP_1335;


/* display control */

#define OFF_NOFLASH	0	/* values for cursor and screen* in display struct */
#define ON_NOFLASH	1
#define ON_FLASH1	2
#define ON_FLASH2	3
struct {
	union {
		unsigned char cmd;
		unsigned char on : 1;
	};
	unsigned char cursor : 2;
	unsigned char screen1 : 2;
	unsigned char screen2 : 2;
	unsigned char screen3 : 2;
} packed display = { {DISP_1335}, OFF_NOFLASH, ON_NOFLASH,
		OFF_NOFLASH, OFF_NOFLASH };

#define ADDR_BASE	(1<<13)
#define ADDR_BASE_HIGH	(ADDR_BASE>>8)
struct {
	unsigned char cmd;
	struct twobytes screen1_addr;
	unsigned char screen1_lines;	/* lines - 1 */
	struct twobytes screen2_addr;
	unsigned char screen2_lines;	/* lines - 1 */
	struct twobytes screen3_addr;
	struct twobytes screen4_addr;
} packed scroll = { SCROLL_1335, {0,ADDR_BASE_HIGH}, 159, {0,ADDR_BASE_HIGH},
		159, {0,ADDR_BASE_HIGH}, {0,ADDR_BASE_HIGH}};

struct {
	unsigned char cmd;
	unsigned char pixels : 3;
	unsigned char : 5;
} packed scrollh = { HDOTSCR_1335, 0 };

#define SHAPE_UNDER	0
#define SHAPE_BLOCK	1
struct {
	unsigned char cmd;
	unsigned char width : 4;	/* width - 1 */
	unsigned char : 4;
	unsigned char height : 4;	/* height - 1 */
	unsigned char : 3;
	unsigned char shape : 1;
} packed cursorform = { CSRF_1335, 7, 7, SHAPE_BLOCK };

#define DIR_RIGHT	0
#define DIR_LEFT	1
#define DIR_UP		2
#define DIR_DOWN	3
struct {
	union {
		unsigned char cmd;
		unsigned char dir : 2;
	};
} packed cursordir = { {CSRDIR_RIGHT_1335} };

struct {
	unsigned char cmd;
	struct twobytes addr;
} packed cgram = { CGRAMADR_1335, {0,0} };

#define COMP_OR		0
#define COMP_XOR	1
#define COMP_AND	2
#define COMP_POR	3
#define MODE_TEXT	0
#define MODE_GRAPH	1
#define LAYERS_TWO	0
#define LAYERS_THREE	1
struct {
	unsigned char cmd;
	unsigned char composition : 2;
	unsigned char mode1 : 1;
	unsigned char mode3 : 1;
	unsigned char layers : 1;
	unsigned char : 3;
} packed overlay = { OVLAY_1335, COMP_OR, MODE_GRAPH, MODE_GRAPH,
		LAYERS_TWO };


/* drawing control */

struct {
	unsigned char cmd;
	struct twobytes pos;
} packed set_cursor = { CSRW_1335, {0,0} };

const unsigned char get_cursor = CSRR_1335;


/* menory control */

struct {
	unsigned char cmd;
	unsigned char data[30];		/* can be up to 8192 bytes long */
} packed mem_write = { MWRITE_1335, {0} };

const unsigned char mem_read = MREAD_1335;
