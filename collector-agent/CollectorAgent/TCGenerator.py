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


# keyWord ={
#     1:'agentId',
#     2:'applicationName',
#     3:'agentStartTime',
#     4:'transactionId',
#     5:'spanId',
#     6:'parentSpanId',
#     7:'spanId',
#     8:'parentSpanId ',
#     9:'startTime',
#     10:'elapsed ',
#     11:'rpc',
#     12:'serviceType',
#     13:'endPoint',
#     14:'remoteAddr',
#     15:'annotations',
#     16:'flag',
#     17:'err',
#     18:'spanEventList',
#     19:'parentApplicationName',
#     20:'parentApplicationType',
#     21:'acceptorHost',
#     25:'apiId',
#     26:'exceptionInfo',
#     30:'pplicationServiceType',
#     31:'loggingTransactionInfo',
#     32:'key',
#     33:'value'
# }

# from Intercept import interceptManger


class TCGenerator(object):

    @staticmethod
    def fetchIfExist(jMap, name):
        '''

        :param dict jMap:
        :param name:
        :return:
        '''
        if jMap.has_key(name):
            return jMap[name]
        return None

    @staticmethod
    def __transactionIDHelper(interManger, id):
        '''

        :param InterceptManager interManger:
        :param int id:
        :return:
        '''

        # return str(interManger.ac.AgentID)+'^'+str(interManger.startTimeStamp)+'^'+str(id)


class ThriftProtocolUtil(object):

    @staticmethod
    def _parseStrField(str):
        '''

        :param string str:
        :return dict:
        '''
        ret = {}
        for item in str.split():
            key, value = item.split(sep='=')
            ret[key] = value
        return ret

    @staticmethod
    def _parseDotFormat(time):
        '''

        :param string time:
        :return:
        '''
        sec, ms = time.split(sep='.')
        return int(sec) * 1000 + int(ms)
