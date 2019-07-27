#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------
#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os
import sys

PY2 = sys.version_info[0] == 2
PY3 = sys.version_info[0] >= 3

if PY2:
    import ConfigParser
else:
    import configparser as ConfigParser


if not 'COLLECTOR_CONFIG' in os.environ:
    raise Exception("COLLECTOR_CONFIG not found in your environment")

__config_path = os.environ['COLLECTOR_CONFIG']

if not os.path.exists(__config_path):
    raise Exception("COLLECTOR_CONFIG:%s not exist"%(__config_path))

def get_conf(fullPath):
    config =  ConfigParser.ConfigParser()
    config.read(fullPath)
    return config

CAConfig = get_conf(__config_path);

if __name__ == '__main__':
    print(CAConfig.get('Common','LOG_DIR'))
