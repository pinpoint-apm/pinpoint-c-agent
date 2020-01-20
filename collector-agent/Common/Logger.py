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
import logging

def _create_logger(name):
    logger = logging.getLogger(name)
    return logger


TCLogger = _create_logger("TC")
PALogger = _create_logger("PA")

def set_logger_file(fullpath,format=logging.Formatter('[%(asctime)s] [%(process)d] [%(levelname)s] [%(filename)s:%(lineno)s] %(message)s')):
    fileHandler = logging.FileHandler(fullpath+'/collector.agent.log')
    fileHandler.setFormatter(format)
    TCLogger.addHandler(fileHandler)

    fileHandler = logging.FileHandler(fullpath+'/front.agent.log')
    fileHandler.setFormatter(format)
    PALogger.addHandler(fileHandler)

def set_logger_level(level):
    TCLogger.setLevel(level)
    PALogger.setLevel(level)

def logger_enable_console(format=logging.Formatter('[%(asctime)s] [%(process)d] [%(levelname)s] [%(filename)s:%(lineno)s] %(message)s')):
    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(format)
    TCLogger.addHandler(streamHandler)

    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(format)
    PALogger.addHandler(streamHandler)