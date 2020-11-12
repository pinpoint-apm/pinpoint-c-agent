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
from pinpoint.common import Interceptor

try:
    import requests
    from .NextSpanPlugin import NextSpanPlugin


    # HookSet = [
    #     ('requests','post', requests, requests.post),
    #     ('requests','get', requests, requests.get),
    #  ('requests','patch', requests, requests.patch)
    # ]
    #
    # for hook in HookSet:
    #     new_requests = NextSpanPlugin(hook[0], '')
    #     setattr(hook[2],hook[1], new_requests(hook[3]))

    Interceptors = [
        Interceptor(requests.post,'find', NextSpanPlugin),
        Interceptor(requests.get, 'insert', NextSpanPlugin),
        Interceptor(requests.patch, 'update', NextSpanPlugin),
    ]

    for interceptor in Interceptors:
        interceptor.enable()


except ImportError:
    pass
