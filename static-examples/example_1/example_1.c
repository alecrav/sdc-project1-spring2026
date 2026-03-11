#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

int example_1(int a[N], int b[N], int c[N], int d[N]) {
  int tmp = 0;
  for (unsigned i = 0; i < N; i++) {
    tmp += a[i] * b[i] + c[i] * d[i];
  }
  return tmp;
}

int main(void) {
  int a[N];
  int b[N];
  int c[N];
  int d[N];

  srand(13);
  for (int j = 0; j < N; ++j) {
    a[j] = rand() % 100;
    b[j] = rand() % 100;
    c[j] = rand() % 100;
    d[j] = rand() % 100;
  }

  CALL_KERNEL(example_1, a, b, c, d);
  return 0;
}
