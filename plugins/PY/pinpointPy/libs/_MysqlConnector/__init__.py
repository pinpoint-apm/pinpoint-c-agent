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

from pinpointPy.Interceptor import Interceptor, intercept_once
from pinpointPy import get_logger


@intercept_once
def monkey_patch():
    try:
        from mysql.connector.cursor import MySQLCursor, MySQLCursorPrepared
        from .MysqlPlugin import MysqlPlugin

        Interceptors = [
            Interceptor(MySQLCursor, 'execute', MysqlPlugin),
            Interceptor(MySQLCursorPrepared, 'execute', MysqlPlugin),
        ]
        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError as e:
        get_logger().warning(f'import _MysqlConnector {e}')
        import sys
        if 'unittest' in sys.modules.keys():
            raise e

    try:
        from mysql.connector.cursor_cext import CMySQLCursor, CMySQLCursorPrepared
        from .CMysqlPlugin import CMysqlPlugin

        Interceptors = [
            Interceptor(CMySQLCursor, 'execute', CMysqlPlugin),
            Interceptor(CMySQLCursorPrepared, 'execute', CMysqlPlugin),
        ]
        for interceptor in Interceptors:
            interceptor.enable()
    except ImportError as e:
        get_logger().info(f"exception at {e}")
        import sys
        if 'unittest' in sys.modules.keys():
            raise e


__all__ = ['monkey_patch']
__version__ = '0.0.3'
__author__ = 'liu.mingyi@navercorp.com'
