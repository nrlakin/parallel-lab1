#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VECTOR_LENGTH 5

float norm(float* vec, int len) {
  int i;
  float result = 0;
  for (i=0; i<len; i++) {
    result += (*vec)*(*vec);
    vec++;
  }
  return result;
}

int main(int argc, char** argv) {
    int i;
    float x[VECTOR_LENGTH];
    float result;

    srand(time(NULL));
    for (i = 0; i < VECTOR_LENGTH; i++) {
      x[i] = (float)rand() / (float)RAND_MAX;
      printf("%f\n", x[i]);
    }
    result = norm(x, VECTOR_LENGTH);
    printf("L2 norm = %f\n", result);

    i = 500000000;
    printf("%d\n", i);
    i *= 8;
    printf("%d\n", i);
    i /= 8;
    printf("%d\n", i);

    return 0;
}
