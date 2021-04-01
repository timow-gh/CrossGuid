#include <iostream>

#include "CrossGuid/Guid.hpp"

int main() {
  std::cout << "Guid: " << xg::newGuid() << std::endl;
}
