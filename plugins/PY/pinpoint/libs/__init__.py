# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.                                                -
#                                                                              -
#  Licensed under the Apache License, Version 2.0 (the "License");             -
#  you may not use this file except in compliance with the License.            -
#  You may obtain a copy of the License at                                     -
#                                                                              -
#   http://www.apache.org/licenses/LICENSE-2.0                                 -
#                                                                              -
#  Unless required by applicable law or agreed to in writing, software         -
#  distributed under the License is distributed on an "AS IS" BASIS,           -
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    -
#  See the License for the specific language governing permissions and         -
#  limitations under the License.                                              -
# ------------------------------------------------------------------------------
import importlib
from .internal import gl

def __monkey_patch(*args,**kwargs):
    for key in kwargs:
        if kwargs[key]:
            module = importlib.import_module('pinpoint.libs.'+key)
            monkey_patch = getattr(module,'monkey_patch')
            if callable(monkey_patch):
                monkey_patch()



def monkey_patch_for_pinpoint(pymongo=True,PyMysql=True,pyRedis=True,requests=True,urllib=True,
                       sqlalchemy=True,aioHttp=False,MySQLdb=True,amqp=True,kombu=True,celeryCaller=True):
    if not gl.patched:
        gl.patched = True
        __monkey_patch(pymongo=pymongo,PyMysql=PyMysql,pyRedis=pyRedis,requests=requests,urllib=urllib,
                       sqlalchemy=sqlalchemy,aioHttp=aioHttp,MySQLdb=MySQLdb,amqp=amqp,kombu=kombu,celeryCaller=celeryCaller)

__all__=['monkey_patch_for_pinpoint']
