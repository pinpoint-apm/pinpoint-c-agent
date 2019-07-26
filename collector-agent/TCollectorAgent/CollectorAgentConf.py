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
import time

AGENT_VERSION = '1.8.0-RC1'


class CollectorAgentConf(object):
    def __init__(self,config):
        '''

        :param ConfigParser config:
        '''
        self.CollectorSpanIp = config.get('Collector',
                                         'CollectorSpanIp')
        self.CollectorSpanPort = config.getint('Collector',
                                          'CollectorSpanPort')
        self.CollectorStatIp = config.get('Collector',
                                          'CollectorStatIp')
        self.CollectorStatPort = config.getint('Collector',
                                          'CollectorStatPort')
        self.CollectorTcpIp = config.get('Collector',
                                          'CollectorTcpIp')
        self.CollectorTcpPort = config.getint('Collector',
                                         'CollectorTcpPort')

        self.AgentID = config.get('Collector',
                                         'AgentID')

        self.ApplicationName = config.get('Collector',
                                     'ApplicationName')

        # self._newSharedObject();
        self.version = AGENT_VERSION
        self.config = config
        self.startTimestamp = int(time.time()*1000)




    def getSpanHost(self):
        return ( self.CollectorSpanIp,self.CollectorSpanPort)

    def getStatHost(self):
        return ( self.CollectorStatIp,self.CollectorStatPort)

    def getTcpHost(self):
        return ( self.CollectorTcpIp, self.CollectorTcpPort)

    # def _newSharedObject(self):
    #     with open(self.sharedObjectAddress, "w") as f:
    #         # f.write(b"Hello Python!\n")
    #         f.truncate(1024)
    #         f.close()

    def clean(self):
        pass
        # os.remove(self.sharedObjectAddress)
