/*
The MIT License (MIT)

Copyright (c) 2014 Graeme Hill (http://graemehill.ca)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include <array>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

#include "crossguid_export.h"

#define BEGIN_XG_NAMESPACE namespace xg {
#define END_XG_NAMESPACE   }

BEGIN_XG_NAMESPACE

// Class to represent a GUID/UUID. Each instance acts as a wrapper around a
// 16 byte value that can be passed around by value. It also supports
// conversion to string (via the stream operator <<) and conversion from a
// string via constructor.

class CROSSGUID_EXPORT Guid {
public:
  Guid();
  explicit Guid(const std::string& fromString);
  explicit Guid(const std::array<unsigned char, 16>& bytes);

  bool operator==(const Guid& other) const { return _bytes == other._bytes; }
  bool operator!=(const Guid& other) const { return !((*this) == other); }

  bool operator<(const Guid& rhs) const { return _bytes < rhs._bytes; }
  bool operator>(const Guid& rhs) const { return rhs < *this; }
  bool operator<=(const Guid& rhs) const { return !(rhs < *this); }
  bool operator>=(const Guid& rhs) const { return !(*this < rhs); }

  [[nodiscard]] std::string asString() const;
  [[nodiscard]] const std::array<unsigned char, 16>& bytes() const {
    return _bytes;
  }
  [[nodiscard]] bool isValid() const;

  void swap(Guid& other) { _bytes.swap(other._bytes); }

private:
  void zeroify() {
    std::fill(_bytes.begin(), _bytes.end(), static_cast<unsigned char>(0));
  }
  std::array<unsigned char, 16> _bytes;
  friend std::ostream& operator<<(std::ostream& s, const Guid& guid);
};

CROSSGUID_EXPORT Guid newGuid();

std::ostream& operator<<(std::ostream& s, const xg::Guid& guid) {
  std::ios_base::fmtflags f(
      s.flags()); // politely don't leave the ostream in hex mode
  s << std::hex << std::setfill('0') << std::setw(2) << (int)guid._bytes[0]
    << std::setw(2) << (int)guid._bytes[1] << std::setw(2)
    << (int)guid._bytes[2] << std::setw(2) << (int)guid._bytes[3] << "-"
    << std::setw(2) << (int)guid._bytes[4] << std::setw(2)
    << (int)guid._bytes[5] << "-" << std::setw(2) << (int)guid._bytes[6]
    << std::setw(2) << (int)guid._bytes[7] << "-" << std::setw(2)
    << (int)guid._bytes[8] << std::setw(2) << (int)guid._bytes[9] << "-"
    << std::setw(2) << (int)guid._bytes[10] << std::setw(2)
    << (int)guid._bytes[11] << std::setw(2) << (int)guid._bytes[12]
    << std::setw(2) << (int)guid._bytes[13] << std::setw(2)
    << (int)guid._bytes[14] << std::setw(2) << (int)guid._bytes[15];
  s.flags(f);
  return s;
}

namespace details {
  template<typename...>
  struct hash;

  template<typename T>
  struct hash<T> : public std::hash<T> {
    using std::hash<T>::hash;
  };

  template<typename T, typename... Rest>
  struct hash<T, Rest...> {
    inline std::size_t operator()(const T& v, const Rest&... rest) {
      std::size_t seed = hash<Rest...>{}(rest...);
      seed ^= hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      return seed;
    }
  };
} // namespace details

END_XG_NAMESPACE

namespace std {
  // Template specialization for std::swap<Guid>() --
  // See guid.cpp for the function definition
  template<>
  void swap(xg::Guid& lhs, xg::Guid& rhs) noexcept {
    lhs.swap(rhs);
  }

  // Specialization for std::hash<Guid> -- this implementation
  // uses std::hash<std::string> on the stringification of the guid
  // to calculate the hash
  template<>
  struct hash<xg::Guid> {
    std::size_t operator()(xg::Guid const& guid) const {
      const auto* p = reinterpret_cast<const uint64_t*>(guid.bytes().data());
      return xg::details::hash<uint64_t, uint64_t>{}(p[0], p[1]);
    }
  };
} // namespace std
