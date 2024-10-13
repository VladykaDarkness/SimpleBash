#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct options {
  int n, b, s, t, v, e;
} options;

void parse_arguments(int argc, char* argv[], options* flag);
void read_file(char* argv[], options* flag);
void flag_v(options* flag, unsigned char* symb);

int main(int argc, char* argv[]) {
  options flag = {0};
  parse_arguments(argc, argv, &flag);
  while (optind < argc) {
    read_file(argv, &flag);
    optind++;
  }
}
// optind — это переменная, используемая функцией getopt в стандартной
// библиотеке C для указания индекса следующего элемента, который должен быть
// обработан из командной строки. Она играет ключевую роль при разборе
// аргументов командной строки и позволяет вам определять, какие аргументы были
// уже обработаны как опции, а какие остаются для дальнейшего анализа как
// позиционные аргументы (например, имена файлов).

void parse_arguments(int argc, char* argv[], options* flag) {
  struct option LONG_OPTIONS[] = {{"number", 0, 0, 'n'},
                                  {"number-nonblank", 0, 0, 'b'},
                                  {"squeeze-blank", 0, 0, 's'},
                                  {0, 0, 0, 0}};
  int opt;
  while ((opt = getopt_long(argc, argv, "nbstTveE", LONG_OPTIONS, 0)) !=
         -1) {  // анализ аргументов, пока опции не закончились
    switch (opt) {
      case 'n':
        flag->n = 1;  // нумерует все выходные строки
        break;
      case 'b':
        flag->b = 1;  // нумерует только непустые строки
        break;
      case 's':
        flag->s = 1;  // сжимает несколько смежных пустых строк
        break;
      case 't':
        flag->t = 1;
        flag->v = 1;  // отображает непечатаемые символы
        break;
      case 'T':
        flag->t = 1;  // отображает табы как ^I
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'e':
        flag->e = 1;  // отображает символы конца строки как $
        flag->v = 1;
        break;
      case 'E':
        flag->e = 1;
        break;
      case '?':
        printf("Try 'cat --help' for more information.");
        exit(1);
      default:
        break;
    }
  }
}

void read_file(char* argv[], options* flag) {
  FILE* fileName;
  fileName = fopen(argv[optind], "r");
  if (fileName == NULL) {
    printf("cat: %s: No such file or directory", argv[optind]);
    exit(1);
  }
  char symb;
  char prev = '\n';
  int count_n = 1;
  int count_str = 0;
  while ((symb = fgetc(fileName)) != EOF) {
    if (flag->b == 1) {
      flag->n = 0;
    }
    if ((prev == '\n') && (symb == '\n')) count_n++;
    if ((prev == '\n') && (symb != '\n')) count_n = 1;
    if (flag->s == 1 && count_n > 2 && symb == '\n') {
      prev = symb;
      continue;
    }
    if (flag->b == 1 && prev == '\n' && symb != '\n') {
      printf("%6d\t", ++count_str);
    }
    if (flag->n == 1 && prev == '\n') {
      printf("%6d\t", ++count_str);
    }
    if (flag->t == 1 && symb == '\t') {
      printf("^");
      symb = 'I';
    }
    flag_v(flag, (unsigned char*)&symb);
    if (flag->e == 1 && symb == '\n') {
      printf("$");
    }
    fputc(symb, stdout);
    prev = symb;
  }
  fclose(fileName);
}

void flag_v(options* flag, unsigned char* symb) {
  if (flag->v == 1) {
    if (*symb == '\n' || *symb == '\t') {
      *symb = *symb;
    } else if (*symb < 32) {
      printf("^");
      *symb += 64;
    } else if (*symb == 127) {
      printf("^");
      *symb = '?';
    } else if (*symb >= 128 && *symb < 161) {
      printf("M-^");
      *symb -= 64;
    }
  }
}