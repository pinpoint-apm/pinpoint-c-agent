#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from pinpointPy.CommonPlugin import PinpointCommonPlugin

@PinpointCommonPlugin( __name__+".fib")
def fib(max):
    n, a, b = 0, 0, 1
    while n < max:
        yield b
        a, b = b, a + b
        n = n + 1
    return 'done'

# def fib(max):
#     n, a, b = 0, 0, 1
#     while True:
#         yield b
#         a, b = b, a + b
#         n = n + 1
#         if n > max:
#             raise ValueError("")
#     return 'done'
