import asyncio
import tornado.ioloop
import tornado.web
import tornado.httpclient
import tornado.gen as gen
from plugins.TornadoHTTPRequestPlugins import TornadoHTTPRequestPlugins
from plugins.PinpointCommonPlugin import PinpointCommonPlugin

## copy from https://www.tornadoweb.org/en/stable/
class MainHandler(tornado.web.RequestHandler):
    @TornadoHTTPRequestPlugins('tornado.web.RequestHandler', __name__)
    async def get(self):
        # await self.get_1()
        # await self.get_2()
        task1 = asyncio.create_task(self.get_1())
        task2 = asyncio.create_task(self.get_2())
        await task1
        await task2
        self.write("Hello, world")

    @PinpointCommonPlugin('',__name__)
    async def get_1(self,t = 2):
        await gen.sleep(0.01)
        if t == 0:
            return
        else:
            t-=1
            await self.get_1(t)

    @PinpointCommonPlugin('',__name__)
    async def get_2(self):
        http_client = tornado.httpclient.AsyncHTTPClient()
        try:
            response = await http_client.fetch("http://www.google.com")
        except Exception as e:
            print("Error: %s" % e)
        else:
            self.write(response.body)


def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
    ])

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()