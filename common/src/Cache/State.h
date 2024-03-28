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
#pragma once

#include <cstdint>
#include <ctime>
#include <memory>
#include <atomic>
namespace PP {
class State {
public:
  virtual int64_t GetUniqueId() = 0;
  virtual uint64_t GetStartTime() = 0;
  virtual void SetStartTime(uint64_t) = 0;
  virtual bool IsReady() = 0;
  virtual void ResetUniqueID() = 0;
  virtual bool CheckTraceLimit(int64_t timestamp) = 0;
};

class ProcessState : public State {
public:
  int64_t GetUniqueId() override { return uuid_++; }
  bool IsReady() override { return ready_; }
  bool CheckTraceLimit(int64_t timestamp) override;
  void ResetUniqueID() override { uuid_ = 0; }
  uint64_t GetStartTime() override { return starttime_; }
  void SetStartTime(uint64_t start_time) override;

public:
  explicit ProcessState(int64_t trace_limit);
  ProcessState(const ProcessState&) = delete;
  virtual ~ProcessState() = default;

private:
  int64_t tick_ = {0};
  std::time_t timestamp_ = {0};
  // default is the unix time in ms
  uint64_t starttime_;
  int64_t trace_limit_;
  std::atomic<std::int64_t> uuid_ = {0};
  bool ready_ = {false};
};
using StatePtr = std::unique_ptr<State>;

} // namespace PP