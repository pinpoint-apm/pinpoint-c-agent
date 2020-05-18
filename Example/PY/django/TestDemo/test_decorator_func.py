#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import functools
from pinpoint.plugins.PinpointCommonPlugin import PinpointCommonPlugin


def log(text):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kw):
            print('%s %s():' % (text, func.__name__))
            return func(*args, **kw)
        return wrapper
    return decorator

@log('execute')
@PinpointCommonPlugin('', __name__)
def func_in_decorator(x):
    return x * x