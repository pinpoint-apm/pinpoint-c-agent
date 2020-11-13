#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20


def monkey_patch():

    from pinpoint.common import Interceptor
    try:
        import pymongo
        from .MongoClientPlugin import MongoClientPlugin

        Interceptors = [
            Interceptor(pymongo.collection,'find', MongoClientPlugin),
            Interceptor(pymongo.collection, 'insert', MongoClientPlugin),
            Interceptor(pymongo.collection, 'update', MongoClientPlugin),
            Interceptor(pymongo.collection, 'update_many', MongoClientPlugin),
            Interceptor(pymongo.collection, 'delete_one', MongoClientPlugin),
            Interceptor(pymongo.collection, 'delete_many', MongoClientPlugin),
            Interceptor(pymongo.collection, 'insert_many', MongoClientPlugin),
            Interceptor(pymongo.collection, 'insert_one', MongoClientPlugin),
        ]

        for interceptor in Interceptors:
            interceptor.enable()

    except ImportError:
        pass

__all__=['monkey_patch']

