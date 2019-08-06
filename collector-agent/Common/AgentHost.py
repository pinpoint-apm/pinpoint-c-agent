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

import socket
from Common.Config import CAConfig

class AgentHost(object):
    def __init__(self):
        self.hostname = socket.gethostname()
        self.ip   = socket.gethostbyname(self.hostname)
        self.port = CAConfig.get('Common','Web_Port')
    def __str__(self):
        return "host:%s ip:%s"%(self.hostname,self.ip)

if __name__ == '__main__':
    agent = AgentHost()
    print(agent)
