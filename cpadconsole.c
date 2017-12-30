/* cpadconsole 0.1 
 *
 * send commands to the sed1335
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "cpadconsole.h"
#include "commands.h"

char *device_file = DEFAULT_DEVICE;
char *input_file = NULL;
int silent = 0;
int cpad = 0;
int prompt = 1;
FILE *input_stream = NULL;

char **cpad_argv = NULL;
char *input = NULL;


/* some helper functions */

inline void command_line(int argc, char **argv)
{
	int i;

	if (argc < 2)
		return;

	for (i=1; i<argc; i++) {
		if ((!strcmp(argv[i], "-d")) && (i<argc-1)) {
			device_file = argv[++i];
		} else if ((!strcmp(argv[i], "-f")) && (i<argc-1)) {
			input_file = argv[++i];
			prompt = 0;
		} else if (!strcmp(argv[i], "-s")) {
			silent = 1;
		} else {
			printf(PROGRAM_DESCRIPTION, argv[0]);
			leave(1);
		}
	}
}

inline void open_files()
{
	int test;

	info("Using device %s\n", device_file);
	if ((cpad = open(device_file, O_RDWR)) < 0) {
		perror("can not open device");
		leave(1);
	}
	if (input_file) {
		if ((test = open(input_file, O_RDWR)) < 0) {
			perror("can not open input file");
			leave(1);
		}
		close(test);
		if ((input_stream = fopen(input_file, "r")) < 0) {
			perror("can not open input file");
			leave(1);
		}
	} else {
		input_stream = stdin;
	}
}

inline int parse_input(char *input, int input_length, char **argv)
{
	char *input_pos = input;
	int argc = 0;
	int space = 1;

	for ( ; input_length; input_length--) {
		switch (*input_pos) {
		case ' ':
		case 9:
		case '\n':
			*input_pos = 0;
			space = 1;
			break;
		case '#':
			return argc;
		case '\"':
			if (space) {
				if (argc < MAXPARAM)
					argv[argc] = input_pos+1;
				argc++;
				space = 0;
			}
			do {
				input_pos++;
				input_length--;
			} while ((*input_pos != '\"') && (input_length > 1));
			*input_pos = 0;
			break;
		default:
			if (space) {
				if (argc < MAXPARAM)
					argv[argc] = input_pos;
				argc++;
				space = 0;
			}
			break;
		}
		input_pos++;
	}
	return argc;
}

void leave(int retval)
{
	if (cpad)
		close(cpad);
	if (input_stream)
		fclose(input_stream);
	if (input)
		free(input);
	if (cpad_argv)
		free(cpad_argv);
	exit(retval);
}


/* the main loop */

int main(int argc, char **argv)
{
	int input_length, cpad_argc;
	size_t max_input_length = 256;

	/* malloc */
	input = (char *) malloc(max_input_length);
	cpad_argv = (char **) malloc(sizeof(char *) * MAXPARAM);
	if ((!input) || (!cpad_argv)) {
		perror("not enough memory");
		leave(errno);
	}

	command_line(argc, argv);
	if (prompt)
		info(WELCOME);
	open_files();

	while (1) {
		/* read input */
		if (prompt)
			info(PROMPT);
		errno = 0;
		input_length = getline(&input, &max_input_length, input_stream);
		if (input_length < 0) {
			if (!errno)
				leave(0);
			perror("getline");
			leave(errno);
		}

		cpad_argc = parse_input(input, input_length, cpad_argv);

		if (cpad_argc == 0)
			continue;

		call_command(cpad_argc, cpad_argv);
	}
}
