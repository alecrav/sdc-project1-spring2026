#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

int example_4(int a[N], int b[N], int c[N]) {
  int x = 0;
  for (unsigned i = 0; i < N; i++) {
    x += b[i] + a[i] * b[i];
    c[i] = a[i] + b[i] * b[i];
  }
  return x;
}

int main(void) {
  int a[N];
  int b[N];
  int c[N];

  srand(13);
  for (unsigned i = 0; i < N; ++i) {
    a[i] = rand() % 100;
    b[i] = rand() % 100;
    c[i] = rand() % 100;
  }

  CALL_KERNEL(example_4, a, b, c);
  return 0;
}
