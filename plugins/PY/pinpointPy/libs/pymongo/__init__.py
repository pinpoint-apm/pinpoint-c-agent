#!/usr/bin/env python
# -*- coding: UTF-8 -*-

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

# Created by eeliu at 8/20/20
from ...Interceptor import Interceptor,intercept_once

@intercept_once
def monkey_patch():

    try:
        from pymongo.collection import Collection
        from .MongoClientPlugin import MongoClientPlugin
        # print(pymongo.collection)
        Interceptors = [
            Interceptor(Collection,'find', MongoClientPlugin),
            # Interceptor(Collection, 'insert', MongoClientPlugin),
            # Interceptor(Collection, 'update', MongoClientPlugin),
            Interceptor(Collection, 'update_many', MongoClientPlugin),
            Interceptor(Collection, 'delete_one', MongoClientPlugin),
            Interceptor(Collection, 'delete_many', MongoClientPlugin),
            Interceptor(Collection, 'insert_many', MongoClientPlugin),
            Interceptor(Collection, 'insert_one', MongoClientPlugin),
        ]
        
        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError:
        pass

__all__=['monkey_patch']

