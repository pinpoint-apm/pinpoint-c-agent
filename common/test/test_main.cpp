#include "common.h"
// #include <iostream>
#include <string>

int main(int argc, char const* argv[]) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);
  register_logging_cb(nullptr, 1);
  NodeID root = pinpoint_start_trace(E_ROOT_NODE);

  NodeID next = root;
  for (int i = 0; i < 1000; i++) {
    NodeID child = pinpoint_start_trace(next);
    pinpoint_add_clue(child, "id", std::to_string(child).c_str(), E_LOC_CURRENT);
    pinpoint_end_trace(child);

    if (i % 2 == 0) {
      next = child;
    }
  }
  pinpoint_end_trace(root);
  show_status();
  return 0;
}
