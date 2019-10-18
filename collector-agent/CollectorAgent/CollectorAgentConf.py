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
from PinpointAgent.Type import SUPPORT_THRIFT, SUPPORT_GRPC
from CollectorAgent.ThriftAgentImplement import ThriftAgentImplement
from CollectorAgent.GrpcAgentImplement import GrpcAgentImplement
AGENT_VERSION = '1.8.0-RC1'


class CollectorAgentConf(object):
    def __init__(self,config):
        '''

        :param ConfigParser config:
        '''
        if  config.has_option('Collector','collector.grpc.agent.ip') and \
            config.has_option('Collector','collector.grpc.stat.ip') and \
            config.has_option('Collector', 'collector.grpc.span.ip'):
            self.CollectorSpanIp = config.get('Collector',
                                             'collector.grpc.span.ip')
            self.CollectorSpanPort = config.getint('Collector',
                                              'collector.grpc.span.port')
            self.CollectorStatIp = config.get('Collector',
                                              'collector.grpc.stat.ip')
            self.CollectorStatPort = config.getint('Collector',
                                              'collector.grpc.stat.port')
            self.CollectorAgentIp = config.get('Collector',
                                              'collector.grpc.agent.ip')
            self.CollectorAgentPort = config.getint('Collector',
                                             'collector.grpc.agent.port')
            self.max_pending_size = config.getint('Collector',
                                             'collector.grpc.discardpolicy.maxpendingthreshold')
            self.collector_type = SUPPORT_GRPC
            self.collector_implement = GrpcAgentImplement
        else:
            self.CollectorSpanIp = config.get('Collector',
                                             'CollectorSpanIp')
            self.CollectorSpanPort = config.getint('Collector',
                                              'CollectorSpanPort')
            self.CollectorStatIp = config.get('Collector',
                                              'CollectorStatIp')
            self.CollectorStatPort = config.getint('Collector',
                                              'CollectorStatPort')
            self.CollectorAgentIp = config.get('Collector',
                                              'CollectorTcpIp')
            self.CollectorAgentPort = config.getint('Collector',
                                             'CollectorTcpPort')
            self.collector_type = SUPPORT_THRIFT
            self.collector_implement = ThriftAgentImplement

        self.AgentID = config.get('Collector',
                                         'AgentID')
        self.ApplicationName = config.get('Collector',
                                     'ApplicationName')
        self.version = AGENT_VERSION
        self.config = config

    def getSpanHost(self):
        return ( self.CollectorSpanIp,self.CollectorSpanPort)

    def getStatHost(self):
        return ( self.CollectorStatIp,self.CollectorStatPort)

    def getTcpHost(self):
        return (self.CollectorAgentIp, self.CollectorAgentPort)

    def clean(self):
        pass
        # os.remove(self.sharedObjectAddress)
