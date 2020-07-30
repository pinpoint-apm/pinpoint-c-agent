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


from .PinpointCommon import *
import pinpointPy
import urlparse


class NextSpanPlugin(Candy):

    def __init__(self,class_name,module_name):
        super(NextSpanPlugin,self).__init__(class_name,module_name)
        self.nsid = ''

    def handleHttpHeader(self, url, headers):

        self.url = url
        # headers[SAMPLED] = pinpointPy.get_special_key(SAMPLED)
        if pinpointPy.check_tracelimit():
            headers[SAMPLED] = 's0'
            return
        else:
            headers[SAMPLED] = 's1'
        headers[PINPOINT_PAPPTYPE] = '1700'
        headers[PINPOINT_PAPPNAME] = APP_NAME
        headers['Pinpoint-Flags'] = "0"
        headers[PINPOINT_HOST] = self.getHostFromURL(self.url)
        headers[PINPOINT_TRACEID] = pinpointPy.get_special_key('tid')
        headers[PINPOINT_PSPANID] = pinpointPy.get_special_key('sid')
        self.nsid = self.generateSid()
        headers[PINPOINT_SPANID] = self.nsid

    def onBefore(self,*args, **kwargs):
        args, kwargs = super(NextSpanPlugin,self).onBefore(*args, **kwargs)
        ###############################################################
        pinpointPy.add_clue(FuncName,self.getFuncUniqueName())
        pinpointPy.add_clue(ServerType,PYTHON_METHOD_CALL)
        arg = self.get_arg(*args, **kwargs)
        pinpointPy.add_clues(PY_ARGS, arg)
        if "headers" in kwargs:
            self.handleHttpHeader(args[0], kwargs["headers"])
        else:
            kwargs["headers"] = {}
            self.handleHttpHeader(args[0], kwargs["headers"])
        ###############################################################
        return args, kwargs

    def onEnd(self,ret):
        ###############################################################
        pinpointPy.add_clue("dst", self.getHostFromURL(self.url))
        pinpointPy.add_clue("stp", PYTHON_REMOTE_METHOD)
        pinpointPy.add_clue('nsid', self.get_nsid())
        pinpointPy.add_clues(HTTP_URL, self.url)
        pinpointPy.add_clues(HTTP_STATUS_CODE, str(ret.status_code))
        pinpointPy.add_clues(PY_RETURN,str(ret))

        ###############################################################
        super(NextSpanPlugin,self).onEnd(ret)
        return ret

    def onException(self, e):
        pinpointPy.add_clue('EXP',str(e))

    def get_arg(self, *args, **kwargs):
        args_tmp = {}
        j = 0

        for i in args:
            args_tmp["arg["+str(j)+"]"] = (str(i))
            j += 1

        for k in kwargs:
            args_tmp[k] = kwargs[k]

        return str(args_tmp)

    def getHostFromURL(self, url):
        returl = urlparse.urlparse(url)
        return returl.netloc

    def get_nsid(self):
        return self.nsid
