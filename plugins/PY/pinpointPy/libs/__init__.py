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
from pinpointPy.pinpoint import get_logger


def __monkey_patch(*args, **kwargs):
    for key in kwargs:
        if kwargs[key]:
            module = importlib.import_module('pinpointPy.libs.' + key)
            monkey_patch = getattr(module, 'monkey_patch')
            if callable(monkey_patch):
                try:
                    monkey_patch()
                except Exception as e:
                    get_logger().info(f'monkey_patch exception:{e}')


def monkey_patch_for_pinpoint(pymongo=True,
                              PyMysql=True,
                              pyRedis=True,
                              requests=True,
                              urllib=True,
                              sqlalchemy=True,
                              MySQLdb=True,
                              MysqlConnector=True):
    __monkey_patch(_pymongo=pymongo, _MySQLdb=MySQLdb, _PyMysql=PyMysql, _pyRedis=pyRedis, _requests=requests,
                   _urllib=urllib, _sqlalchemy=sqlalchemy,   _MysqlConnector=MysqlConnector)


__all__ = ['monkey_patch_for_pinpoint']
__version__ = '0.0.3'
__author__ = 'liu.mingyi@navercorp.com'
