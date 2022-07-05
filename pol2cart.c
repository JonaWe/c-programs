#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct vec2d {
  double x;
  double y;
};

struct vec2d*
pol2cart(double r, double phi) {
  struct vec2d *tmp;
  tmp = malloc(sizeof(struct vec2d));
  tmp->x = r * cos(phi);
  tmp->y = r * sin(phi);
  return tmp;
}

int main() {
  struct vec2d* result = pol2cart(1.4, 1.2);

  printf("x=%f\n", result->x);
  printf("y=%f\n", result->y);

  free(result);
  exit(0);
}
