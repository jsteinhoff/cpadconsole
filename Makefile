LDFLAGS = `Wand-config --ldflags --libs` -lreadline -lhistory
CFLAGS = `Wand-config --cflags --cppflags`

all:	cpadconsole

cpadconsole: commands.o

.PHONY: clean

clean:
	$(RM) -f *.o cpadconsole

install:
	install cpadconsole /usr/bin
