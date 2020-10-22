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
 * NodeTreeWriter.h
 *
 *  Created on: Sep 21, 2020
 *      Author: eeliu
 */

#ifndef COMMON_SRC_CACHE_NODETREEWRITER_H_
#define COMMON_SRC_CACHE_NODETREEWRITER_H_
#include "json/json.h"
#include <iostream>
#include <sstream> 
namespace Cache {

/*
 * @author eeliu
 * Sep 21, 2020 2:05:59 PM
 */
class NodeTreeWriter
{
public:
    NodeTreeWriter()
    {
        builder["commentStyle"] = "None";
        builder["indentation"] = "";
    }
    
    std::string write(Json::Value& value)
    {
        Json::OStringStream out;
        std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
        writer->write(value, &out);
        return out.str();
    }

private:
    Json::StreamWriterBuilder builder;
    
};

} /* namespace Cache */

#endif /* COMMON_SRC_CACHE_NODETREEWRITER_H_ */
