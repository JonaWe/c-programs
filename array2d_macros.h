#include <stdlib.h>

#define ALLOC_ARRAY2D(ARRAY,TYPE,DIM1,DIM2,INIT) \
{ \
  ARRAY = (TYPE**) malloc(DIM1 * sizeof(TYPE*)); \
  for (int i = 0; i < DIM1; i++) { \
    ARRAY[i] = (TYPE*) malloc(DIM2 * sizeof(TYPE)); \
    for (int j = 0; j < DIM2; j++) { \
      ARRAY[i][j] = INIT; \
    } \
  } \
}

#define DEL_ARRAY2D(ARRAY,DIM1) \
{ \
  for (int i = 0; i < DIM1; i++) \
    free(ARRAY[i]); \
  free(ARRAY); \
}

