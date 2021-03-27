#include "gtest/gtest.h"

#include "CrossGuid/Guid.hpp"

using namespace xg;

TEST(CrossGuid, DefaultConstructor) {
  Guid guid{};
  EXPECT_FALSE(guid.isValid());
}

TEST(CrossGuid, newGuid) {
  Guid guid = newGuid();
  EXPECT_TRUE(guid.isValid());
}

TEST(CrossGuid, CopyConstructor) {
  Guid guid = newGuid();
  Guid guidCopy = Guid(guid);
  EXPECT_TRUE(guid.isValid());
  EXPECT_TRUE(guidCopy.isValid());
  EXPECT_EQ(guid, guidCopy);
}

TEST(CrossGuid, CopyAssignment) {
  Guid guid = newGuid();
  Guid guidCopy = guid;
  EXPECT_TRUE(guid.isValid());
  EXPECT_TRUE(guidCopy.isValid());
  EXPECT_EQ(guid, guidCopy);
}

TEST(CrossGuid, GuidFromBytesContructor) {
  Guid guid = newGuid();
  Guid guidFromBytes = Guid(guid.bytes());
  EXPECT_TRUE(guid.isValid());
  EXPECT_TRUE(guidFromBytes.isValid());
  EXPECT_EQ(guid, guidFromBytes);
}

TEST(CrossGuid, GuidFromString) {
  Guid guid = newGuid();
  Guid guidFromString = Guid(guid.asString());
  EXPECT_TRUE(guid.isValid());
  EXPECT_TRUE(guidFromString.isValid());
  EXPECT_EQ(guid, guidFromString);
}

TEST(CrossGuid, MemberSwap) {
  Guid a = newGuid();
  Guid b = newGuid();
  EXPECT_TRUE(a.isValid());
  EXPECT_TRUE(b.isValid());
  EXPECT_NE(a, b);
  Guid aa = a;
  Guid bb = b;

  a.swap(b);
  EXPECT_EQ(a, bb);
  EXPECT_EQ(b, aa);
}

TEST(CrossGuid, StdSwap) {
  Guid a = newGuid();
  Guid b = newGuid();
  EXPECT_TRUE(a.isValid());
  EXPECT_TRUE(b.isValid());
  EXPECT_NE(a, b);
  Guid aa = a;
  Guid bb = b;

  std::swap(a, b);
  EXPECT_EQ(a, bb);
  EXPECT_EQ(b, aa);
}

TEST(CrossGuid, leftShiftOperator) {
  Guid a = newGuid();
  EXPECT_TRUE(a.isValid());
  std::stringstream sStream;
  sStream << a;
  Guid b = Guid(sStream.str());
  EXPECT_TRUE(b.isValid());
  EXPECT_EQ(a, b);
}
