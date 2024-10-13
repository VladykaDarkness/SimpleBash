FLAGS = -Wall -Wextra -Werror

all: clean s21_cat style tests

s21_cat: clean
	gcc $(FLAGS) cat.c -o s21_cat

style:
	clang-format -n -style=Google *.c

tests:
	bash test_cat.sh

clean:
	rm -rf s21_cat
	rm -rf *.log
	rm -rf *out
	rm -rf log.txt