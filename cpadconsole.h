/* cpadconsole.h */

int silent;
int cpad;

void leave(int retval);

#define err(format, arg...) fprintf(stderr, "Error: " format "\n", ## arg)
#define info(format, arg...) do { if (!silent) printf(format, ## arg); } while (0);

#define PROMPT "> "
#define MAXPARAM 31
#define DEFAULT_DEVICE "/dev/usb/cpad0"
#define WELCOME "cpadconsole 0.1\nType \'help\' for help\n"

#define PROGRAM_DESCRIPTION \
"cpadconsole 0.1\n\
send commands to sed1335\n\
\n\
Syntax:\n\
	%s [-d device] [-f file] [-s]\n\
\n\
Description:\n\
	-d device : cpad character device (default: /dev/usb/cpad0)\n\
	-f file   : read commands from file instead from stdin\n\
	-s	  : silent\n"
