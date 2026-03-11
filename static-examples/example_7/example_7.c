#include "dynamatic/Integration.h"
#include "stdlib.h"

#define N 1000

void example_7(int a[N], int b[N], int c[N], int d[N], int e[N], int f[N],
               int g[N], int h[N]) {
  for (unsigned i = 0; i < N; i++) {
    a[i] *= h[i];
    b[i] *= h[i];
    c[i] *= h[i];
    d[i] *= h[i];
    e[i] *= h[i];
    f[i] *= h[i];
    g[i] *= h[i];
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
  for (int j = 0; j < N; ++j) {
    a[j] = rand() % 100;
    b[j] = rand() % 100;
    c[j] = rand() % 100;
    d[j] = rand() % 100;
    e[j] = rand() % 100;
    f[j] = rand() % 100;
    g[j] = rand() % 100;
    h[j] = rand() % 100;
  }

  CALL_KERNEL(example_7, a, b, c, d, e, f, g, h);
  return 0;
}
