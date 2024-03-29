#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import functools
from pinpointPy.CommonPlugin import PinpointCommonPlugin


def log(text):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kw):
            print('%s %s():' % (text, func.__name__))
            return func(*args, **kw)
        return wrapper
    return decorator

@log('execute')
@PinpointCommonPlugin(__name__+".func_in_decorator")
def func_in_decorator(x):
    return x * x