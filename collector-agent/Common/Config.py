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

__config_path = os.path.abspath(os.path.dirname(__file__)) +"/../conf/collector.conf"

def get_conf(fullPath):
    config =  ConfigParser.ConfigParser()
    config.read(fullPath)
    return config

CAConfig = get_conf(__config_path);

if __name__ == '__main__':
    print(CAConfig.get('Common','LOG_DIR'))