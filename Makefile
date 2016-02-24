all: bin_dir gcadapter_evdev

bin_dir:
	-mkdir bin

gcadapter_evdev: gcadapter_evdev.o
	gcc -o bin/gcadapter_evdev src/gcadapter_evdev.o -pthread -Llibgcadapter -l:libgcadapter.a -lusb-1.0
	
gcadapter_evdev.o:
	gcc -c -o src/gcadapter_evdev.o src/gcadapter_evdev.c -pthread -Llibgcadapter -l:libgcadapter.a -lusb-1.0 -Iinclude -Ilibgcadapter/include -Wall -Werror
	
clean: 
	-rm src/gcadapter_evdev.o
	-rm -rf bin
