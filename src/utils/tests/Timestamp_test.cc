#include "src/utils/Timestamp.h"
#include <cassert>
#include <cstdio>

using wethands::Timestamp;

void PassedByValue(Timestamp time) {
  printf("%s, %p\n", time.ToString(true).c_str(), &time);
}

void PassedByReference(const Timestamp& time) {
  printf("%s, %p\n", time.ToString(true).c_str(), &time);
}

int main() {
  Timestamp t1;
  assert(t1.MicrosecondsSinceEpoch() == 0);
  printf("%s\n", t1.ToString(false).c_str());

  t1 = Timestamp::Now();
  printf("%s, %p\n", t1.ToString(false).c_str(), &t1);

  PassedByValue(t1);
  PassedByReference(t1);

  return 0;
}