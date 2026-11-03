from pinpointPy.tests import TestCase
import unittest
from pinpointPy import PinTransaction, Defines, GenPinHeader, PinHeader
from pinpointPy.tests import GenTestHeader, create_http_bin_response
from pinpointPy.libs._requests import monkey_patch


class TestHeader(GenPinHeader):
    def GetHeader(self, *args, **kwargs) -> PinHeader:
        _header = PinHeader()
        _header.Url = "/test"
        _header.Host = "127.0.0.1"
        _header.RemoteAddr = "127.0.0.1"
        _header.ParentType = "1700"
        _header.ParentName = "parent"
        _header.ParentHost = "127.0.0.1"
        return _header


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_post(self):
        import requests
        url = 'http://httpbin/anything'
        proto = {'a': 'proto'}
        body = requests.post(url, json=proto)
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

        body = requests.post(url, json=proto)
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

        body = requests.post(url, data={
            'a': 12,
            'b': 'sdf'
        })
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

    @PinTransaction("testcase", GenTestHeader())
    def test_get(self):
        import requests
        body = requests.get('http://httpbin/anything')
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

    @PinTransaction("testcase", GenTestHeader())
    def test_patch(self):
        import requests
        body = requests.patch('http://httpbin/anything')
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

    @PinTransaction("testcase", GenTestHeader())
    def test_request(self):
        import requests
        body = requests.request("POST", 'http://httpbin/anything', data='abc')
        httpbin = create_http_bin_response(body.text)
        self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

    def test_request_no_transaction(self):
        import requests
        from threading import Thread
        from multiprocessing import Process

        from pinpointPy.TraceContext import get_trace_context

        def threaded_function():
            sample, _ = get_trace_context().get_parent_id()
            self.assertFalse(sample)
            body = requests.request(
                "POST", 'http://httpbin/anything/threaded_function', data='abc')
            httpbin = create_http_bin_response(body.text)
            print(httpbin.headers)

        def process_function():
            sample, traceId = get_trace_context().get_parent_id()
            # self.assertFalse(sample)
            print(f'process_function:{sample},{traceId}')
            body = requests.request(
                "POST", 'http://httpbin/anything/threaded_function', data='abc')
            httpbin = create_http_bin_response(body.text)

            # body = requests.request(
            #     "POST", 'xxx', data='abc')
            # httpbin = create_http_bin_response(body.text)

            sample, traceId = get_trace_context().get_parent_id()
            # self.assertFalse(sample)
            print(f'process_function:{sample},{traceId}')
            print(httpbin.headers)

        @PinTransaction("testcase", TestHeader())
        def test_body():

            body = requests.request(
                "POST", 'http://httpbin/anything', data={'abc': 'sdf'})
            httpbin = create_http_bin_response(body.text)

            body = requests.request(
                "POST", 'http://httpbin/anything', data='abc')
            httpbin = create_http_bin_response(body.text)
            print(httpbin.headers)
            thread = Thread(target=threaded_function)
            thread.start()
            thread.join()
            sample, _ = get_trace_context().get_parent_id()
            self.assertTrue(sample)
            p = Process(target=process_function)
            p.start()
            p.join()

        test_body()

        sample, parentId = get_trace_context().get_parent_id()
        print(f'{sample},{parentId}')
        requests.request(
            "POST", 'http://httpbin/anything', data='abc')
        sample, parentId = get_trace_context().get_parent_id()
        requests.request(
            "POST", 'http://httpbin/anything', data='abc')
        sample, parentId = get_trace_context().get_parent_id()
        print(f'{sample}, {parentId}')
        self.assertFalse(sample)


if __name__ == '__main__':
    unittest.main()
