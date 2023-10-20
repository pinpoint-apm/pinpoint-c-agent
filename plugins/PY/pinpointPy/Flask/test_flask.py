import unittest
from flask import Flask, request
from pinpointPy.Flask.PinPointMiddleWare import PinPointMiddleWare
from pinpointPy import set_agent, monkey_patch_for_pinpoint


class Test_Flask(unittest.TestCase):
    def setUp(self) -> None:
        app = Flask("abc")
        app.wsgi_app = PinPointMiddleWare(app, app.wsgi_app)
        app.config.update({
            "TESTING": True,
        })

        set_agent("cd.dev.test.py", "cd.dev.test.py",
                  'tcp:dev-collector:9999', -1)

        @app.route('/abc', methods=['GET'])
        def builtin_hello():
            return '''hello'''

        self.app = app
        self.client = app.test_client()
        return super().setUp()

    def test_request_example(self):
        response = self.client.get("/abc")
        assert "ut" in response.headers

if __name__ == '__main__':
    unittest.main()
