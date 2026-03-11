#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

void example_3(int a[N], int b[N]) {
  for (unsigned i = 0; i < N; i++) {
    a[i] = b[i] * b[i];
    a[i] = i;
  }
}

int main(void) {
  int a[N];
  int b[N];

  srand(13);
  for (unsigned i = 0; i < N; ++i) {
    a[i] = rand() % 100;
    b[i] = rand() % 100;
  }

  CALL_KERNEL(example_3, a, b);
  return 0;
}
