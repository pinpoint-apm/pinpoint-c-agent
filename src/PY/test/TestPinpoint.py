from unittest import TestCase
import unittest
import pinpointPy
import sys


def output(msg):
    print(msg)

class TestAgent(TestCase):
     
    def setUp(self):
        # self.assertTrue(pinpointPy.enable_debug(output))
        # self.assertTrue(pinpointPy.enable_debug(None))
        self.assertTrue(pinpointPy.set_agent(collector_host='unix:/tmp/collector-agent.sock'))

    def test_trace_life(self):
        pinpointPy.start_trace()
        pinpointPy.start_trace()
        pinpointPy.start_trace()
        pinpointPy.start_trace()
        pinpointPy.add_clue("key","value")
        pinpointPy.add_clue("key","value3")

        pinpointPy.add_clues("key","values")
        pinpointPy.add_clues("key","values")
        pinpointPy.add_clues("key","values")

        pinpointPy.end_trace()
        pinpointPy.end_trace()
        pinpointPy.end_trace()
        pinpointPy.end_trace()

    def test_set_collector_host(self):
        self.assertTrue(pinpointPy.set_agent(collector_host='unix:/tmp/collector1.sock'))
        self.assertTrue(pinpointPy.set_agent(collector_host='Unix:/tmp/collector1.sock'))
        self.assertTrue(pinpointPy.set_agent(collector_host='TCP:dev-collector:11331'))
        self.assertTrue(pinpointPy.set_agent(collector_host='Tcp:dev-collector:11331'))

        try:
            pinpointPy.set_agent(collector_host='dev-collector:11331')
            self.assertFalse(1)
        except:
            pass
        try:
            self.assertFalse(pinpointPy.set_agent(collector_host='/tmp/collector1.sock'))
            self.assertFalse(1)
        except:
            pass
        
        self.assertTrue(pinpointPy.set_agent(collector_host='Tcp:dev-collector:11331',trace_limit=1000))


# pinpointPy.start_trace()
# pinpointPy.end_trace()

if __name__ == '__main__':
    unittest.main()