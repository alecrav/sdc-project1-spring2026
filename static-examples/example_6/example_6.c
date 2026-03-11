#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

void example_6(int a[N], int b[N], int c[N], int d[N], int e[N], int f[N],
               int g[N], int h[N]) {
  for (unsigned i = 0; i < N; i++) {
    c[i] = b[i] * a[i];
    h[i] = g[i] * f[i] * e[i] * (d[i] + b[i]);
  }
}

int main(void) {
  int a[N];
  int b[N];
  int c[N];
  int d[N];
  int e[N];
  int f[N];
  int g[N];
  int h[N];

  srand(13);
  for (unsigned i = 0; i < N; ++i) {
    a[i] = rand() % 100;
    b[i] = rand() % 100;
    c[i] = rand() % 100;
    d[i] = rand() % 100;
    e[i] = rand() % 100;
    f[i] = rand() % 100;
    g[i] = rand() % 100;
    h[i] = rand() % 100;
  }

  CALL_KERNEL(example_6, a, b, c, d, e, f, g, h);
  return 0;
}
