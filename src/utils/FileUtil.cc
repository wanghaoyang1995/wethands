// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-19 23:10:06
// Description:

#include "src/utils/FileUtil.h"
#include <errno.h>
#include <string.h>
#include <cassert>
#include <cstdio>

using wethands::FileUtil::AppendFile;

AppendFile::AppendFile(const std::string& filename)
    : AppendFile(filename.c_str()) {}

AppendFile::AppendFile(const char* filename)
    : fp_(::fopen(filename, "a")),
      writtenBytes_(0) {
  assert(fp_);
  ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

AppendFile::~AppendFile() {
  ::fclose(fp_);
}

void AppendFile::Append(const char* line, size_t len) {
  size_t remain = len;
  do {
    size_t n = ::fwrite_unlocked(line, 1, remain, fp_);
    if (n == 0) {
      int err = ::ferror(fp_);
      if (err) {
        char buf[512];
        fprintf(stderr, "AppendFile::Append(): %s\n",
                strerror_r(err, buf, sizeof(buf)));
      }
      writtenBytes_ += len - remain;
      return;
    }
    remain -= n;
  } while (remain > 0);

  writtenBytes_ += len;
}

void AppendFile::Flush() {
  ::fflush(fp_);
}
