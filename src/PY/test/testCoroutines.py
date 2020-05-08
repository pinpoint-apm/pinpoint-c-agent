from unittest import TestCase
import unittest
import pinpointPy
import asyncio
import contextvars

class TestInCoroutines(TestCase):
    @classmethod
    def setUpClass(cls):
        pass
        pinpointPy.set_agent(collector_host="unix:/tmp/unexist.sock",trace_limit=-1,enable_coroutines=True)
        pinpointPy.enable_debug(None)

    def testCoroFlow(self):
        

        # def test_syn(name):

            

        # def fetch_var(name):
        #     client_addr_var = contextvars.ContextVar('client_addr')
        #     client_addr = client_addr_var.get()
        #     assert client_addr == name

        

        # client_addr_var = contextvars.ContextVar('client_addr')

        # async def unit(name):
        #     await asyncio.sleep(0.1)
        #     client_addr_var.set(name)
        #     print(name)
        #     await asyncio.sleep(0.1)
        #     print(name)
        #     await asyncio.sleep(0.1)
        #     print(name)
        #     client_addr = client_addr_var.get()
        #     assert client_addr == name

        async def unit(name):
            print(name)
            pinpointPy.start_trace()
            await asyncio.sleep(0.1)
            pinpointPy.end_trace()
            print(name)
            pinpointPy.start_trace()
            await asyncio.sleep(0.1)
            print(name)
            await asyncio.sleep(0.1)
            print(name)
            pinpointPy.end_trace()

        async def main():
            task1 = asyncio.create_task(unit('a'))
            task2 = asyncio.create_task(unit('b'))
            await task1
            await task2

        asyncio.run(main())



if __name__ == '__main__':
    unittest.main()
    