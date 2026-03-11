
#define N 4
#include "dynamatic/Integration.h"
#include <stdlib.h>

// Read after write in the same BB.

void test_memory_raw(int a[N], int n) {
  int x = 0;
  for (int i = 0; i < n - 1; i++) {
    a[i] = x;
    x = a[i];
  }
}

int main(void) {
  int a[N];
  int n = N;

  srand(13);
  for (unsigned j = 0; j < N; ++j)
    a[j] = rand() % 10;

  CALL_KERNEL(test_memory_raw, a, n);
  return 0;
}
