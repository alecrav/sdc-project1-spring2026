#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

void example_9(int a[N], int b[N]) {
  for (unsigned i = 0; i < N; i++)
    a[i] = a[i] * b[i];
}

int main(void) {
  int a[N];
  int b[N];

  srand(13);
  for (unsigned j = 0; j < N; ++j) {
    a[j] = rand() % 100;
    b[j] = rand() % 100;
  }

  CALL_KERNEL(example_9, a, b);
  return 0;
}
