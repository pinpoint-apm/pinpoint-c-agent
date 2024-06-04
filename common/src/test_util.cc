////////////////////////////////////////////////////////////////////////////////
// Copyright 2020 NAVER Corp
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.
////////////////////////////////////////////////////////////////////////////////
/*
 * test_util.cc
 *
 *  Created on: Sep 15, 2020
 *      Author: eeliu
 */

#include <cstddef>
#include <gtest/gtest.h>
#include "NodePool/PoolManager.h"
using std::chrono::seconds;
namespace Json = AliasJson;

using PP::NodePool::PoolManager;
using PP::NodePool::TraceNode;

int sum(int n_args, ...) {
  va_list ap;
  va_start(ap, n_args);
  int var = n_args;

  int total = n_args;
  do {
    var = va_arg(ap, int);
    total += var;
  } while (var != 0);

  va_end(ap);

  return total;
}
//./bin/unittest --gtest_filter=util.variadic_func
TEST(util, variadic_func_int) {
  EXPECT_EQ(sum(1, 2, 3, 4, 5, 6, 0), 21);
  EXPECT_EQ(sum(1, 2, 3, 5, 0), 11);
}

int opt(const char* start, ...) {
  va_list ap;
  va_start(ap, start);
  const char* var = start;

  int total = 0;
  while (var != nullptr) {
    printf("%s\n", var);
    total++;
    var = va_arg(ap, const char*);
  }

  va_end(ap);

  return total;
}
//./bin/unittest --gtest_filter=util.variadic_func_str
TEST(util, variadic_func_str) {
  EXPECT_EQ(opt("a", "b", "c", "d", nullptr), 4);
  EXPECT_EQ(opt("a", "b", "c", nullptr), 3);
}

TEST(util, mergeTraceNodeTree) {
  PoolManager node_pool;
  TraceNode& n1 = node_pool.Take();
  TraceNode& n2 = node_pool.Take();
  TraceNode& n3 = node_pool.Take();
  TraceNode& n4 = node_pool.Take();

  n2.parent_id_ = n1.id_;
  n1.last_child_id_ = n2.id_;
  n3.parent_id_ = n2.id_;
  n4.parent_id_ = n2.id_;
  n3.sibling_id_ = n4.id_;
  n2.last_child_id_ = n3.id_;

  // Json::Value var = node_pool.ExpandTraceTreeNodes(n1);
  // std::cout << var.toStyledString();
}

std::string span;

void captureSpan(const char* s) { span = s; }

TEST(util, mergeTraceNodeTree_1) {
  pinpoint_set_agent("tcp:127.0.0.1:9999", 0, -1, 7000);
  register_span_handler(captureSpan);

  NodeID id1, id2, id3, id4;
  id1 = pinpoint_start_trace(E_ROOT_NODE);
  pinpoint_add_clue(id1, "name", "id1", E_LOC_CURRENT);
  id2 = pinpoint_start_trace(id1);
  pinpoint_add_clue(id2, "name", "id2", E_LOC_CURRENT);
  id3 = pinpoint_start_trace(id2);
  pinpoint_add_clue(id3, "name", "id3", E_LOC_CURRENT);
  id4 = pinpoint_start_trace(id2);
  pinpoint_add_clue(id4, "name", "id4", E_LOC_CURRENT);
  pinpoint_end_trace(id3);
  pinpoint_end_trace(id2);
  pinpoint_end_trace(id1);
  pinpoint_end_trace(id4);
  EXPECT_TRUE(span.find("id4") != span.npos);
  EXPECT_TRUE(span.find("id1") != span.npos);
  EXPECT_TRUE(span.find("id2") != span.npos);
  EXPECT_TRUE(span.find("id3") != span.npos);
  EXPECT_TRUE(span.find("calls") != span.npos);
}

TEST(util, logger) {
  pp_trace("abc:%d", 2);
  pp_trace("abc:%s", nullptr);
}