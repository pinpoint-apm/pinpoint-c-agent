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
import os
import time

from CollectorAgent.GrpcAgentImplement import GrpcAgentImplement
from CollectorAgent.ThriftAgentImplement import ThriftAgentImplement
from Common.Logger import set_logger_file, set_logger_level, logger_enable_console
from PinpointAgent.Type import SUPPORT_THRIFT, SUPPORT_GRPC

AGENT_VERSION = '1.8.0-RC1'


class CollectorAgentConf(object):
    def __init__(self, config):
        '''

        :param ConfigParser config:
        '''

        self.CollectorSpanIp = ''
        self.CollectorSpanPort = ''
        self.CollectorStatIp = ''
        self.CollectorStatPort = ''
        self.CollectorAgentIp = ''
        self.CollectorAgentPort = ''
        self.max_pending_size = ''
        self.collector_type = -1

        self.collector_implement = None
        self.startTimestamp = int(time.time() * 1000)
        self.config = config
        self.version = AGENT_VERSION
        self.max_pending_size = 10000

        if not config:
            ## update system evn and it own a higher priority
            ## while, only support grpc
            self._parseSysEnv()
            return

        if config.has_option('Collector', 'collector.grpc.agent.ip') and \
                config.has_option('Collector', 'collector.grpc.stat.ip') and \
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
                                                  'collector.grpc.discardpolicy.maxpendingthreshold',fallback=10000)

            agentIsDocker = config.get('Common', 'Isdocker', fallback='false')

            self.agentIsDocker = True if agentIsDocker.lower() == 'true' else False

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
            self.agentIsDocker = False
            self.collector_type = SUPPORT_THRIFT
            self.collector_implement = ThriftAgentImplement

        self.log_dir = config.get('Common', 'LOG_DIR', fallback='/tmp')
        self.log_level = config.get('Common', 'Log_Level', fallback='DEBUG')
        self.webPort = self.config.get('Common', 'Web_Port')
        self._setLog(self.log_dir, self.log_level)

    def _parseSysEnv(self):
        try:
            self.CollectorSpanIp = os.environ['PP_COLLECTOR_AGENT_SPAN_IP']
            self.CollectorSpanPort = int(os.environ['PP_COLLECTOR_AGENT_SPAN_PORT'])
            self.CollectorAgentIp = os.environ['PP_COLLECTOR_AGENT_AGENT_IP']
            self.CollectorAgentPort = int(os.environ['PP_COLLECTOR_AGENT_AGENT_PORT'])
            self.CollectorStatIp = os.environ['PP_COLLECTOR_AGENT_STAT_IP']
            self.CollectorStatPort = int(os.environ['PP_COLLECTOR_AGENT_STAT_PORT'])
            self.agentIsDocker = True if os.environ.get('PP_COLLECTOR_AGENT_ISDOCKER').lower() == 'true' else False
            self.log_dir = os.getenv('PP_LOG_DIR', '/tmp/')
            self.log_level = os.getenv('PP_Log_Level', 'DEBUG')
            self.webPort = os.getenv('PP_WEB_PORT', '80')
            self.collector_type = SUPPORT_GRPC
            self.collector_implement = GrpcAgentImplement
            self._setLog(self.log_dir, self.log_level)
        except KeyError as e:
            # an exception is convenience for debugging
            raise RuntimeError('Please check your environment variable %s is missing' % (e))

    def _setLog(self, dir, level):
        if dir is not None:
            set_logger_file(dir)
        else:
            logger_enable_console()
        set_logger_level(level)

    def getSpanHost(self):
        return (self.CollectorSpanIp, self.CollectorSpanPort)

    def getStatHost(self):
        return (self.CollectorStatIp, self.CollectorStatPort)

    def getTcpHost(self):
        return (self.CollectorAgentIp, self.CollectorAgentPort)

    def getWebPort(self):
        return self.webPort

    def clean(self):
        pass
        # os.remove(self.sharedObjectAddress)
