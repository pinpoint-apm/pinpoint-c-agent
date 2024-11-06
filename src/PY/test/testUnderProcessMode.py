from unittest import TestCase
import unittest
from multiprocessing import Process
import time
import _pinpointPy
import random
import platform


class TestUnderProcessMode(TestCase):

    def tearDown(self):
        _pinpointPy.force_flush_trace(4)

    def _test_api_flow(self):
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='tcp:127.0.0.1:9999'))

        while True:
            id = _pinpointPy.start_trace(0)
            _pinpointPy.set_context_key('sid', f'{id}', id)
            _pinpointPy.add_clue("key", "value3", id)
            _pinpointPy.add_clues("key", "value3", id)
            value = _pinpointPy.get_context_key('sid', id)
            self.assertEqual(value, f'{id}')
            _pinpointPy.mark_as_error("fghjk", "fghjkl", 234234, id)
            id = _pinpointPy.end_trace(id)
            _pinpointPy.force_flush_trace(id)
            _pinpointPy.drop_trace(id)

    @unittest.skipIf(platform.system() == "Darwin", "skip Darwin")
    @unittest.skipIf(platform.system() == "Windows", "skip Windows")
    def test_process(self):
        p1 = Process(target=self._test_api_flow)
        p1.start()
        p2 = Process(target=self._test_api_flow)
        p2.start()
        p3 = Process(target=self._test_api_flow)
        p3.start()
        time.sleep(3)
        p1.terminate()
        p2.terminate()
        p3.terminate()
        p1.join()
        p2.join()
        p3.join()


if __name__ == '__main__':
    unittest.main()
