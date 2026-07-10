# Makefile — 畳み込み符号 誤り訂正能力評価

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
TARGET = error_eval

SRCS = main.c encoder.c trellis.c viterbi.c error_inject.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) --verbose

clean:
	rm -f $(OBJS) $(TARGET) ../output/results.csv

# 依存関係
main.o: main.c common.h encoder.h trellis.h viterbi.h error_inject.h
encoder.o: encoder.c encoder.h common.h
trellis.o: trellis.c trellis.h encoder.h common.h
viterbi.o: viterbi.c viterbi.h trellis.h common.h
error_inject.o: error_inject.c error_inject.h common.h
