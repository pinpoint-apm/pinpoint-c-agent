import random
import pinpointPy
from pinpoint.common.Defines import *
from pinpoint.settings import *

class Candy(object):
    def __init__(self,name):
        self.name = name

    def onBefore(self,*args, **kwargs):
        pinpointPy.start_trace()

    def onEnd(self,ret):
        pinpointPy.end_trace()

    def onException(self,e):
        pinpointPy.add_clue(PP_ADD_EXCEPTION, str(e))

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            ret = None
            self.onBefore(*args, **kwargs)
            try:
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)

        return pinpointTrace

    def getFuncUniqueName(self):
        return self.name