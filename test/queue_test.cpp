#include "winws/queue.hpp"

#include <assert.h>

int main () {
  Queue<int> q1;
  // q1.shift(); // throws
  assert(q1.isEmpty());
  q1.push(0);
  assert(!q1.isEmpty());
  assert(q1.shift() == 0);
  assert(q1.isEmpty());

  Queue<int> q2;
  q2.push(1);
  q2.push(2);
  q2.push(3);
  q2.push(4);
  q2.push(5);
  q2.push(6);
  assert(q2.shift() == 1);
  assert(q2.shift() == 2);
  assert(q2.shift() == 3);
  assert(q2.shift() == 4);
  assert(q2.shift() == 5);
  assert(q2.shift() == 6);
  printf("ok\n");
  return 0;
}
