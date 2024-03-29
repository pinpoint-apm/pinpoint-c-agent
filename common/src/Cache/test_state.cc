////////////////////////////////////////////////////////////////////////////////
// Copyright 2024 NAVER Corp
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

#include "gtest/gtest.h"
#include "State.h"

using PP::ProcessState;
TEST(ProcessState, api) {
  PP::StatePtr statePtr_ = std::unique_ptr<ProcessState>(new ProcessState(20));
  EXPECT_NE(statePtr_->GetStartTime(), 0);
  statePtr_->SetStartTime(1234567896);
  EXPECT_EQ(statePtr_->GetStartTime(), 1234567896);
  statePtr_->GetUniqueId();
  statePtr_->ResetUniqueID();
}

TEST(ProcessState, CheckTraceLimit) {
  PP::StatePtr statePtr_ = std::unique_ptr<ProcessState>(new ProcessState(4));
  EXPECT_FALSE(statePtr_->CheckTraceLimit(-1));
  EXPECT_FALSE(statePtr_->CheckTraceLimit(-1));
  EXPECT_FALSE(statePtr_->CheckTraceLimit(-1));
  EXPECT_FALSE(statePtr_->CheckTraceLimit(-1));
  EXPECT_TRUE(statePtr_->CheckTraceLimit(-1));
}