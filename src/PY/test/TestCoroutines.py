from unittest import TestCase
import unittest
import _pinpointPy
import asyncio
import contextvars
import sys

pinpointId = contextvars.ContextVar('pinpoint_id')
pinpointId.set(0)
class TestInCoroutines(TestCase):
    @classmethod
    def setUpClass(cls):
        # _pinpointPy.enable_debug(None)
        _pinpointPy.set_agent(collector_host="unix:/tmp/unexist.sock")
    

    @unittest.skipIf(hex(sys.hexversion) <'0x30701f0', "python version must 3.7.1+")
    def testCoroFlow(self):
        def decorator_pinpoint(func):
            async def pinpoint_trace(*args, **kwargs):
                id = pinpointId.get()
                id = _pinpointPy.start_trace(id)
                pinpointId.set(id)
                func_name = func.__name__
                _pinpointPy.add_clue('name',func_name,id,0)
                _pinpointPy.add_clues('start','3434',id)
                _pinpointPy.set_context_key('sid','12345',id)
                value = _pinpointPy.get_context_key('sid',id)
                self.assertEqual(value,'12345')

                ret = await func(*args, **kwargs)
                _pinpointPy.add_clue('end','3434',id)
                id = _pinpointPy.end_trace(id)
            return pinpoint_trace

        @decorator_pinpoint
        async def unit2(name):
            await asyncio.sleep(0.1)
            await asyncio.sleep(0.1)
            await asyncio.sleep(0.1)

        @decorator_pinpoint
        async def unit(name):
            await asyncio.sleep(0.1)
            await asyncio.sleep(0.1)
            await asyncio.sleep(0.1)
            await unit2(name)

        async def main():
            task1 = asyncio.create_task(unit('a'))
            task2 = asyncio.create_task(unit('b'))
            task3 = asyncio.create_task(unit('c'))
            task4 = asyncio.create_task(unit('e'))
            task5 = asyncio.create_task(unit('f'))
            await task1
            await task2
            await task3
            await task4
            await task5

        asyncio.run(main())



if __name__ == '__main__':
    unittest.main()
    