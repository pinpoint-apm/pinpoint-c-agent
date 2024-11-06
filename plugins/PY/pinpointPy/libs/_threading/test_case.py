from pinpointPy.tests import TestCase
import unittest
from pinpointPy import PinTransaction, GenPinHeader, PinHeader, use_thread_local_context, enable_experiment_plugins
from pinpointPy.tests import GenTestHeader
from pinpointPy.libs._threading import monkey_patch
from pinpointPy.libs._process import monkey_patch as process_patch
from pinpointPy.CommonPlugin import PinpointCommonPlugin
import threading
from multiprocessing import Process
import time


class TestHeader(GenPinHeader):
    def GetHeader(self, *args, **kwargs) -> PinHeader:
        _header = PinHeader()
        _header.Url = "/test"
        _header.Host = "127.0.0.1"
        _header.RemoteAddr = "127.0.0.1"
        _header.ParentType = "1700"
        _header.ParentName = "parent"
        _header.ParentHost = "127.0.0.1"
        return _header


@PinpointCommonPlugin("task")
def task():
    time.sleep(0.5)


@PinpointCommonPlugin("thread_task_01")
def thread_task_01():
    pass


@PinpointCommonPlugin("thread_task")
def thread_task():
    time.sleep(0.5)
    thread_task_01()


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        use_thread_local_context()
        monkey_patch()
        process_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_process(self):
        @PinpointCommonPlugin("process_task")
        def process_task():
            self.assert_in_pinpoint_context()
            time.sleep(0.5)
        process = Process(target=process_task, daemon=True)
        process.start()
        process.join()

    @PinTransaction("testcase", GenTestHeader())
    def test_threading(self):

        task()
        thread = threading.Thread(target=thread_task,)
        thread.start()
        thread.join()

        thread_task_01()

        empty_thread = threading.Thread()
        empty_thread.start()
        empty_thread.join()


if __name__ == '__main__':
    unittest.main()
