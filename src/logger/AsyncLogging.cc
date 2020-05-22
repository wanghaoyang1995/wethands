// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-21 02:28:56
// Description:

#include "src/logger/AsyncLogging.h"
#include <cstdio>
#include <utility>
#include "src/logger/LogFile.h"
#include "src/logger/Logger.h"

using wethands::AsyncLogging;

AsyncLogging::AsyncLogging(const std::string& basename,
                           size_t rollsize,
                           int flushInterval)
    : basename_(basename),
      rollsize_(rollsize),
      flushInterval_(flushInterval),
      running_(false),
      thread_(std::bind(&AsyncLogging::ThreadFunc, this), "AsyncLogging"),
      lock_(),
      notEmpty_(lock_),
      latch_(1),
      buffers_(),
      currentBuffer_(new Buffer),
      nextBuffer_(new Buffer) {
  buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging() {
  if (running_) Stop();
}

void AsyncLogging::Append(const char* line, size_t len) {
  // 由前台线程调用, 尽量不要做耗时操作.
  // 除非迫不得已, 内存分配操作尽量由后台线程来做.
  assert(running_);
  MutexLockGuard guard(lock_);
  if (currentBuffer_->AvailableBytes() > len) {  // 空间足够.
    currentBuffer_->Append(line, len);
  } else {  // 空间不足.
    // 将当前缓冲区放入队列.
    buffers_.push_back(std::move(currentBuffer_));
    if (nextBuffer_) {  // 备用缓冲区还未使用.
      currentBuffer_ = std::move(nextBuffer_);
    } else {  // 备用缓冲区已被使用过.
      currentBuffer_.reset(new Buffer);  // 迫不得已.
    }
    currentBuffer_->Append(line, len);
    notEmpty_.Notify();
  }
}

void AsyncLogging::Start() {
  assert(!running_);
  // Start() 一般在子线程启动前由主线程执行, 不需要加锁.
  running_ = true;
  asyncLog = this;
  Logger::SetOutputFunc(&AsyncLogging::AsyncOutput);
  thread_.Start();
  latch_.Wait();
}

void AsyncLogging::Stop() {
  assert(running_);
  running_ = false;
  asyncLog = nullptr;
  Logger::SetOutputFunc(&wethands::DefaultOutput);
  notEmpty_.Notify();
  thread_.Join();
}

AsyncLogging* AsyncLogging::asyncLog = nullptr;

void AsyncLogging::AsyncOutput(const char* line, size_t len) {
  asyncLog->Append(line, len);
}

void AsyncLogging::ThreadFunc() {
  latch_.CountDown();
  // 只有一个后台线程操作LogFile, 非线程安全版本足够了.
  LogFile file(basename_, rollsize_, false, flushInterval_);
  // 提前分配好两个缓冲区. 与 currentBuffer_ 及 nextBuffer_ 的交换使用.
  BufferPtr newBuffer1(new Buffer);
  BufferPtr newBuffer2(new Buffer);
  // 用于 buffers_ 交换使用.
  BufferVector buffersToWrite;
  buffersToWrite.reserve(16);

  while (running_) {
    {  // 临界区.
      MutexLockGuard guard(lock_);
      if (buffers_.empty()) {  // 使用 if 而不是 while.
        notEmpty_.WaitForSeconds(flushInterval_);
      }
      // 取走 currentBuffer_ 和 buffers.
      buffers_.push_back(std::move(currentBuffer_));
      currentBuffer_ = std::move(newBuffer1);
      buffers_.swap(buffersToWrite);
      // 如果备用缓冲区用完了要更新.
      if (!nextBuffer_) {
        nextBuffer_ = std::move(newBuffer2);
      }
    }

    // 日志增长过快, 丢弃并报错.
    if (buffersToWrite.size() > 16) {
      fprintf(stderr,
              "Dropped %lu Buffers at %s.\n",
              buffersToWrite.size() - 2,
              Timestamp::Now().ToFormattedString().c_str());
      buffersToWrite.resize(2);
    }

    // 将从队列取出的缓冲区写入文件.
    for (const auto& buffer : buffersToWrite) {
      file.Append(buffer->Data(), buffer->WrittenBytes());
    }

    if (buffersToWrite.size() > 2) {
      buffersToWrite.resize(2);
    }

    // 从队列中取出旧的复用
    newBuffer1 = std::move(buffersToWrite.back());
    buffersToWrite.pop_back();
    newBuffer1->Reset();

    // 如果缓冲区 newBuffer2 用尽了, 就从队列中取出旧的复用.
    if (!newBuffer2) {
      newBuffer2 = std::move(buffersToWrite.back());
      buffersToWrite.pop_back();
      newBuffer2->Reset();
    }

    buffersToWrite.clear();
    file.Flush();
  }
}
