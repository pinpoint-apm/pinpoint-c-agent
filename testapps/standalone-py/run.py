
from pinpointPy import PinTransaction
from pinpointPy.CommonPlugin import PinpointCommonPlugin, AsyncCommonPlugin
from pinpointPy import GenPinHeader, PinHeader, use_asyncio_local_context, set_agent, enable_experiment_plugins
import logging
import time
import asyncio
import threading
from multiprocessing import Process


use_asyncio_local_context()
enable_experiment_plugins()
set_agent("cd.dev.test.py", "cd.dev.test.py",
          'tcp:dev-collector:10000', -1, log_level=logging.DEBUG)


class FakeHeader(GenPinHeader):
    def GetHeader(self, *args, **kwargs) -> PinHeader:
        _header = PinHeader()
        _header.Url = "/test"
        _header.Host = "127.0.0.1"
        return _header


@PinpointCommonPlugin("task")
def task():
    time.sleep(0.5)


@PinpointCommonPlugin("thread_task_01")
def thread_task_01():
    # time.sleep(0.5)
    pass


@PinpointCommonPlugin("thread_task")
def thread_task():
    time.sleep(0.5)
    thread_task_01()


@PinpointCommonPlugin("process_task")
def process_task():
    time.sleep(0.5)


@AsyncCommonPlugin("async_coroutine_task")
async def async_coroutine_task():
    await asyncio.sleep(0.5)
    thread_task_01()


@PinTransaction("main", FakeHeader())
def run():
    task()
    thread = threading.Thread(target=thread_task,)
    thread.start()
    thread.join()
    process = Process(target=process_task, daemon=True)
    process.start()
    process.join()

    thread_task_01()

    empty_thread = threading.Thread()
    empty_thread.start()
    empty_thread.join()

    print(f'asyncio.run: {asyncio.run}')

    asyncio.run(async_coroutine_task())

    async def call_task_func() -> None:
        new_task = asyncio.create_task(
            async_coroutine_task())
        await new_task

    asyncio.run(call_task_func())


if __name__ == '__main__':
    run()
