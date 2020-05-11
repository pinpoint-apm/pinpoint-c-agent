import random
from http.server import BaseHTTPRequestHandler, HTTPServer
import socketserver
import time

from plugins.BaseHTTPRequestPlugins import BaseHTTPRequestPlugins
from plugins.PinpointCommonPlugin import PinpointCommonPlugin

import test_function
from app1 import user_function
import person, student, teacher, doctor
import test_recursion
import test_generatior
import test_higher_order
import test_return
import test_lambda
import test_decorator
import test_partial
import test_band
from test_band import BandClass
import test_mixin
import test_special
import test_private
import test_abstract
from test_static_class_method import Method
import test_exception
from test_exception import UserDefineException
import test_args
import test_returns
from DBControl import DBContrl

class SimpleWebServer(BaseHTTPRequestHandler):
    @PinpointCommonPlugin('SimpleWebServer',__name__)
    def getAgent(self,headers):
        time.sleep(1)
        if 'User-Agent' in headers:
            print(headers['User-Agent'])
            return headers['User-Agent']
        else:
            return 'null'

    @BaseHTTPRequestPlugins('SimpleWebServer',__name__)
    def do_GET(self):
        # print(self.headers)
        # self.getAgent(self.headers)

        #Call user-defined function.
        # h = test_function.test_func1("hello", "world")

        #Call user-defined function.
        # instance = test_function.TestUserFunc1("Evy", "99")
        # h = instance.test_func2()

        #Call user-defined function in a package.
        # h = user_function.test_func3("hello", "world")

        #Call user-defined function in a package.
        # instance = user_function.TestUserFunc2("Evy", "99")
        # h = instance.test_func4()

        #Call inherit function
        # instance = student.Student()
        # s = instance.eat()
        # instance = teacher.Teacher()
        # t = instance.eat()
        # instance = doctor.Doctor()
        # d1 = instance.eat()
        # d2 = instance.other()
        # h = '''<h3>%s</h3>
        #       <h3>%s</h3>
        #       <h3>%s But %s</h3>''' % (s, t, d1, d2)

        #Call Higher-order
        @PinpointCommonPlugin('', __name__)
        def f(x):
            print(x)
            return x * x

        l1 = [1, 2, 3, 4, 5, 6, 7, 8, 9]
        r = map(f, l1)
        # r = map(test_higher_order.f, l1)
        h = "["
        for i in r:
            h = h + str(i) + ","
        h = h + "]"

        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(bytes(h, encoding='utf-8'))

    @BaseHTTPRequestPlugins('SimpleWebServer',__name__)
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])  
        post_data = self.rfile.read(content_length)
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        print(self.headers)
        print(post_data)
        self.wfile.write(bytes('false',encoding='utf-8'))

PORT = 9001

with socketserver.TCPServer(("", PORT), SimpleWebServer,False) as httpd:
    print("start at:", PORT)
    httpd.allow_reuse_address = True
    httpd.server_bind()
    httpd.server_activate()
    httpd.serve_forever()


