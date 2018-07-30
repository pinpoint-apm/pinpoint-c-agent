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
#ifndef SIMULATE_DEF_H
#define SIMULATE_DEF_H

const char ANSWER_EXT_NAME[] = ".answer";
const char RESULT_EXT_NAME[] = ".result";

const char AGENT_INFO_PATH[] = "agent_info";
const char AGENT_INFO_NAME[] = "agent_info";

const char AGENT_STAT_BATCH_PATH[] = "agent_stat_batch";
const char AGENT_STAT_BATCH_NAME[] = "agent_stat_batch";

const char API_META_DATA_PATH[] = "api_meta_data";

const char STRING_META_DATA_PATH[] = "string_meta_data";

// trace raw name: prefix_parentSpanId_spanId
// trace normalized name: prefix_num
const char TRACE_PATH[] = "trace";
const char TRACE_NAME_PREFIX[] = "trace";
const char TRACE_ROOT_NAME[] = "root";
const char TRACE_NORMALIZED_NAME_PREFIX[] = "trace";
#endif
