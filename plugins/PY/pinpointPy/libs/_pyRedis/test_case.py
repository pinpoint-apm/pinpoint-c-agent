from pinpointPy.libs._pyRedis import monkey_patch
import unittest
from pinpointPy import PinTransaction
from pinpointPy.tests import TestCase, GenTestHeader


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_redis(self):
        import redis
        r = redis.Redis(host='redis', port=6379, db=0)
        r.set('foo', 'bar')
        r.get('foo')
        pipe = r.pipeline()
        pipe.set('foo', 5)
        pipe.set('bar', 18.5)
        pipe.set('blee', "hello world!")
        pipe.execute()
        self.assertTrue(pipe._pinpoint_)


if __name__ == '__main__':
    unittest.main()
