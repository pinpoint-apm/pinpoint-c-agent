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

import logging

import Config


def ca_logger(name,config,filename):
    fullPath = config.get('Common',
               'LOG_DIR')+filename
    logger = logging.getLogger(name)
    caFormat = logging.Formatter('[%(asctime)s] [%(levelname)s] [%(filename)s:%(lineno)s] %(message)s')
    fileHandler = logging.FileHandler(fullPath)
    fileHandler.setFormatter(caFormat)
    # level = config.get('Common','Log_Level','DEBUG')
    level = config.get('Common', 'Log_Level',fallback='DEBUG')
    logger.setLevel(level)
    logger.addHandler(fileHandler)

    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(caFormat)
    logger.addHandler(streamHandler)

    return logger


"""
TCLogger response for thrift collector
PALogger response for php agent
"""
TCLogger=ca_logger("TC",Config.CAConfig, "collector.agent.log")
PALogger=ca_logger("PA",Config.CAConfig, "php.agent.log")

if __name__ == '__main__':
    TCLogger.debug("test tc info")
    PALogger.info("test pa info")
