import random
from http.server import BaseHTTPRequestHandler, HTTPServer
import socketserver
import time

from plugins.BaseHTTPRequestPlugins import BaseHTTPRequestPlugins
from plugins.PinpointCommonPlugin import PinpointCommonPlugin

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


