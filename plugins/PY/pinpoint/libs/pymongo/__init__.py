#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 8/20/20


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

# from  pymongo import MongoClient
# import datetime
#
# client = MongoClient("mongodb://10.34.130.40:27017/?readPreference=primary&appname=MongoDB%20Compass%20Community&ssl=false")
# db = client.testdb1
# # print(client.address)
#
# # print(db)
#
# post = {"author": "Mike",
#          "text": "My first blog post!",
#  "tags": ["mongodb", "python", "pymongo"],
#          "date": datetime.datetime.utcnow()}
#
#
# posts = db.posts
# print(posts)
# post_id = posts.insert_one(post).inserted_id
# # print(post_id)
