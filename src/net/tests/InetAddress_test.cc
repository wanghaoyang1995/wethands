#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include <endian.h>
#include <cassert>
using namespace wethands;

int main() {
  InetAddress addr1("1.2.3.4", 7766);
  assert(addr1.Family() == AF_INET);
  assert(addr1.PortInNetEndian() == ::htons(7766));
  printf("addr1: %x\n", ::be32toh(addr1.IpInNetEndian()));

  InetAddress addr2("123.213.132.231", 9876);
  printf("addr2: %s\n", addr2.ToString(true).c_str());

  InetAddress addr3;
  assert(InetAddress::NameToAddress("baidu.com", "7766", &addr3));
  printf("addr4: %s\n", addr3.ToString(true).c_str());

  InetAddress addr4("111.120.222.124", 2222);
  printf("addr4: %s\n", addr4.ToString(true).c_str());
  assert(InetAddress::NameToAddress(nullptr, "7766", &addr4));
  printf("addr4: %s\n", addr4.ToString(true).c_str());
  return 0;
}
