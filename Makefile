#LDFLAGS = `Wand-config --ldflags --libs`
#CFLAGS = `Wand-config --cflags --cppflags`
LDFLAGS = -lWand

all:	cpadconsole

cpadconsole: commands.o

.PHONY: clean

clean:
	$(RM) -f *.o cpadconsole

install:
	install cpadconsole /usr/bin
