#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 3/5/20

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

import pinpointPy
import random

PYTHON = '1700'
PYTHON_METHOD_CALL='1701'
PYTHON_REMOTE_METHOD = '9900'
MYSQL = '2101'

###############################################################

# user should set below before use
APP_ID ='python-app-id' # application id
APP_NAME ='python-app-name' # application name 
COLLECTOR_HOST='unix:/tmp/collector-agent.sock'

###############################################################
HTTP_PINPOINT_PSPANID = 'HTTP_PINPOINT_PSPANID'
HTTP_PINPOINT_SPANID = 'HTTP_PINPOINT_SPANID'
HTTP_PINPOINT_TRACEID = 'HTTP_PINPOINT_TRACEID'
HTTP_PINPOINT_PAPPNAME = 'HTTP_PINPOINT_PAPPNAME'
HTTP_PINPOINT_PAPPTYPE = 'HTTP_PINPOINT_PAPPTYPE'
HTTP_PINPOINT_HOST = 'HTTP_PINPOINT_HOST'

PINPOINT_PSPANID = 'Pinpoint-Pspanid'
PINPOINT_SPANID = 'Pinpoint-Spanid'
PINPOINT_TRACEID = 'Pinpoint-Traceid'
PINPOINT_PAPPNAME ='Pinpoint-Pappname'
PINPOINT_PAPPTYPE ='Pinpoint-Papptype'
PINPOINT_HOST = 'Pinpoint-Host'

NGINX_PROXY = 'Pinpoint-ProxyNginx'
HTTP_NGINX_PROXY = 'HTTP_Pinpoint-ProxyNginx'
APACHE_PROXY = 'PINPOINT_PROXYAPACHE'
HTTP_APACHE_PROXY = 'HTTP_PINPOINT_PROXYAPACHE'

SAMPLED = 'Pinpoint-Sampled'
HTTPD_SAMPLED = 'HTTP_PINPOINT_SAMPLED'
ServerType='stp'
FuncName='name'
PY_ARGS= '-1'
PY_RETURN='14'
PROXY_HTTP_HEADER=300
SQL_ID = 20
SQL=21
SQL_METADATA=22
SQL_PARAM=  23
SQL_BINDVALUE=24
STRING_ID=30
HTTP_URL='40'
HTTP_PARAM='41'
HTTP_PARAM_ENTITY='42'
HTTP_COOKIE='45'
HTTP_STATUS_CODE='46'
HTTP_INTERNAL_DISPLAY=48
HTTP_IO=49
MESSAGE_QUEUE_URI=100


MYSQL='2101'
REDIS='8200'
REDIS_REDISSON='8203'
REDIS_REDISSON_INTERNAL='8204'
MEMCACHED='8050'
pinpointPy.set_agent(collector_host=COLLECTOR_HOST,trace_limit=-1)
# pinpointPy.set_agent(collector_host='Tcp:10.34.130.152:6666',trace_limit=-1)

# def output(msg):
#     print(msg)
#
pinpointPy.enable_debug(None)

class Candy(object):
    def __init__(self,class_name,module_name):
        self.class_name = class_name
        self.module_name = module_name


    def onBefore(self,*args, **kwargs):
        pinpointPy.start_trace()
        return (args,kwargs)

    def onEnd(self,ret):
        pinpointPy.end_trace()

    def onException(self,e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            ret = None
            # print("start", self.func_name)
            args, kwargs = self.onBefore(*args, **kwargs)
            try:
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                # print("end", self.func_name)
                self.onEnd(ret)

        return pinpointTrace

    def generateTid(self):
        return ('%s^%s^%s') % (APP_ID,str(pinpointPy.start_time()), str(pinpointPy.unique_id()))

    def generateSid(self):
        return str(random.randint(0,2147483647))

    def getFuncUniqueName(self):
        if self.class_name:
            return '%s\%s.%s'%(self.module_name,self.class_name,self.func_name)
        else:
            return '%s\%s'%(self.module_name,self.func_name)

if __name__ == '__main__':

    @Candy('main',__name__)
    def run():
        print("run")

    run()

    run()
