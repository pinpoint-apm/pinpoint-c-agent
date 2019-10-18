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
from PinpointAgent.Type import PHP


class PinpointAgent(object):
    def __init__(self,app_id,app_name,serviceType=PHP):
        self.app_id =app_id
        self.app_name = app_name
        self.service_type = serviceType

    def start(self):
        raise NotImplementedError()

    def sendSpan(self,stack):
        raise NotImplementedError()

    def stop(self):
        raise NotImplementedError()
