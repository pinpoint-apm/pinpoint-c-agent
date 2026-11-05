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

# !/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 2/4/21

from pinpointPy.libs import monkey_patch_for_pinpoint
from pinpointPy.pinpoint import set_agent, app_id, app_name, gen_tid, get_logger
from pinpointPy.TraceContext import set_trace_context, thread_local_context, asyncio_local_context
from pinpointPy.Common import PinTransaction, GenPinHeader, PinHeader, enable_experiment_plugins


def use_thread_local_context():
    get_logger().debug("use_thread_local_context")
    set_trace_context(thread_local_context())


def use_asyncio_local_context():
    get_logger().debug("use_asyncio_local_context")
    set_trace_context(asyncio_local_context())


__all__ = ['monkey_patch_for_pinpoint', 'use_thread_local_context', 'use_asyncio_local_context',
           'set_agent', 'app_id', 'app_name', 'gen_tid', 'get_logger', 'PinTransaction', 'GenPinHeader', 'PinHeader', 'enable_experiment_plugins']

# DO NOT Change it by hand, it managed by CI(sync-component-versions)
def _resolve_version():
    try:
        from importlib.metadata import version as _dist_version
        return _dist_version("pinpointPy")
    except Exception:
        return "1.4.2"

__version__ = _resolve_version()
__author__ = 'liu.mingyi@navercorp.com'


# Changes
# 1.4.1 11-19-2024
# - add celery plugins and testcase
# 1.4.0
# - use_asyncio_local_context
