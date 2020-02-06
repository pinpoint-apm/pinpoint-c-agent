import unittest
from Events.GTimer import GTimer
from gevent import time
class TestStringMethods(unittest.TestCase):
    def test_once_timer(self):
        self.i = 0
        def add_1(args):
            self.i+=1
        gt = GTimer()
        gt.start(add_1, 0.1)
        time.sleep(0.2)
        self.assertEqual(self.i,1)

    def test_persist_timer(self):
        self.i = 0

        def add_1(args):
            self.i += 1

        gt = GTimer(only_once = False)
        gt.start(add_1, 0.1)
        gt.start(add_1, 0.1)
        gt.start(add_1, 0.1)
        gt.start(add_1, 0.1)
        time.sleep(0.3)
        self.assertTrue(self.i >= 3 and self.i <=4)
        gt.stop()
        time.sleep(0.3)
        self.assertTrue(self.i >= 3 and self.i <= 4)
        gt.start(add_1, 0.1)
        time.sleep(0.3)
        self.assertTrue(self.i >= 6 and self.i <= 8)


