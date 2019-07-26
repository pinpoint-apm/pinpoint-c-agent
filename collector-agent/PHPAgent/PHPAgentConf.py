#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from Common import PY2

if PY2:
    pass
else:
    pass

class PHPAgentConf(object):
    def __init__(self,config):
        self.Address = config.get('Agent','Address')
