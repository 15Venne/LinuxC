SRCS:=$(wildcard *.c)
a.out:$(SRCS)
	gcc $^ -I /usr/include/mysql/ -lmysqlclient -lcrypto
clean:
	rm a.out
