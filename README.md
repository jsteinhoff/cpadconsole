# cpadconsole

With this program you can send commands to the Seiko/Epson 1335 LCD Controller
and see what happens. This tool should help developing programs for the cpad
character device.

## Installation

To compile the program, you need libmagick and libreadline with header files.
Then do 'make' and 'make install', if you want to install the program to
/usr/bin.

## Description

Run cpadconsole and type in 'help'.
You can get the command line options with 'cpadconsole --help'.

## Video Memory

The following memory areas can be used:
    0 -  8191
16384 - 24575
32768 - 40959
49152 - 57343
The size of each area is 8kB, and they are in fact all the same memory. The
total memory is thus 8kB, and it can be accessed throught four different
address areas.

## Example

The following example will bring the sed1335 to combined text/graph mode,
display the image test.gif and the text "Hello World!". The image will flash at
16 Hz, so it seems to be gray.
Run cpadconsole and type in the following:

sysset char_height=8
display screen2=flash2 cursor=flash2
cursorform shape=underscore
overlay screen1=text
scroll screen1=4800
display on
cursor 0
writeimage test.gif
cursor 4800
writetext "Hello World!"
