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