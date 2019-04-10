GETC:=$(wildcard *.c)
OS:=$(patsubst %.c,%.o,$(GETC))
EXE:=main.exe
CFLAGS:=-Wall -g
$(EXE):$(OS)
	gcc $^ -o $@
clean:
	rm -rf $(OS) $(EXE)
