CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -lm -I..
OBJECTS = mag_inertial.o Matrix.o

.PHONY: all
all: mag_inertial

mag_inertial: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(CFLAGS) -o mag_inertial
	
mag_inertial.o: mag_inertial.c
	$(CC) $(CFLAGS) -c mag_inertial.c
	
Matrix.o: ../Matrix.c
	$(CC) $(CFLAGS) -c ../Matrix.c
	
.PHONY: clean
clean:
	rm $(OBJECTS) mag_inertial
