/*******************************************************************************
 * Copyright 2024 NAVER Corp
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#pragma once
#include "common.h"

#ifdef __linux__
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#ifdef __linux__

#elif defined(_WIN32)
#include <windows.h>

#define sleep(x) Sleep(x * 1000)
#define usleep(x) Sleep(x / 1000)
#else
#endif

void debug_nodeid(NodeID id);
uint64_t get_unix_time_ms();
void pinpoint_stop_agent();