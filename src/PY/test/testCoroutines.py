from unittest import TestCase
import unittest
import pinpointPy
import asyncio
import contextvars

class TestInCoroutines(TestCase):
    @classmethod
    def setUpClass(cls):
        # pinpointPy.enable_debug(None)
        pinpointPy.set_agent(collector_host="unix:/tmp/unexist.sock",enable_coroutines=True)

    def testCoroFlow(self):

        def decorator_pinpoint(func):
            async def pinpoint_trace(*args, **kwargs):
                start_size = pinpointPy.start_trace()
                func_name = func.__name__
                pinpointPy.add_clue('name',func_name)
                pinpointPy.add_clues('sdf','3434')
                ret = await func(*args, **kwargs)
                pinpointPy.add_clue('sdf','3434')
                end_size = pinpointPy.end_trace()
                if func_name == 'unit':
                    self.assertEqual(start_size,1)
                    self.assertEqual(end_size,0)
                elif func_name == 'unit2':
                    self.assertEqual(start_size,2)
                    self.assertEqual(end_size,1)
                return ret
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
    