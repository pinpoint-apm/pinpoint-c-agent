#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20


from pinpoint.common import Interceptor
from .MongoClientPlugin import MongoClientPlugin
try:


    from motor.core import AgnosticCollection

    Interceptors = [
        Interceptor(AgnosticCollection,'find', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'find_one', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'insert', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'update', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'update_many', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'delete_one', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'delete_many', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'insert_many', MongoClientPlugin),
        # Interceptor(AgnosticCollection, 'insert_one', MongoClientPlugin),
    ]

    for interceptor in Interceptors:
        interceptor.enable()

except ImportError as e:
    print(e)

# try:
#
#     # class MongoPlugin(Wrapper):
#         # def find(self,*args, **kwargs):
#         #     origin = getattr(self.origObj, 'find')
#         #     print("bingo bingo bingo")
#         #     return origin(*args,**kwargs)
#
#     #
#     # from motor.motor_asyncio import AsyncIOMotorClient
#     # origin = AsyncIOMotorClient.__getitem__
#     #
#     # def getitem(self, key):
#     #     ## call origin class
#     #     return origin(self,key)
#     #
#     # AsyncIOMotorClient.__getitem__ = getitem
#
#     pass
#
#
# except ImportError as e:
#     print(e)
