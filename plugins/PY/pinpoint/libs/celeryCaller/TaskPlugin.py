#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/29/20


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

import pinpointPy

from pinpoint.common import PinpointCommonPlugin, PP_DESTINATION, PP_NEXT_SPAN_ID, generateSid, \
    PP_SERVER_TYPE, PP_METHOD_CALL, PP_REMOTE_METHOD


class TaskPlugin(PinpointCommonPlugin):
    def onBefore(self, *args, **kwargs):
        super().onBefore(*args, **kwargs)
        pinpointPy.add_clue(PP_SERVER_TYPE, PP_REMOTE_METHOD)
        pinpointPy.add_clue(PP_DESTINATION,'test-virtual-machine')
        nsid = generateSid()
        pinpointPy.set_context_key(PP_NEXT_SPAN_ID, nsid)
        pinpointPy.add_clue(PP_NEXT_SPAN_ID,nsid)
        print(args[0].name)
        return args,kwargs




