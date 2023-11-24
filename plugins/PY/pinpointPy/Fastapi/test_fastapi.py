import unittest


from pinpointPy.Fastapi import PinPointMiddleWare, use_starlette_context
from pinpointPy import set_agent

from starlette_context.middleware import RawContextMiddleware
from starlette.middleware import Middleware

from fastapi import FastAPI
from fastapi.testclient import TestClient
from fastapi import Request


class Test_UT(unittest.TestCase):

    def setUp(self) -> None:
        middlewares = [
            Middleware(
                RawContextMiddleware
            ),
            Middleware(PinPointMiddleWare)
        ]
        use_starlette_context()
        app = FastAPI(title='pinpointpy test', middleware=middlewares)

        set_agent("cd.dev.test.py", "cd.dev.test.py",
                  'tcp:dev-collector:9999', -1)

        @app.get("/cluster/{name}")
        async def read_main(name, request: Request):
            return {"msg": f"Hello World,{name}"}

        self.app = app
        self.client = TestClient(app)

    def test_request_example(self):
        response = self.client.get("/cluster/abc")
        assert "ut" in response.headers
        self.assertEqual(response.headers['ut'], "/cluster/{name}")


if __name__ == '__main__':
    unittest.main()
