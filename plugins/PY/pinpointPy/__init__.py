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

#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 2/4/21
__version__ = "0.0.5"
from .libs import monkey_patch_for_pinpoint
from .common import APP_ID,APP_NAME
from _pinpointPy import set_agent

__all__= ['monkey_patch_for_pinpoint','APP_ID','APP_NAME','set_agent']