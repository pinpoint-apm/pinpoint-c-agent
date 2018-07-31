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
#ifndef PINPOINT_PHP_PLUGIN_H
#define PINPOINT_PHP_PLUGIN_H

#include "config.h"
#include <vector>
#include "interceptor.h"
#include "pinpoint_api.h"

class PhpPluginManager
{
public:
    static PhpPluginManager* getInstance();
    int32_t registerPlugins();
    Pinpoint::Plugin::PluginPtrVector& getAllPlugins();
private:
    const static int32_t DEFAULT_PLUGIN_NUM = 20;
    PhpPluginManager();
    static PhpPluginManager* instance;
    Pinpoint::Plugin::PluginPtrVector plugins;
};


#endif
