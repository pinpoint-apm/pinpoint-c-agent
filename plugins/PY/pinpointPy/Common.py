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

# Created by eeliu at 3/5/20

from . import Defines
from . import pinpoint
from abc import ABCMeta, abstractmethod

class PinTrace(object):
    E_PER_REQ = 1
    E_FUNCTION = 2

    def __init__(self, name):
        self.name = name

    def isSample(self, args):
        '''
        if not root, no trace
        :return:
        '''
        if pinpoint.trace_has_root() and pinpoint.get_context(Defines.PP_HEADER_PINPOINT_SAMPLED) == "s1":
            return True
        else:
            return False

    def onBefore(self, *args, **kwargs):
        pinpoint.with_trace()
        return args,kwargs

    def onEnd(self, ret):
        pinpoint.end_trace()

    def onException(self, e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name = func.__name__

        def pinpointTrace(*args, **kwargs):
            if not self.isSample((args, kwargs)):
                return func(*args, **kwargs)

            ret = None
            try:
                args, kwargs = self.onBefore(*args, **kwargs)
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)
        return pinpointTrace

    def getFuncUniqueName(self):
        return self.name

class PinHeader:
    def __init__(self) -> None:
        # Path field in pinpoint-web
        self.Url = ''
        # ENDPOINT field
        self.Host = ''
        # REMOTE_ADDRESS field
        self.RemoteAddr = ''
        # parents's type
        # 8660: kafka type
        # 1800: go remote call type
        # 1310: zookeeper node Notice
        # the more: https://github.com/pinpoint-apm/pinpoint/blob/5d2e5f6ad533446353bba5eec076a2a821ef712b/commons/src/main/java/com/navercorp/pinpoint/common/trace/ServiceType.java#L24
        self.ParentType = ''
        # naming the parents(or upstream)
        self.ParentName = ''
        # REMOTE_ADDRESS field (the same as RemoteAddr)
        self.ParentHost = ''
        # create a new one, or use an exist parents Tid, this Tid used to generate call-tree
        self.ParentTid = ''
        # if find a error, just fill here. it could rise an error(read mark) in pinpoint-web
        self.Error = ''


class GenPinHeader(metaclass=ABCMeta):
    """GenPinHeader

    Args:
        metaclass (_type_, optional): _description_. Defaults to ABCMeta.
    Example:
        ```
        def GetHeader(self,parenthost,parentname,url,...)->PinHeader:
            header = PinHeader()
            header.ParentHost= parenthost
            header.ParentName= ParentName
            header.Url = url
            ...
            return header
        ```
    Returns:
        _type_: _description_
    """
    @abstractmethod
    def GetHeader(self, *args, **kwargs) -> PinHeader:
        return PinHeader()
    
class PinTransaction(PinTrace):

    def __init__(self, name: str, userGenHeaderCb: GenPinHeader):
        """pinpointPy user entry point

        Example: 

            ```
            from pinpointPy.Common import GenPinHeader, PinHeader, PinTransaction
            class MsgHeader(GenPinHeader):
                def GetHeader(self, *args, **kwargs) -> PinHeader:
                    # geneate some useful information from args/kwargs
                    header = PinHeader()
                    ...
                    return header
            @PinTransaction('run', MsgHeader())
            def run(msg):
                do_something()
            ```
        Args:
            name (str): entry points name(showing pinpoint)
            userGenHeaderCb (GenPinHeader): This helps getting header from current function 
        """
        super().__init__(name)
        self.name = name
        self.getHeader = userGenHeaderCb

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0

    def onBefore(self, *args, **kwargs):
        args, kwargs = super().onBefore(*args, **kwargs)
        header = self.getHeader.GetHeader(*args, **kwargs)

        sid = pinpoint.gen_sid()
        pinpoint.add_context(Defines.PP_SPAN_ID, sid)
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid)

        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.name)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name())
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id())

        pinpoint.add_trace_header(Defines.PP_REQ_URI, header.Url)
        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, header.Host)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, header.RemoteAddr)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON)
        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1")

        if header.ParentType != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, header.ParentType)

        if header.ParentName != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, header.ParentName)

        if header.ParentHost != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, header.ParentHost)

        tid = ''
        if header.ParentTid != '':
            tid = header.ParentTid
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, tid)
            pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, sid)
        else:
            tid = pinpoint.gen_tid()

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid)
        pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid)

        if header.Error:
            pinpoint.mark_as_error(header.Error, header.Error, 0)

        return args, kwargs

    def onEnd(self, ret):
        super().onEnd(ret)

    def onException(self, e):
        pinpoint.mark_as_error(str(e), "", 0)
        raise e
