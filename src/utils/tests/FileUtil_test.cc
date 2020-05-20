#include "src/utils/FileUtil.h"
#include <cassert>
#include <cstdio>
using namespace wethands;

int main() {
  {
    FileUtil::AppendFile file("./appendfile1.txt");
    const char str[] = "0123456789\nabcdefghijklmnopqrstuvwxyz\n";
    file.Append(str, sizeof(str));
    assert(file.WrittenBytes() == sizeof(str));
  }
  {
    FileUtil::AppendFile file("/home/ubuntu/work/test/appendfile2.txt");
    const char str[] = "0123456789 abcdefghijklmnopqrstuvwxyz\n";
    file.Append(str, sizeof(str));
    assert(file.WrittenBytes() == sizeof(str));
    printf("written bytes = %d\n", file.WrittenBytes());
  }
  printf("Done\n");
  return 0;
}
