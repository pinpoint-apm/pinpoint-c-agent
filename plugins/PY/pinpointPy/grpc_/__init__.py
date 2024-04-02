# ------------------------------------------------------------------------------
#  Copyright  2024. NAVER Corp.                                                -
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

try:
    import grpc_interceptor
except:
    raise ModuleNotFoundError(
        '''
            pinpointPy.grpc_ needs grpc_interceptor, try to install by "pip install grpc-interceptor".
            read more: `https://github.com/d5h-foss/grpc-interceptor` and `https://github.com/pinpoint-apm/pinpoint-c-agent`
        ''')


from pinpointPy.grpc_.GrpcRequestPlugins import PinpointServerInterceptor

__version__ = '0.0.1'
__all__ = ['PinpointServerInterceptor']
