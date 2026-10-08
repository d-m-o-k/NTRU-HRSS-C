CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -g -Isrc -I.
LDLIBS  := -lm

TARGET  := test_poly
SRCS    := main.c src/poly.c

all: $(TARGET)

$(TARGET): $(SRCS) src/poly.h param.h
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
