from pinpointPy.tests import TestCase
import unittest
from pinpointPy import PinTransaction, GenPinHeader, PinHeader, use_asyncio_local_context, enable_experiment_plugins
from pinpointPy.tests import GenTestHeader
from pinpointPy.libs._asyncio import monkey_patch
from pinpointPy.CommonPlugin import AsyncCommonPlugin, PinpointCommonPlugin
import asyncio


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


class Test_Case(TestCase):

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        use_asyncio_local_context()
        enable_experiment_plugins()
        monkey_patch()

    @PinTransaction("testcase", GenTestHeader())
    def test_asyncio(self):

        @PinpointCommonPlugin("thread_task_01")
        def thread_task_01():
            self.assert_in_pinpoint_context()

        @AsyncCommonPlugin("async_coroutine_task")
        async def async_coroutine_task():
            await asyncio.sleep(0.5)
            thread_task_01()
            self.assert_in_pinpoint_context()

        asyncio.run(async_coroutine_task())

        async def call_task_func() -> None:
            new_task = asyncio.create_task(
                async_coroutine_task())
            await new_task
            self.assert_in_pinpoint_context()

        asyncio.run(call_task_func())


if __name__ == '__main__':
    unittest.main()
