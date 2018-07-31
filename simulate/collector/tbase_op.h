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
#ifndef SIMULATE_TBASE_OP_H
#define SIMULATE_TBASE_OP_H

#include "stdint.h"
#include <boost/filesystem.hpp>
#include <thrift/TBase.h>
#include "thrift/gen-cpp/Trace_types.h"
#include "thrift/gen-cpp/Pinpoint_types.h"
#include "utility.h"

class TBaseOP
{
public:
    static TBasePtr readTbaseFromJsonFile(int16_t type, const boost::filesystem::path& path);
    static int32_t checkTbase(int16_t type, const TBasePtr& answer, const TBasePtr& result);
    static int32_t writeTbaseToJsonFile(const boost::filesystem::path& filePath, const apache::thrift::TBase& tbase);
    static std::string getTApiMetaDataFileName(const TBasePtr &tbasePtr);
    static std::string getTSpanFileRawName(const TBasePtr &tbasePtr);
    static std::string getTStringMetaDataFileName(const TBasePtr &tbasePtr);
    static int32_t normalizeTSpanRawFiles(const boost::filesystem::path& testCasePath);
    static int32_t getTSpanTestcasePath(const TSpanPtr& tSpanPtr, std::string& testCasePath);
};

#endif
