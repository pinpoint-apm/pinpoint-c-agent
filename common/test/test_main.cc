#include <gtest/gtest.h>
#include "common.h"

int main(int argc, char** argv) {
  register_logging_cb(nullptr, 1);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}