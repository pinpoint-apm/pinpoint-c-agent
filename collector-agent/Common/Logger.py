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

TCLogger = logging.getLogger(__name__)
PALogger = logging.getLogger(__name__)

def create_logger(name, filename,fullPath,level):
    # fullPath = config.get('Common',
    #            'LOG_DIR')+filename
    fullPath =fullPath + filename
    logger = logging.getLogger(name)
    caFormat = logging.Formatter('[%(asctime)s] [%(levelname)s] [%(filename)s:%(lineno)s] %(message)s')
    fileHandler = logging.FileHandler(fullPath)
    fileHandler.setFormatter(caFormat)
    # level = config.get('Common','Log_Level','DEBUG')
    # level = config.get('Common', 'Log_Level',fallback='DEBUG')
    logger.setLevel(level)
    logger.addHandler(fileHandler)

    # disable console print
    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(caFormat)
    logger.addHandler(streamHandler)
    return logger

"""
TCLogger response for thrift collector
PALogger response for php agent
"""




