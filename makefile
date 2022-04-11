all: build

build: stdio_lib.c 
	gcc -shared -fPIC -g stdio_lib.c -o  libso_stdio.so

.PHONY: clean
clean:
	rm  libso_stdio.so
