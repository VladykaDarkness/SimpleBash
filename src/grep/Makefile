FLAGS = -Wall -Wextra -Werror

all: clean s21_grep style tests

s21_grep: clean
	gcc $(FLAGS) grep.c -o s21_grep

style:
	clang-format -n -style=Google *.c

tests:
	bash test_grep.sh

clean:
	rm -rf s21_grep
	rm -rf *.log
	rm -rf *out
	rm -rf log_grep.txt