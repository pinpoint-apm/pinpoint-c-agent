from celery import Celery
import time
app = Celery('tasks', broker='redis://redis:6379',
             backend='redis://redis:6379')


def debug_(func):
    def decorate(*args, **kwargs):
        print(kwargs)
        # del kwargs['name']
        # del kwargs['instance_id']
        return func(*args, **kwargs)
    return decorate


@app.task
def add(x, y,):
    time.sleep(10)
    return x + y
