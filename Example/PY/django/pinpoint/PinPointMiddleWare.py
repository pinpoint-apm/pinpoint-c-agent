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

from django.utils.deprecation import MiddlewareMixin

from .BaseDjangoRequestPlugins import BaseDjangoRequestPlugins

class PinPointMiddleWare(MiddlewareMixin):
    def __init__(self, get_response=None):
        self.get_response = get_response
        super().__init__(self.get_response)
        self.request_plugin = BaseDjangoRequestPlugins("Django Web App", __name__)

    def process_request(self,request):
        print("*****MyMiddleware request******")
        self.request_plugin.onBefore(self,request)


    def process_response(self,request,response):
        print("*****MyMiddleware response******")
        self.request_plugin.onEnd(response)
        return response


    def process_exception(self, request, exception):
        self.request_plugin.onException(exception)
