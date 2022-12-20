FLAGS=-pthread -std=c11 -Wall -Werror -Wpedantic


main: main.c db.o student.o parsing.o 
	gcc -o tinydb main.c parsing.o student.o db.o  ${FLAGS}

student.o: student.c student.h
	gcc -c student.c
	
parsing.o: parsing.c parsing.h
	gcc -c parsing.c

db.o: db.c db.h
	gcc -c db.c
	

run:
	make main && ./main

tests: main tests/run_tests.py
	./tests/run_tests.py

clean:
	rm logs/*
	
clear:
	rm *.o
