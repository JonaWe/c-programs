#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[]) {
  int year;

  if (argc > 1) {
    year = atoi(argv[1]);
  } else {
    scanf("%d", &year);
  }

  if (year % 4 == 0 && (!(year % 100 == 0) || year % 400 == 0)) {
    printf("%d ist ein Schaltjahr.\n", year);
  } else {
    printf("%d ist kein Schaltjahr.\n", year);
  }

  return 0;
}
