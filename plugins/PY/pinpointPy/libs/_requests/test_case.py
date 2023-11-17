from pinpointPy.tests import TestCase
import unittest
from pinpointPy import PinTransaction, Defines
from pinpointPy.tests import GenTestHeader, create_http_bin_response
from pinpointPy.libs._requests import monkey_patch


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


if __name__ == '__main__':
    unittest.main()
