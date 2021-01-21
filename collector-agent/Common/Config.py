#!/usr/bin/env python
# -*- coding: UTF-8 -*-



# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ------------------------------------------------------------------------------
import configparser as ConfigParser
import os


def get_conf(env_name):
    if not env_name in os.environ:
        return None

    __config_path = os.environ[env_name]
    if os.path.exists(__config_path):
        config = ConfigParser.ConfigParser()
        config.read(__config_path)
        return config
    else:
        return None

CAConfig = get_conf('COLLECTOR_CONFIG')

