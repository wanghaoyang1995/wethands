#include "src/thread/CurrentThread.h"
#include <cassert>
#include <cstdio>

using namespace wethands;

int main() {
  assert(CurrentThread::IsMainThread() == true);
  printf("tid = %d\n", CurrentThread::Tid());
  CurrentThread::sleepUsec(2 * 1000 * 1000);
  printf("All done.\n");
  return 0;
}
