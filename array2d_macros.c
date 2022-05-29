#include <stdlib.h>
#include <stdio.h>
#include "array2d_macros.h"

#define BASE 10

// pass appropriate format specifier e.g. %g to print a double
#define PRINT_ARRAY2D(ARRAY, DIM1, DIM2, FORMAT) \
  {                                              \
    int i, j;                                    \
    if (ARRAY != NULL)                           \
    {                                            \
      for (i = 0; i < DIM1; i++)                 \
      {                                          \
        for (j = 0; j < DIM2; j++)               \
        {                                        \
          printf(#FORMAT " ", ARRAY[i][j]);      \
        }                                        \
        putchar('\n');                           \
      }                                          \
    }                                            \
  }


// test functionality with your own header file by replacing the placeholder in line 3

int main(int argc, char const *argv[])
{
  unsigned int **arr1;
  long double **arr2;
  unsigned long long **arr3;
  char **arr4;
  unsigned short **arr5;
  float **arr6;

  int dim1, dim2;
  char *end_ptr1, *end_ptr2;

  // parse dimensions from commandline arguments
  if (argc == 3)
  {
    dim1 = strtol(argv[1], &end_ptr1, BASE);
    dim2 = strtol(argv[2], &end_ptr2, BASE);
    if (dim1 > 0 && dim2 > 0)
    {
      if (!*end_ptr1 && !*end_ptr2)
      {
        // unsigned int
        ALLOC_ARRAY2D(arr1, unsigned int, dim1, dim2, 42);
        arr1[dim1 - 1][dim2 - 1] = 'a';
        PRINT_ARRAY2D(arr1, dim1, dim2, %u);
        DEL_ARRAY2D(arr1, dim1);

        // long double
        ALLOC_ARRAY2D(arr2, long double, dim1, dim2, ((float)22) / 7);
        arr2[0][0] = 1.8125;
        PRINT_ARRAY2D(arr2, dim1, dim2, %Lf);
        DEL_ARRAY2D(arr2, dim1);

        // unsigned long long
        ALLOC_ARRAY2D(arr3, unsigned long long, dim1, dim2, -1);
        PRINT_ARRAY2D(arr3, dim1, dim2, %llu);
        DEL_ARRAY2D(arr3, dim1);
      }
      else
      {
        printf("Error: %s %s\n", end_ptr1, end_ptr2);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      printf("Error: invalid dimensions\n");
      exit(EXIT_FAILURE);
    }
  }

  // char
  ALLOC_ARRAY2D(arr4, char, 4, 3, 'u');
  arr4[1][2] = 's';
  arr4[3][0] = 's';
  PRINT_ARRAY2D(arr4, 4, 3, %c);
  DEL_ARRAY2D(arr4, 4);

  // unsigned short
  ALLOC_ARRAY2D(arr5, unsigned short, 6, 6, -1);
  arr5[4][2] = 'a' - '7';
  PRINT_ARRAY2D(arr5, 6, 6, %u);
  DEL_ARRAY2D(arr5, 6);

  // float
  ALLOC_ARRAY2D(arr6, float, 3, 2, ((float)22 / 7) - 1.125);
  arr6[0][1] = ((float)1) / (1 + 2);
  arr6[2][0] = -2;
  PRINT_ARRAY2D(arr6, 3, 2, %f);
  DEL_ARRAY2D(arr6, 3);

  return EXIT_SUCCESS;
}
