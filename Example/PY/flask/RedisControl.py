#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import redis


class RedisControl(object):

    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.conn = None

    def connection(self):
        self.conn = redis.Redis(host=self.host, port=self.port)

    def set(self, key, value):
        self.conn.set(key, value)
        return self.conn.get(key)

    def get(self, key):
        return "%s:%s" % (key, self.conn.get(key))

    def delete(self, key):
        self.conn.delete(key)
        return "Delete %s success!"
