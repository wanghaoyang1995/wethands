#include "src/utils/FileUtil.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace wethands;

int main() {
  {
    FileUtil::AppendFile file("./appendfile1.txt");
    const char str[] = "0123456789\nabcdefghijklmnopqrstuvwxyz\n";
    file.Append(str, ::strlen(str));
    file.Flush();
    assert(file.WrittenBytes() == ::strlen(str));
  }
  printf("Done\n");
  return 0;
}
