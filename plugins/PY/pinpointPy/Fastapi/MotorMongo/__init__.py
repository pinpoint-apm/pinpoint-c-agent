#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.                                                -
#                                                                              -
#  Licensed under the Apache License, Version 2.0 (the "License");             -
#  you may not use this file except in compliance with the License.            -
#  You may obtain a copy of the License at                                     -
#                                                                              -
#   http://www.apache.org/licenses/LICENSE-2.0                                 -
#                                                                              -
#  Unless required by applicable law or agreed to in writing, software         -
#  distributed under the License is distributed on an "AS IS" BASIS,           -
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    -
#  See the License for the specific language governing permissions and         -
#  limitations under the License.                                              -
# ------------------------------------------------------------------------------

# create by eelu

from ...Interceptor import Interceptor,intercept_once
from pymongo import monitoring

from .. import AsyCommon
from ... import pinpoint
from ... import Defines


class MotorComandPlugins(AsyCommon.AsynPinTrace):

    def __init__(self, name):
        super().__init__(name)

    def onBefore(self,parentId,*args, **kwargs):
        traceId,_,_=super().onBefore(parentId,*args,**kwargs)
        event = args[0]
        ###############################################################
        pinpoint.add_trace_header(Defines.PP_INTERCEPTOR_NAME, event.command_name, traceId)
        pinpoint.add_trace_header(Defines.PP_SERVER_TYPE, Defines.PP_MONGDB_EXE_QUERY,traceId)
        pinpoint.add_trace_header(Defines.PP_DESTINATION, event.connection_id[0], traceId)
        ###############################################################
        return event

    def onEnd(self,traceId, ret):
        super().onEnd(traceId,ret)
        return ret

    def onException(self,traceId, e):
        pinpoint.add_trace_header(Defines.PP_ADD_EXCEPTION, str(e),traceId)


class CommandLogger(monitoring.CommandListener):
    def __init__(self) -> None:
        self.CommandPlugins={}

    def started(self, event):
        sampled,parentId = MotorComandPlugins.isSample()
        if not sampled:
            return 

        if event.command_name not in self.CommandPlugins:
            self.CommandPlugins[event.command_name] = MotorComandPlugins(event.command_name)

        plugin = self.CommandPlugins[event.command_name]
        plugin.onBefore(parentId,event)

    def succeeded(self, event):
        sampled,traceId= MotorComandPlugins.isSample()
        if not sampled:
            return 
        plugin = self.CommandPlugins[event.command_name]
        plugin.onEnd(traceId,None)
        

    def failed(self, event):
        sampled,traceId= MotorComandPlugins.isSample()
        if not sampled:
            return 
        plugin = self.CommandPlugins[event.command_name]
        plugin.onException(traceId,None)  
        plugin.onEnd(traceId,None)


@intercept_once
def monkey_patch():
    monitoring.register(CommandLogger())

__all__=['monkey_patch']