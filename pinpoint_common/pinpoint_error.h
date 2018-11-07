/*******************************************************************************
 * Copyright 2018 NAVER Corp.
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
#ifndef PINPOINT_COMMON_ERROR
#define PINPOINT_COMMON_ERROR
#define __STDC_LIMIT_MACROS 
#include <stdint.h>
namespace Pinpoint {

#ifndef SUCCESS
    static const int32_t SUCCESS = (0);
#endif
    static const int32_t FAILED = (-1);
    static const int32_t OUT_OF_MEMORY = (-2);
    static const int32_t WAIT_TIMEOUT = (-3);
    static const int32_t TRY_LOCK_FAIL = (-4);
    static const int32_t QUEUE_FULL = (-5);
    static const int32_t QUEUE_EMPTY = (-6);
    static const int32_t INVALID_PINPOINT_HTTP_HEADER = (-7);
    static const int32_t NOT_SUPPORTED = (-8);
    static const int32_t SAMPLING_IGNORE = (-9);
}

#endif
