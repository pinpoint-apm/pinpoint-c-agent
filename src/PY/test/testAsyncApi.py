from os import wait
from unittest import TestCase
import unittest
import _pinpointPy
import random
import threading
import asyncio


def output(msg):
    print(msg)


class TestAsync(TestCase):
    def setUp(self):
        def debug_func(msg: str):
            print(msg)
        _pinpointPy.enable_debug(debug_func)
        self.assertTrue(_pinpointPy.set_agent(
            collector_host='unix:/tmp/collector-agent.sock'))

    def test_async_id(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)
        async_id = random.randint(1, 100000)
        a_id = _pinpointPy.start_trace(0)
        _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
        a_id = _pinpointPy.start_trace(a_id)
        a_id = _pinpointPy.end_trace(a_id)
        a_id = _pinpointPy.end_trace(a_id)

        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)

    def test_async_id_thread(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)

        def thread_main():
            a_id = _pinpointPy.start_trace(0)
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            a_id = _pinpointPy.start_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)

        thread = threading.Thread(target=thread_main,)
        thread.start()
        thread.join()
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)

    def test_async_id_thread_run_in_same_time(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)

        def thread_main():
            a_id = _pinpointPy.start_trace(0)
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            a_id = _pinpointPy.start_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)

        thread = threading.Thread(target=thread_main,)
        thread.start()
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        thread.join()

    def test_async_id_thread_run_after(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)

        def thread_main():
            a_id = _pinpointPy.start_trace(0)
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            a_id = _pinpointPy.start_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)

        thread = threading.Thread(target=thread_main,)
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        thread.start()
        thread.join()

    def test_asyncio_run(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)

        async def thread_main():
            a_id = _pinpointPy.start_trace(0)
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            a_id = _pinpointPy.start_trace(a_id)
            await asyncio.sleep(1)
            a_id = _pinpointPy.end_trace(a_id)
            a_id = _pinpointPy.end_trace(a_id)
        asyncio.run(thread_main())
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)

    def test_asyncio_main(self):

        async def thread_main():
            id = 0
            id = _pinpointPy.start_trace(id)
            id = _pinpointPy.start_trace(id)
            async_id = random.randint(1, 100000)
            seq_id = _pinpointPy.get_sequence_id(id)
            self.assertGreaterEqual(seq_id, 0)

            async def task_func():
                a_id = _pinpointPy.start_trace(0)
                _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
                a_id = _pinpointPy.start_trace(a_id)
                await asyncio.sleep(1)
                a_id = _pinpointPy.end_trace(a_id)
                a_id = _pinpointPy.end_trace(a_id)
            task = asyncio.create_task(task_func())
            await task
            id = _pinpointPy.end_trace(id)
            id = _pinpointPy.end_trace(id)

        asyncio.run(thread_main())

    def test_asyncio_task(self):

        async def thread_main():
            id = 0
            id = _pinpointPy.start_trace(id)
            id = _pinpointPy.start_trace(id)
            async_id = random.randint(1, 100000)
            seq_id = _pinpointPy.get_sequence_id(id)
            self.assertGreaterEqual(seq_id, 0)

            async def task_func():
                a_id = _pinpointPy.start_trace(0)
                _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
                a_id = _pinpointPy.start_trace(a_id)
                await asyncio.sleep(1)
                a_id = _pinpointPy.end_trace(a_id)
                a_id = _pinpointPy.end_trace(a_id)
            task = asyncio.create_task(task_func())
            id = _pinpointPy.end_trace(id)
            id = _pinpointPy.end_trace(id)
            await task

        asyncio.run(thread_main())

    def test_asyncio_task_infinity(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, 0)

        def thread_main():
            a_id = _pinpointPy.start_trace(0)
            root = a_id
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            size = 20241025
            while size > 0 and a_id != -1:
                a_id = _pinpointPy.start_trace(a_id)
                a_id = _pinpointPy.end_trace(a_id)
                size -= 1
            a_id = _pinpointPy.end_trace(root)

        thread = threading.Thread(target=thread_main,)
        id = _pinpointPy.end_trace(id)
        id = _pinpointPy.end_trace(id)
        thread.start()
        thread.join()

    def test_asyncio_task_infinity_main(self):
        id = 0
        id = _pinpointPy.start_trace(id)
        id = _pinpointPy.start_trace(id)
        async_id = random.randint(1, 100000)
        seq_id = _pinpointPy.get_sequence_id(id)
        self.assertGreaterEqual(seq_id, -1)

        def thread_main():
            a_id = _pinpointPy.start_trace(0)
            _pinpointPy.set_async_ctx(a_id, async_id, seq_id)
            a_id = _pinpointPy.end_trace(a_id)

        thread = threading.Thread(target=thread_main,)
        id = _pinpointPy.end_trace(id)

        size = 20241025
        root = id
        while size > 0 and id != -1:
            id = _pinpointPy.start_trace(id)
            id = _pinpointPy.end_trace(id)
            size -= 1

        _pinpointPy.end_trace(root)
        thread.start()
        thread.join()


if __name__ == '__main__':
    unittest.main()
