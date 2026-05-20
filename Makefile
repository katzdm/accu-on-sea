CC = g++
CFLAGS = --std=c++26 -freflection

all:    main univ

main:	main.cpp building-info.h my-cls.h univ
	$(CC) $(CFLAGS) *.cpp -o BUILD/main

univ:	univ/dump.h univ/formatter.h
