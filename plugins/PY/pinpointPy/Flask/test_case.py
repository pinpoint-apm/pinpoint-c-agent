import unittest
from flask import Flask
from pinpointPy.Flask.PinPointMiddleWare import PinPointMiddleWare
from pinpointPy import PinTransaction, GenPinHeader, PinHeader, use_thread_local_context, set_agent
import logging


class Test_case(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        use_thread_local_context()
        set_agent("cd.dev.test.py", "cd.dev.test.py",
                  'tcp:dev-collector:9999', -1, logging.DEBUG)
        app = Flask("abc")
        app.wsgi_app = PinPointMiddleWare(app, app.wsgi_app)
        app.config.update({
            "TESTING": True,
        })

        @app.route('/abc', methods=['GET'])
        def builtin_hello():
            return '''hello'''

        cls.app = app
        cls.client = app.test_client()
        return super().setUpClass()

    def test_request_example(self):
        response = self.client.get("/abc")
        assert "ut" in response.headers


if __name__ == '__main__':
    unittest.main()
