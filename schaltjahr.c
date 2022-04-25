#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[]) {
  int year;

  // arg provided -> reading args
  if (argc > 1) {
    year = atoi(argv[1]);
  }
  // no arg provided -> reading from stdin
  else {
    printf("Bitte Jahr eingeben: ");
    scanf("%d", &year);
  }

  // year % 4 == 0 && (year % 100 == 0 => year % 400 == 0)
  if (year % 4 == 0 && (!(year % 100 == 0) || year % 400 == 0)) {
    printf("%d ist ein Schaltjahr.\n", year);
  } else {
    printf("%d ist kein Schaltjahr.\n", year);
  }

  exit(0);
}
