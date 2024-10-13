#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
  int e, i, v, c, l, n, h, s, f, o;
  char pattern[3333];
  int pattern_size;

} options;

void parse_arguments(int argc, char *argv[], options *flag);
void add_pattern(options *flag, char *regular);
void read_f_file(options *flag, char *filepath);
int process_file(options flag, char *filepath, regex_t *compiled);
void print_line(char *str, int len);
void find_matches(regex_t *compiled, char *str, char *filepath, int line_count,
                  options flag);

int main(int argc, char *argv[]) {
  options flag = {0};
  parse_arguments(argc, argv, &flag);
  regex_t compiled;
  int error = regcomp(&compiled, flag.pattern, REG_EXTENDED | flag.i);
  // компилирует в объект compiled регулярное выражение, содержащееся в
  // flag.pattern
  if (error) printf("Error");
  // optind = название файлов после шаблонов
  for (int i = optind; i < argc; i++) {  // обрабатываем каждый файл
    process_file(flag, argv[i], &compiled);
  }
  regfree(&compiled);
  return 0;
}

void parse_arguments(int argc, char *argv[], options *flag) {
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) !=
         -1) {  // анализ аргументов, пока опции не закончились
    switch (opt) {
      case 'e':
        flag->e = 1;  // шаблон
        add_pattern(flag, optarg);
        break;
      case 'i':
        flag->i = REG_ICASE;  // игнорирует регистр
        break;
      case 'v':
        flag->v = 1;  // инверсия
        break;
      case 'c':
        flag->c = 1;  // количество строк
        break;
      case 'l':
        flag->l = 1;  // только имена совпадвющих файлов
        break;
      case 'n':
        flag->n = 1;  // номер строки в файле
        break;
      case 'h':
        flag->h = 1;  // не выводит названия файлов
        break;
      case 's':
        flag->s = 1;  // не выводит ошибки
        break;
      case 'f':
        flag->f = 1;  // берет шаблон из файла
        read_f_file(flag, optarg);
        break;
      case 'o':
        flag->o = 1;  // выводит только совпадающую часть строки, не всю
        break;
      case '?':
        printf("Usage: grep [OPTION]... PATTERNS [FILE]...\n");
        printf("Try 'grep --help' for more information.");
        exit(1);
      default:
        break;
    }
  }
  // добавление шаблона, если не был указан ни один шаблон через опции
  if (flag->pattern_size == 0) {
    add_pattern(flag, argv[optind]);
    optind++;
  }
  if (argc - optind == 1)
    flag->h = 1;  // если файл один, то вывод без названия файла
}

// добавляет регулярное выражение к строке pattern
void add_pattern(options *flag, char *regular) {
  if (flag->pattern_size != 0) {
    strcat(flag->pattern + flag->pattern_size, "|");
    flag->pattern_size++;
  }
  int len = snprintf(flag->pattern + flag->pattern_size, 3333, "(%s)", regular);
  // форматирует строку и записывает результат в строку pattern
  flag->pattern_size += len;
}

// обрабатывает файлы, переданные как аргумент опции
void read_f_file(options *flag, char *filepath) {
  FILE *fileName = fopen(filepath, "r");  // файл, из которого берём шаблон
  if (fileName == NULL && !flag->s) {
    printf("grep: %s: No such file or directory\n", filepath);
    exit(1);
  }
  char str[3333];
  while (fgets(str, 3333, fileName) != NULL) {  // читает каждую строку файла
    int len_str = strlen(str);
    if (str[len_str - 1] == '\n')
      str[len_str - 1] = '\0';  // заменяет конец строки на \0
    add_pattern(flag, str);
  }
  fclose(fileName);
}

int process_file(options flag, char *filepath, regex_t *compiled) {
  FILE *fileName = fopen(filepath, "r");
  if (fileName != NULL) {
    int line_count = 1;
    int match_counter = 0;
    int res;
    char str[3333];
    while (fgets(str, 3333, fileName) != NULL) {
      int len_str = strlen(str);
      res = regexec(compiled, str, 0, NULL, 0);  // 0 - если есть совпадение
      // выполняет поиск соответствия регулярному выражению в строке
      if ((res == 0 && !flag.v) || (res != 0 && flag.v)) {
        if (!flag.c && !flag.l) {
          if (!flag.h && !flag.o) printf("%s:", filepath);
          if (flag.n && !flag.o) printf("%d:", line_count);
          if (flag.o && !flag.v) {
            find_matches(compiled, str, filepath, line_count, flag);
          } else
            print_line(str, len_str);
        }
        match_counter++;
      }
      line_count++;
    }
    if (flag.c) {
      if (!flag.h) printf("%s:", filepath);
      printf("%d\n", match_counter);
    }
    if (flag.l && match_counter > 0) printf("%s\n", filepath);
    fclose(fileName);
  } else if (!flag.s)
    printf("grep: %s: No such file or directory\n", filepath);
  return 0;
}

void print_line(char *str, int len) {  // печать всей строки
  for (int i = 0; i < len; i++) {
    printf("%c", str[i]);
  }
  if (str[len - 1] != '\n') printf("%c", '\n');
}

void find_matches(regex_t *compiled, char *str, char *filepath, int line_count,
                  options flag) {
  // regex_t *compiled - указатель на скомпилированное регулярное выражение
  regmatch_t matches;
  int offset = 0;
  while (1) {
    int res = regexec(compiled, str + offset, 1, &matches, 0);
    // выполняет поиск соответствия регулярному выражению в строке
    if (res != 0) break;
    if (!flag.h) printf("%s:", filepath);
    if (flag.n) printf("%d:", line_count);

    for (int i = matches.rm_so; i < matches.rm_eo; i++) {
      printf("%c", str[offset + i]);
    }
    printf("%c", '\n');
    offset += matches.rm_eo;  // смещение, чтобы продолжить поиск после конца
                              // текущего совпадения.
  }
}

/*
1. -e Шаблон.
2. -i Игнорирует различия регистра.
3. -v Инвертирует смысл поиска соответствий.
4. -c Выводит только количество совпадающих строк.
5. -l Выводит только совпадающие файлы.
6. -n Предваряет каждую строку вывода номером строки из файла ввода.
7. -h Выводит совпадающие строки, не предваряя их именами файлов.
8. -s Подавляет сообщения об ошибках о несуществующих или нечитаемых файлах.
9. -f file Получает регулярные выражения из файла.
10. -o Печатает только совпадающие (непустые) части совпавшей строки.
*/