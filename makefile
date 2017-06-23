build: pre compile asm linker

pre:
	cpp aplicacion.c > aplicacion.i
compile:
	gcc -S aplicacion.i
asm:
	as -o aplicacion.o aplicacion.s
linker:
	gcc -Wall -pthread -o aplicacion.exe aplicacion.o -lpigpio -lrt
run:
	sudo ./aplicacion.exe
clean:
	rm aplicacion.i aplicacion.s aplicacion.o aplicacion.exe