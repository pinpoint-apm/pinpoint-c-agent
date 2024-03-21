from unittest import TestCase
import unittest
import pinpointPy
import logging

from pinpointPy.tests import TestCase, GenTestHeader
from pinpointPy import PinTransaction
from pinpointPy.CommonPlugin import PinpointCommonPlugin
from pinpointPy import use_thread_local_context


def output(msg):
    print(msg)


class TestAgent(TestCase):
    @classmethod
    def setUpClass(cls):
        # _pinpointPy.enable_debug(None)
        pinpointPy.set_agent(
            "test", "test", "tcp:0.0.0.0:5623", -1, 0, logging.DEBUG)
        use_thread_local_context()

    @PinTransaction("testcase", GenTestHeader())
    def test_569(self):
        @PinpointCommonPlugin('foo')
        def foo():
            print('foo')
        foo()


if __name__ == '__main__':
    unittest.main()
