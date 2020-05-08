from unittest import TestCase
import unittest
import pinpointPy
import sys


def output(msg):
    print(msg)

class TestAgent(TestCase):
     
    def setUp(self) -> None:
        self.assertTrue(pinpointPy.set_agent(collector_host='unix:/tmp/collector-agent.sock'))
        self.assertTrue(pinpointPy.enable_debug(output))

    def test_trace_life(self):
        self.assertEqual(pinpointPy.start_trace(),1)
        self.assertEqual(pinpointPy.start_trace(),2)
        self.assertEqual(pinpointPy.start_trace(),3)
        self.assertEqual(pinpointPy.start_trace(),4)
        pinpointPy.add_clue("key","value")
        pinpointPy.add_clue("key","value3")

        pinpointPy.add_clues("key","values")
        pinpointPy.add_clues("key","values")
        pinpointPy.add_clues("key","values")

        self.assertEqual(pinpointPy.end_trace(),3)
        self.assertEqual(pinpointPy.end_trace(),2)
        self.assertEqual(pinpointPy.end_trace(),1)
        self.assertEqual(pinpointPy.end_trace(),0)

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