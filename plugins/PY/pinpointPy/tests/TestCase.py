import unittest
import logging
import json
from pinpointPy import GenPinHeader, PinHeader, use_thread_local_context, set_agent


class GenTestHeader(GenPinHeader):
    def GetHeader(self, *args, **kwargs) -> PinHeader:
        _header = PinHeader()
        _header.Url = "/test"
        _header.Host = "127.0.0.1"
        _header.RemoteAddr = "127.0.0.1"
        _header.ParentType = "1700"
        _header.ParentName = "parent"
        _header.ParentHost = "127.0.0.1"
        _header.ParentTid = "abc^452568^23"
        return _header


class TestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        use_thread_local_context()
        set_agent("cd.dev.test.py", "cd.dev.test.py",
                  'tcp:localhost:9999', -1, log_level=logging.DEBUG)


class AsyncTestCase(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        # use_thread_local_context()
        set_agent("cd.dev.test.py", "cd.dev.test.py",
                  'tcp:localhost:9999', -1, log_level=logging.DEBUG)


class HttpBinResponse:
    def __init__(self, args, data, files, form, headers, json, method, origin, url) -> None:
        self.args = args
        self.data = data
        self.files = files
        self.form = form
        self.headers = headers
        self.json = json
        self.origin = origin
        self.url = url


def create_http_bin_response(data: str) -> HttpBinResponse:
    _dict = json.loads(data)
    response = HttpBinResponse(**_dict)
    return response
