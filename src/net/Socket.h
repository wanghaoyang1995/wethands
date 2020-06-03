// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:39:37
// Description:

#ifndef SRC_NET_SOCKET_H_
#define SRC_NET_SOCKET_H_

#include "src/utils/Uncopyable.h"

namespace wethands {

class Socket : public Uncopyable {
 public:
  explicit Socket(int sockfd);
  ~Socket();

  int Fd() const { return sockfd_; }


 private:
  const int sockfd_;
};

}  // namespace wethands

#endif  // SRC_NET_SOCKET_H_
