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
  printf("t1 = %s\n", t1.ToString(false).c_str());

  t1 = Timestamp::Now();
  printf("t1 = %s, %p\n", t1.ToString(false).c_str(), &t1);

  PassedByValue(t1);
  PassedByReference(t1);

  Timestamp t2 = Timestamp::Now();
  Timestamp t3 = AddSeconds(t2, 3600);
  printf("t2 = %s\n", t2.ToString(false).c_str());
  printf("t3 = %s\n", t3.ToString(false).c_str());
  printf("t3 - t2 = %f\n", SecondsDifference(t3, t2));
  return 0;
}
