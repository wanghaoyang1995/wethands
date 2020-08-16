// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 23:13:31
// Description:

#include "src/net/Buffer.h"
#include <sys/uio.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>
#include "src/logger/Logger.h"

using wethands::Buffer;

const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer(size_t initialSize)
    : buffer_(kInitPrependSize + initialSize),
      readIndex_(kInitPrependSize),
      writeIndex_(kInitPrependSize) {
  assert(ReadableBytes() == 0);
  assert(WritableBytes() == initialSize);
  assert(PrependableBytes() == kInitPrependSize);
}

void Buffer::Swap(Buffer& rhs) {
  buffer_.swap(rhs.buffer_);
  std::swap(readIndex_, rhs.readIndex_);
  std::swap(writeIndex_, rhs.writeIndex_);
}

void Buffer::Retrieve(size_t len) {
  assert(len <= ReadableBytes());
  if (len < ReadableBytes()) {
    readIndex_ += len;
  } else {
    RetrieveAll();
  }
}

void Buffer::RetrieveUntil(const char* end) {
  assert(end >= Peek());
  assert(end <= BeginWrite());
  Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
  readIndex_ = kInitPrependSize;
  writeIndex_ = kInitPrependSize;
}

std::string Buffer::RetrieveAsString(size_t len) {
  assert(len <= ReadableBytes());
  std::string result(Peek(), len);
  Retrieve(len);
  return result;
}

std::string Buffer::RetrieveAllAsString() {
  return RetrieveAsString(ReadableBytes());
}

void Buffer::HasWriten(size_t len) {
  assert(len <= WritableBytes());
  writeIndex_ += len;
}

void Buffer::Unwrite(size_t len) {
  assert(len <= ReadableBytes());
  writeIndex_ -= len;
}

void Buffer::Prepend(const void* data, size_t len) {
  assert(len <= PrependableBytes());
  readIndex_ -= len;
  ::memcpy(Begin() + readIndex_, data, len);
}

void Buffer::Append(const void* data, size_t len) {
  CheckAndMakePlace(len);
  ::memcpy(BeginWrite(), data, len);
  writeIndex_ += len;
}

ssize_t Buffer::ReadFd(int fd, int* errorCode) {
  // 预先开辟 64KiB 的栈缓冲区作备用.
  // 目的是为了减少系统调用的次数, 和及时扩展缓冲区大小.
  char extrabuf[65536];
  const size_t writable = WritableBytes();
  struct iovec vec[2];  // 两块缓冲区.
  vec[0].iov_base = BeginWrite();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof(extrabuf);
  const int iovCnt = writable >= sizeof(extrabuf) ? 1 : 2;
  // https://www.man7.org/linux/man-pages/man2/readv.2.html
  ssize_t n = ::readv(fd, vec, iovCnt);
  if (n < 0) {
    LOG_SYSERROR << "Buffer::ReadFd: readv() error.";
  } else if (static_cast<size_t>(n) <= writable) {  // 没有用到栈缓冲区.
    writeIndex_ += n;
  } else {  // 用到了栈缓冲区.
    writeIndex_ = buffer_.size();
    Append(extrabuf, n - writable);
  }
  return n;
}

const char* Buffer::FindCRLF() const {
  const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
  return crlf == BeginWrite() ? nullptr : crlf;
}

void Buffer::CheckAndMakePlace(size_t len) {
  if (WritableBytes() >= len) return;
  if (WritableBytes() + PrependableBytes() >= len + kInitPrependSize) {
    // 可写空间不足, 但是加上前置空闲(不包括kInitPrependSize)字节是够的.
    assert(kInitPrependSize < readIndex_);
    const size_t readable = ReadableBytes();
    // 目标指针在源指针之前, 所以内存重叠不会有问题.
    ::memcpy(Begin() + kInitPrependSize, Begin() + readIndex_, readable);
    readIndex_ = kInitPrependSize;
    writeIndex_ = readIndex_ + readable;
    assert(readable == ReadableBytes());
  } else {  // 空间确实不够了, 开辟新的内存.
    buffer_.resize(writeIndex_ + len);
  }
  assert(WritableBytes() >= len);
}
