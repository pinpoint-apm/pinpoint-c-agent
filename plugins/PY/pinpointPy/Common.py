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

from pinpointPy import Defines, pinpoint, get_logger
from pinpointPy.TraceContext import get_trace_context
from functools import wraps


class Trace:
    def __init__(self, name):
        self.name = name

    def onBefore(self, *args, **kwargs):
        raise NotImplementedError("onBefore")

    def onEnd(self, ret):
        raise NotImplementedError("onEnd")

    def onException(self, ret):
        raise NotImplementedError("onException")

    def __call__(self, func):
        @wraps(func)
        def pinpointTrace(*args, **kwargs):
            ret = None
            try:
                args, kwargs = self.onBefore(*args, **kwargs)
                ret = func(*args, **kwargs)
            except Exception as e:
                self.onException(e)
                get_logger().info(f"{func.__name__} catch {e}")
                raise e
            finally:
                return self.onEnd(ret)
        return pinpointTrace


class PinTrace:

    def __init__(self, name):
        self.name = name

    def onBefore(self, parentId: int, *args, **kwargs):
        traceId = pinpoint.with_trace(parentId)
        get_trace_context().set_parent_id(traceId)
        return traceId, args, kwargs

    @staticmethod
    def isSample(*args, **kwargs):
        ret, parentId = get_trace_context().get_parent_id()
        if ret:
            return True, parentId, args, kwargs
        else:
            return False, -1, args, kwargs

    @classmethod
    def _isSample(cls, *args, **kwargs):
        return cls.isSample(*args, **kwargs)

    def onEnd(self, traceId, ret):
        parentId: int = pinpoint.end_trace(trace_id=traceId)
        get_trace_context().set_parent_id(parentId)

    def onException(self, traceId, e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name = func.__name__

        @wraps(func)
        def pinpointTrace(*args, **kwargs):
            sampled, parentId, nArgs, nKwargs = self._isSample(*args, **kwargs)
            if not sampled:
                return func(*nArgs, **nKwargs)
            ret = None
            get_logger().debug(f"call {self.func_name}.onBefore")
            traceId, nArgs, nKwargs = self.onBefore(
                parentId, *nArgs, **nKwargs)
            try:
                get_logger().debug(f"call {self.func_name}")
                ret = func(*nArgs, **nKwargs)
                return ret
            except Exception as e:
                get_logger().debug(f"call {self.func_name}.onException")
                self.onException(traceId, e)
                raise e
            finally:
                get_logger().debug(f"call {self.func_name}.onEnd")
                self.onEnd(traceId, ret)
        return pinpointTrace

    def getUniqueName(self):
        return self.name


class TraceIdObject:
    def __init__(self, id: int) -> None:
        self.traceId = id


class PinTraceV1(PinTrace):
    def onBefore(self, parentId: int, *args, **kwargs):
        trace_id, args, kwargs = super().onBefore(parentId, *args, **kwargs)
        return TraceIdObject(trace_id), args, kwargs

    def onEnd(self, traceIdObj: TraceIdObject, ret):
        return super().onEnd(traceIdObj.traceId, ret)

    def onException(self, traceId: TraceIdObject, e):
        raise NotImplementedError()


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


class GenPinHeader:
    """GenPinHeader

    Args:
        metaclass (_type_, optional): _description_. Defaults to ABCMeta.
    Example:
        ```
        def GetHeader(self,parent_host,parent_name,url,...)->PinHeader:
            header = PinHeader()
            header.ParentHost= parent_host
            header.ParentName= parent_name
            header.Url = url
            ...
            return header
        ```
    Returns:
        _type_: _description_
    """

    def GetHeader(self, *args, **kwargs) -> PinHeader:
        raise NotImplemented("GetHeader")


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
        self.name: str = name
        self.getHeader = userGenHeaderCb

    @staticmethod
    def isSample(*args, **kwargs):
        return True, 0, args, kwargs

    def onBefore(self, parentId: int, *args, **kwargs):
        traceId, args_n, kwargs_n = super().onBefore(parentId, *args, **kwargs)
        header = self.getHeader.GetHeader(*args_n, **kwargs_n)

        sid = pinpoint.gen_sid()
        pinpoint.add_trace_header(Defines.PP_SPAN_ID, sid, traceId)
        pinpoint.add_context(Defines.PP_SPAN_ID, sid, traceId)

        pinpoint.add_trace_header(
            Defines.PP_INTERCEPTOR_NAME, self.name, traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_context(
            Defines.PP_APP_NAME, pinpoint.app_name(), traceId)
        pinpoint.add_trace_header(
            Defines.PP_APP_ID, pinpoint.app_id(), traceId)

        pinpoint.add_trace_header(Defines.PP_REQ_URI, header.Url, traceId)
        pinpoint.add_trace_header(Defines.PP_REQ_SERVER, header.Host, traceId)
        pinpoint.add_trace_header(
            Defines.PP_REQ_CLIENT, header.RemoteAddr, traceId)
        pinpoint.add_trace_header(
            Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)
        pinpoint.add_context(Defines.PP_SERVER_TYPE, Defines.PYTHON, traceId)

        pinpoint.add_context(Defines.PP_HEADER_PINPOINT_SAMPLED, "s1", traceId)

        if header.ParentType != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_TYPE, header.ParentType, traceId)
        if header.ParentName != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_NAME, header.ParentName, traceId)
        if header.ParentHost != '':
            pinpoint.add_trace_header(
                Defines.PP_PARENT_HOST, header.ParentHost, traceId)

        tid = ''
        if header.ParentTid != '':
            tid = header.ParentTid
            pinpoint.add_trace_header(Defines.PP_PARENT_SPAN_ID, tid, traceId)
            pinpoint.add_trace_header(Defines.PP_NEXT_SPAN_ID, sid, traceId)
        else:
            tid = pinpoint.gen_tid()

        pinpoint.add_trace_header(Defines.PP_TRANSCATION_ID, tid, traceId)

        pinpoint.add_context(Defines.PP_TRANSCATION_ID, tid, traceId)

        if header.Error:
            pinpoint.mark_as_error(header.Error, header.Error, traceId, 0)

        return traceId, args, kwargs

    def onEnd(self, traceId, ret):
        super().onEnd(traceId, ret)

    def onException(self, traceId, e):
        pinpoint.mark_as_error(str(e), "", traceId)
        raise e
