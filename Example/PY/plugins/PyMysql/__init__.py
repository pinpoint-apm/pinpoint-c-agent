import pymysql
from pymysql.cursors import Cursor
from plugins.PyMysqlPlugin import PyMysqlPlugin

HookSet = [
    ('pymysql', 'connect', pymysql, pymysql.connect),
    ('Cursor','execute', Cursor, Cursor.execute),
    ('Cursor','fetchall', Cursor, Cursor.fetchall)
]

for hook in HookSet:
    new_Cursor = PyMysqlPlugin(hook[0], '')
    setattr(hook[2],hook[1], new_Cursor(hook[3]))
