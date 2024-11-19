
from flask import Flask, jsonify

from celery import Celery
import time
from pinpointPy.Flask.PinPointMiddleWare import PinPointMiddleWare
from pinpointPy import set_agent, monkey_patch_for_pinpoint, use_thread_local_context
from pinpointPy.libs import CeleryWorkerPlugin, CeleryCallerPlugin
from pinpointPy.CommonPlugin import PinpointCommonPlugin
import logging
use_thread_local_context()
monkey_patch_for_pinpoint()
set_agent("cd.dev.test.celery", "cd.dev.test.py",
          'tcp:dev-collector:10000', -1, 0, logging.DEBUG)

app = Flask(__name__)
app.wsgi_app = PinPointMiddleWare(app, app.wsgi_app)

# Celery configuration
app.config['CELERY_BROKER_URL'] = 'redis://redis:6379/0'
app.config['CELERY_RESULT_BACKEND'] = 'redis://redis:6379/1'


# Initialize Celery
celery = Celery(app.name, broker=app.config['CELERY_BROKER_URL'])
celery.conf.update(app.config)


@PinpointCommonPlugin()
def worker_test():
    time.sleep(1)


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def func_on_arg():
    print("hi func_on_arg")


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def func_on_arg1(a):
    return a


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def func_on_any(**vars):
    return vars


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def func_on_a_any(a, **vars):
    print(f'{a}')
    return vars


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def func_on_a_default_any(a=10, **vars):
    print(f'{a}')
    return vars


@CeleryCallerPlugin()
@celery.task()
@CeleryWorkerPlugin()
@PinpointCommonPlugin()
def add_(a, b):
    worker_test()
    return a + b


@app.route('/', methods=['GET'])
def longtask():
    return '''<!DOCTYPE html>
<html>
<head>
<title>This is test page for pinpoint python agent</title>
</head>
<body>
<h1>This is test page for pinpoint python agent</h1>
</body>
</html>'''


@app.route('/test_apply_async', methods=['GET'])
def test_apply_async():
    task = add_.apply_async(args=[3, 4], kwargs={})
    print(task.get())

    task = add_.apply_async(args=[2, 9])
    print(task.get())

    task = func_on_arg.apply_async()
    print(task.get())

    task = func_on_arg1.apply_async(args=[34])
    print(task.get())

    task = func_on_any.apply_async(kwargs={'v': "sdd", 'v2': 23})
    print(task.get())

    task = func_on_a_any.apply_async(args=[3], kwargs={'v2': 23})
    print(task.get())

    task = func_on_a_default_any.apply_async(kwargs={'v2': 23})
    print(task.get())

    return jsonify({'ret': task.get()})


@app.route('/test_delay', methods=['GET'])
def test_delay():
    task = add_.apply_async(args=[3, 4], kwargs={})
    print(task.get())

    task = add_.delay(2, 9)
    print(task.get())

    task = func_on_arg.delay()
    print(task.get())

    task = func_on_arg1.delay(34)
    print(task.get())

    task = func_on_any.delay(v="sdd", v2=23)
    print(task.get())

    task = func_on_a_any.delay(3, v2=23)
    print(task.get())

    task = func_on_a_default_any.delay(v2=23)
    print(task.get())

    return jsonify({'ret': task.get()})


if __name__ == '__main__':
    app.run(port=5000, debug=True)
