// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:45:10
// Description:

#include "src/net/InetAddress.h"
#include <arpa/inet.h>
#include <endian.h>
#include <netdb.h>
#include <cassert>
#include <cstring>
#include <string>
#include "src/logger/Logger.h"

using wethands::InetAddress;

static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in),
              "unexpected size of InetAddress.");

InetAddress::InetAddress() {
  ::memset(&addr_, 0, sizeof(addr_));
}

InetAddress::InetAddress(const char* ip, uint16_t port) {
  ::memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  // https://www.man7.org/linux/man-pages/man3/endian.3.html
  addr_.sin_port = ::htobe16(port);
  // https://www.man7.org/linux/man-pages/man3/inet_pton.3.html
  if (::inet_pton(AF_INET, ip, &addr_.sin_addr) < 0) {
    LOG_SYSERROR << "inet_pton() error.";
  }
}

InetAddress::InetAddress(uint16_t port) {
  ::memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = ::htobe16(port);
  addr_.sin_addr.s_addr = INADDR_ANY;
}

std::string InetAddress::ToString(bool printPort) const {
  char buf[32];
  ::inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
  if (printPort) {
    uint16_t portInHostEndian = ::be16toh(addr_.sin_port);
    size_t n = ::strnlen(buf, sizeof(buf));
    ::snprintf(buf + n, sizeof(buf) - n, ":%u", portInHostEndian);
  }
  return buf;
}

bool InetAddress::NameToAddress(const char* hostname,
                                const char* service,
                                InetAddress* resultAddr) {
  assert(resultAddr != nullptr);
  struct addrinfo hints, *res;
  ::memset(&hints, 0, sizeof(hints));
  if (hostname == nullptr) hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  // https://www.man7.org/linux/man-pages/man3/getaddrinfo.3.html
  int ret = ::getaddrinfo(hostname, service, &hints, &res);
  if (ret != 0) {
    LOG_ERROR << "getaddrinfo() error.";
    return false;
  }
  // res 返回的是一个链表, 只取第一个.
  assert(res != nullptr);
  assert(res->ai_addrlen == resultAddr->Len());
  assert(res->ai_family == AF_INET);
  assert(res->ai_socktype == SOCK_STREAM);
  assert(res->ai_protocol == IPPROTO_TCP);
  resultAddr->addr_.sin_family = AF_INET;
  resultAddr->addr_.sin_addr.s_addr =
    reinterpret_cast<struct sockaddr_in*>(res->ai_addr)->sin_addr.s_addr;
  resultAddr->addr_.sin_port =
    reinterpret_cast<struct sockaddr_in*>(res->ai_addr)->sin_port;
  ::freeaddrinfo(res);  // 千万别忘了释放.
  return true;
}
