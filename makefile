CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -g -Isrc -I.
LDLIBS  := -lm

TARGET  := test
SRCS    := main.c src/poly.c src/coding.c src/sample.c src/fips202.c src/ntru.c src/keccakf1600.c src/rng.c src/aes.c
HDRS    := src/poly.h src/coding.h src/sample.h src/fips202.h src/ntru.h src/keccakf1600.h src/rng.h src/aes.h param.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
