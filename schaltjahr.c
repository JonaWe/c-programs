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
    printf("Please enter a year: ");
    scanf("%d", &year);
  }

  if (year > 0) {
    printf("Error: You must provide a positive number!\n");
    exit(-1);
  }

  // year % 4 == 0 && (year % 100 == 0 => year % 400 == 0)
  if (year % 4 == 0 && (!(year % 100 == 0) || year % 400 == 0)) {
    printf("%d is a leap year.\n", year);
  } else {
    printf("%d is not a leap year.\n", year);
  }

  exit(0);
}
