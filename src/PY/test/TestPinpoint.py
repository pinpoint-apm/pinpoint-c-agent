from unittest import TestCase
import unittest
import _pinpointPy
import sys


def output(msg):
    print(msg)

class TestAgent(TestCase):
     
    def setUp(self):
        # self.assertTrue(_pinpointPy.enable_debug(output))
        # self.assertTrue(_pinpointPy.enable_debug(None))
        self.assertTrue(_pinpointPy.set_agent(collector_host='unix:/tmp/collector-agent.sock'))

    def test_trace_life(self):
        self.assertFalse(_pinpointPy.trace_has_root())
        _pinpointPy.start_trace()
        self.assertTrue(_pinpointPy.trace_has_root())
        _pinpointPy.start_trace()
        self.assertTrue(_pinpointPy.trace_has_root())
        _pinpointPy.start_trace()
        _pinpointPy.start_trace()
        # self.assertRaises(Exception,lambda: _pinpointPy.trace_has_root(1025))
        # self.assertRaises(Exception,lambda: _pinpointPy.trace_has_root(-1025))

        _pinpointPy.add_clue("key","value")
        _pinpointPy.add_clue("key","value3")
        _pinpointPy.set_context_key('sid','12345')
        value = _pinpointPy.get_context_key('sid')
        self.assertEqual(value,'12345')
        _pinpointPy.add_clues("key","values")
        _pinpointPy.add_clues("key","values")
        _pinpointPy.add_clues("key","values")

        _pinpointPy.end_trace()
        _pinpointPy.end_trace()
        _pinpointPy.end_trace()
        _pinpointPy.end_trace()
        _pinpointPy.force_flush_trace(10)
        self.assertFalse(_pinpointPy.trace_has_root())

    def test_set_collector_host(self):
        self.assertTrue(_pinpointPy.set_agent(collector_host='unix:/tmp/collector1.sock'))
        self.assertTrue(_pinpointPy.set_agent(collector_host='Unix:/tmp/collector1.sock'))
        self.assertTrue(_pinpointPy.set_agent(collector_host='TCP:dev-collector:11331'))
        self.assertTrue(_pinpointPy.set_agent(collector_host='Tcp:dev-collector:11331'))

        try:
            _pinpointPy.set_agent(collector_host='dev-collector:11331')
            self.assertFalse(1)
        except:
            pass
        try:
            self.assertFalse(_pinpointPy.set_agent(collector_host='/tmp/collector1.sock'))
            self.assertFalse(1)
        except:
            pass
        
        self.assertTrue(_pinpointPy.set_agent(collector_host='Tcp:dev-collector:11331',trace_limit=1000))


# _pinpointPy.start_trace()
# _pinpointPy.end_trace()

if __name__ == '__main__':
    unittest.main()