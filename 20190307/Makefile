SRCS:=$(wildcard *.c)
ELF:=main
OBJS:=$(patsubst %.c,%.o,$(SRCS))
CC:=gcc
CFLAGS:= -g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@
clean:
	rm -rf $(ELF) $(OBJS)
