#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20



# ******************************************************************************
#   Copyright  2020. NAVER Corp.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ******************************************************************************

import _pinpointPy

from pinpointPy.common import PinpointCommonPlugin, PP_DESTINATION, PP_NEXT_SPAN_ID, generateSid, \
    PP_SERVER_TYPE, PP_REMOTE_METHOD, local_host_name


class TaskPlugin(PinpointCommonPlugin):
    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        _pinpointPy.add_clue(PP_SERVER_TYPE, PP_REMOTE_METHOD)
        _pinpointPy.add_clue(PP_DESTINATION,args[0].name)
        nsid = generateSid()
        _pinpointPy.set_context_key(PP_NEXT_SPAN_ID, nsid)
        _pinpointPy.add_clue(PP_NEXT_SPAN_ID,nsid)
        return args,kwargs




