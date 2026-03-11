
#define N 4
#include "dynamatic/Integration.h"
#include <stdlib.h>

// Read after write in the same BB. "volatile" prevents the load from being
// optimized away.
int raw_loop_free(volatile int a[N], int n) {
  a[0] = n + 1;
  n = a[0];
  return n;
}

int main(void) {
  int a[N];
  int n = N;

  srand(13);
  for (unsigned j = 0; j < N; ++j)
    a[j] = rand() % 10;

  CALL_KERNEL(raw_loop_free, a, n);
  return 0;
}
