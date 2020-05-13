#!/usr/bin/env python
# -*- coding: UTF-8 -*-


import pymysql


class DBContrl(object):

    def __init__(self, host, user, pw, db=None):
        self.host = host
        self.user = user
        self.pw = pw
        self.db = db
        self.conn = None
        self.cursor = None

    def con_db(self):
        if self.db is None:
            self.conn = pymysql.connect(host=self.host, user=self.user, passwd=self.pw)
        else:
            self.conn = pymysql.connect(host=self.host,user=self.user,passwd=self.pw, db=self.db)
        if self.conn is None:
            print("Connect to Database failed!")
        else:
            self.cursor = self.conn.cursor()

    def db_select(self, sql):
        self.cursor.execute(sql)
        result = self.cursor.fetchall()
        r = []
        for res in result:
            r.append(res)
        return r

    def db_close(self):
        self.cursor.close()
        self.conn.commit()
        self.conn.close()
