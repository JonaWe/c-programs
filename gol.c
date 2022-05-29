#include <stdio.h>
#include <unistd.h>
#include "array2d_macros.h"

void print_generation(int **current_generation, int dim_x, int dim_y, int generation) {
  printf("Generation %d:\n", generation);
  for (int i = 0; i < dim_x; i++) {
    for (int j = 0; j < dim_y; j++) {
      char c = current_generation[i][j] ? '*' : ' ';
      printf("%c", c);
    }
    printf("\n");
  }
}

int get_alive_neighbours(int **current_generation, int dim_x, int dim_y, int pos_x, int pos_y) {
  int alive_neighbours = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      // skip current cell
      if (i == 0 && j == 0) {
        continue;
      }

      int neighbour_x = pos_x + i;
      int neighbour_y = pos_y + j;

      // wrap around if out of bounds on x
      if (neighbour_x < 0) {
        neighbour_x = dim_x - 1;
      } else if (neighbour_x >= dim_x) {
        neighbour_x = 0;
      }

      // wrap around if out of bounds on y
      if (neighbour_y < 0) {
        neighbour_y = dim_y - 1;
      } else if (neighbour_y >= dim_y) {
        neighbour_y = 0;
      }

      if (current_generation[neighbour_x][neighbour_y]) {
        alive_neighbours++;
      }
    }
  }
  return alive_neighbours;

}

void copy_generation(int **current_generation, int **next_generation, int dim_x, int dim_y) {
  for (int i = 0; i < dim_x; i++) {
    for (int j = 0; j < dim_y; j++) {
      current_generation[i][j] = next_generation[i][j];
    }
  }
}

void update_generation(int **current_generation, int **next_generation, int dim_x, int dim_y) {
  for (int i = 0; i < dim_x; i++) {
    for (int j = 0; j < dim_y; j++) {
      int alive_neighbours = get_alive_neighbours(current_generation, dim_x, dim_y, i, j);
      if (current_generation[i][j]) {
        // alive cell
        if (alive_neighbours < 2 || alive_neighbours > 3) {
          next_generation[i][j] = 0;
        } else {
          next_generation[i][j] = 1;
        }
      } else {
        // dead cell
        if (alive_neighbours == 3) {
          next_generation[i][j] = 1;
        } else {
          next_generation[i][j] = 0;
        }
      }
    }
  }
  // copy the next generation to the current generation
  copy_generation(current_generation, next_generation, dim_x, dim_y);
}

void init_generation(int **current_generation, int dim_x, int dim_y) {
  for (int i = 0; i < dim_x; i++) {
    for (int j = 0; j < dim_y; j++) {
      current_generation[i][j] = rand() % 2;
    }
  }
}

int main() {
  // Get the user input
  int dim_x, dim_y, generations, delay;
  printf("Enter the dimensions of the grid (separated by space): ");
  scanf("%d %d", &dim_x, &dim_y);
  printf("Enter the number of generations: ");
  scanf("%d", &generations);
  printf("Enter the delay between generations: ");
  scanf("%d", &delay);

  // print the user input
  printf("Starting game with the following parameters:\n");
  printf("Dimension: %dx%d\n", dim_x, dim_y);
  printf("Generations: %d\n", generations);
  printf("Delay: %d\n", delay);

  // Create the gameboard for the current and next generation
  int **current_generation, **next_generation;
  ALLOC_ARRAY2D(current_generation, int, dim_x, dim_y, 0);
  ALLOC_ARRAY2D(next_generation, int, dim_x, dim_y, 0);

  // Initialize the gameboard randomly
  init_generation(current_generation, dim_x, dim_y);

  // game loop
  for (int generation = 0; generation <= generations; generation++) {
    print_generation(current_generation, dim_x, dim_y, generation);

    update_generation(current_generation, next_generation, dim_x, dim_y);

    // wait for delay in miliseconds
    usleep(delay * 1000);
  }

  DEL_ARRAY2D(current_generation, dim_x);
  DEL_ARRAY2D(next_generation, dim_x);

  return 0;
}
