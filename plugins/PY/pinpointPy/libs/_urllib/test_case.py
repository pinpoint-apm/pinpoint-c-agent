from pinpointPy.libs._urllib import monkey_patch
import unittest
from pinpointPy import PinTransaction, Defines
from pinpointPy.tests import TestCase, create_http_bin_response, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_str_url(self):
        import urllib.request
        with urllib.request.urlopen('http://httpbin/anything') as response:
            body = response.read()
            httpbin = create_http_bin_response(body)
            self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)

    @PinTransaction("testcase", GenTestHeader())
    def test_req(self):
        import urllib.parse
        import urllib.request

        url = 'http://httpbin/anything'
        user_agent = 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)'
        values = {'name': 'Michael Foord',
                  'location': 'Northampton',
                  'language': 'Python'}
        headers = {'User-Agent': user_agent}

        data = urllib.parse.urlencode(values)
        data = data.encode('ascii')
        req = urllib.request.Request(url, data, headers)
        with urllib.request.urlopen(req) as response:
            # print(type(response))
            body = response.read()
            httpbin = create_http_bin_response(body)
            self.assertIn(Defines.PP_HEADER_PINPOINT_SPANID, httpbin.headers)


if __name__ == '__main__':
    unittest.main()
