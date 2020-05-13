
from django.utils.deprecation import MiddlewareMixin

from django.middleware.common import CommonMiddleware
from plugins.BaseDjangoRequestPlugins import BaseDjangoRequestPlugins

class MyMiddleware(MiddlewareMixin):
    def __init__(self, get_response=None):
        self.get_response = get_response
        super().__init__(self.get_response)
        self.request_plugin = BaseDjangoRequestPlugins("Django Web App", __name__)

    # @BaseDjangoRequestPlugins("Django Web App", __name__)
    def process_request(self,request):
        print("*****MyMiddleware request******")
        self.request_plugin.onBefore(self,request)


    def process_response(self,request,response):
        print("*****MyMiddleware response******")
        self.request_plugin.onEnd(response)
        return response


    def process_exception(self, request, exception):
        self.request_plugin.onException(exception)
