#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000
#define N_HALF (N / 2)

void example_8(int a[N], int n) {
  for (unsigned i = 0; i < N_HALF; i++) {
    a[i * 2] = a[i * 2] * n;
    a[i * 2 + 1] = a[i * 2 + 1] * n;
  }
}

int main(void) {
  int a[N];

  srand(13);
  for (unsigned j = 0; j < N; ++j) {
    a[j] = rand() % 100;
  }

  CALL_KERNEL(example_8, a, rand() % 100);
  return 0;
}
