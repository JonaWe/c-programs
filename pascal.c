#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error: usage: pascal <number>\n");
    exit(-1);
  }

  int number = atoi(argv[1]);

  if (number < 0) {
    printf("Error: you must provide a positive number.\n");
    exit(-1);
  }

  // adds padding on both "sides"
  unsigned long row[number + 2];
  unsigned long prev_rows[number + 2];

  // initialize array with ones
  for (int i = 0; i < number + 2; i++) {
    prev_rows[i] = 1;
    row[i] = 1;
  }

  for (int i = 1; i <= number; i++) {
    // calculates the new row
    for (int j = 1; j < i - 1; j++) {
      row[j] = prev_rows[j-1] + prev_rows[j];
    }
    // copy and print the rows
    for (int j = 0; j < i; j++) {
      printf("%lu ", row[j]);
      prev_rows[j] = row[j];
    }

    printf("\n");
  }


  exit(0);
}
