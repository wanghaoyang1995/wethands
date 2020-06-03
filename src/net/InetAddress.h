// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:44:59
// Description:

#ifndef SRC_NET_INETADDRESS_H_
#define SRC_NET_INETADDRESS_H_

#include <netinet/in.h>
#include <string>
#include "src/utils/Copyable.h"

namespace wethands {
// ip 地址. 只支持 ipv4.
// 可拷贝的.
class InetAddress : public Copyable {
 public:
  InetAddress(const char* ip, uint16_t port);
  InetAddress(const std::string& ip, uint16_t port);
  ~IndeAddress();



 private:
  // TODO(GGGGITFKBJG): 增加 ipv6 的支持.
  struct sockaddr_in addr_;
};

}  // namespace wethands

#endif  // SRC_NET_INETADDRESS_H_
