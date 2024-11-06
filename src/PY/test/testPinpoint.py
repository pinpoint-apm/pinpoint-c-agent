from unittest import TestCase
import unittest
import _pinpointPy
import sys


def output(msg):
    print(msg)


class TestAgent(TestCase):

    def setUp(self):
        def debug_func(msg: str):
            print(msg)
        _pinpointPy.enable_debug(debug_func)
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='unix:/tmp/collector-agent.sock'))

    def test_invalid_input(self):
        id = _pinpointPy.start_trace(0)
        _pinpointPy.end_trace(id)

    def test_encode(self):
        id = _pinpointPy.start_trace(0)
        _pinpointPy.add_clues("CN", "测试中文编码", id)
        _pinpointPy.add_clues("KR", "한국어 인코딩 테스트 ", id)
        _pinpointPy.add_clues("JP", "日本語エンコーディングをテストする ", id)
        _pinpointPy.end_trace(id)

    def test_trace_life(self):
        self.assertFalse(_pinpointPy.trace_has_root(-1))
        self.assertFalse(_pinpointPy.is_root_trace(-1))
        id = _pinpointPy.start_trace(0)
        self.assertTrue(_pinpointPy.trace_has_root(id))
        self.assertTrue(_pinpointPy.is_root_trace(id))
        id = _pinpointPy.start_trace(id)
        self.assertTrue(_pinpointPy.trace_has_root(id))
        self.assertFalse(_pinpointPy.is_root_trace(id))
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)

        _pinpointPy.add_clue("key", "value", id)
        _pinpointPy.add_clue("key", "value3", id)
        _pinpointPy.set_context_key('sid', '12345', id)
        value = _pinpointPy.get_context_key('sid', id)
        self.assertEqual(value, '12345')
        _pinpointPy.add_clues("key", "values", id)
        _pinpointPy.add_clues("key", "values", id)
        _pinpointPy.add_clues("key", "values", id)

        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        _pinpointPy.force_flush_trace(10, id)
        self.assertFalse(_pinpointPy.trace_has_root(id))
        self.assertFalse(_pinpointPy.is_root_trace(id))

    def test_set_collector_host(self):
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='unix:/tmp/collector1.sock'))
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='Unix:/tmp/collector1.sock'))
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='TCP:dev-collector:11331'))
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='Tcp:dev-collector:11331'))

        try:
            _pinpointPy.set_agent(collector_host='dev-collector:11331')
            self.assertFalse(1)
        except:
            pass
        try:
            self.assertFalse(_pinpointPy.set_agent(
                collector_host='/tmp/collector1.sock'))
            self.assertFalse(1)
        except:
            pass

        self.assertTrue(_pinpointPy.set_agent(
            collector_host='Tcp:dev-collector:11331', trace_limit=1000))

    def test_set_collector_host(self):
        self.assertGreater(_pinpointPy.start_time(), 0,
                           "_pinpointPy.start_time() in wrong format")


if __name__ == '__main__':
    unittest.main()
