CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g

TARGET = odd_sum
SRCS = main.c big_number.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean test unit-test integration-test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c big_number.h
	$(CC) $(CFLAGS) -c $< -o $@

# Test Big Number library
test_bignum: ../test/test_bignum.c big_number.c big_number.h
	$(CC) $(CFLAGS) ../test/test_bignum.c big_number.c -o test_bignum

# Test odd_sum function
test_odd_sum: ../test/test_odd_sum.c big_number.c big_number.h
	$(CC) $(CFLAGS) ../test/test_odd_sum.c big_number.c -o test_odd_sum

# Run both C tests
unit-test: test_bignum test_odd_sum
	./test_bignum
	./test_odd_sum


# Run all tests
test: unit-test integration-test

clean:
	rm -f $(OBJS) $(TARGET) test_bignum test_odd_sum