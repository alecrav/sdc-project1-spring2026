#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

float example_12(float a[N], float b[N]) {
  float x = 1;
  unsigned i = 0;
  while (x < 1000 && i < N) {
    x *= a[i] * b[i];
    i++;
  }
  return x;
}

int main(void) {
  float a[N];
  float b[N];

  srand(13);
  for (unsigned j = 0; j < N; ++j) {
    a[j] = rand() % 100;
    b[j] = rand() % 100;
  }

  CALL_KERNEL(example_12, a, b);
  return 0;
}
