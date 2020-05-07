from unittest import TestCase
import unittest
from threading import Thread
import time
import pinpointPy


class TestUnderThreadMode(TestCase):

    def setUp(self) -> None:
        self.thread_running = True

    def tearDown(self):
        pinpointPy.force_flush_trace(4)

    def _test_api_flow(self):
        self.assertTrue(pinpointPy.set_collector(collector_host='unix:/tmp/unexist.sock'))
        # self.assertTrue(pinpointPy.enable_debug(None))

        while self.thread_running:
            self.assertEqual(pinpointPy.start_trace(),1)
            pinpointPy.set_special_key('sid','12345678')
            pinpointPy.add_clue("key","value3")
            pinpointPy.add_clues("key","value3")
            value = pinpointPy.get_special_key('sid')
            self.assertEqual(value,'12345678')
            self.assertEqual(pinpointPy.end_trace(),0)
            value = pinpointPy.get_special_key('sid')
            self.assertFalse(value)


    def test_thead_safe(self):
        thread1 = Thread(target=self._test_api_flow)
        thread2 = Thread(target=self._test_api_flow)
        thread3 = Thread(target=self._test_api_flow)
        thread1.start()
        thread2.start()
        thread3.start()
        time.sleep(3)
        self.thread_running = False
        thread1.join()
        thread2.join()
        thread3.join()

if __name__ == '__main__':
    unittest.main()