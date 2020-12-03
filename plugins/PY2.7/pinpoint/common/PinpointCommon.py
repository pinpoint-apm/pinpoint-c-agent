import random
import pinpointPy
from .Defines import *
from ..settings import *

class Candy(object):
    def __init__(self,class_name,module_name):
        self.class_name = class_name
        self.module_name = module_name


    def onBefore(self,*args, **kwargs):
        pinpointPy.start_trace()
        return (args,kwargs)

    def onEnd(self,ret):
        pinpointPy.end_trace()

    def onException(self,e):
        raise NotImplementedError()

    def __call__(self, func):
        self.func_name=func.__name__
        def pinpointTrace(*args, **kwargs):
            ret = None
            args, kwargs = self.onBefore(*args, **kwargs)
            try:
                ret = func(*args, **kwargs)
                return ret
            except Exception as e:
                self.onException(e)
                raise e
            finally:
                self.onEnd(ret)

        return pinpointTrace

    def generateTid(self):
        return ('%s^%s^%s') % (APP_ID,str(pinpointPy.start_time()), str(pinpointPy.unique_id()))

    def generateSid(self):
        return str(random.randint(0,2147483647))

    def getFuncUniqueName(self):
        if self.class_name:
            return '%s\%s.%s'%(self.module_name,self.class_name,self.func_name)
        else:
            return '%s\%s'%(self.module_name,self.func_name)