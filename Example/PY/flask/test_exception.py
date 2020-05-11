#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.PinpointCommonPlugin import PinpointCommonPlugin


@PinpointCommonPlugin('', __name__)
def test_nameerror(arg):
    xxx(arg, 123)
    return "Call an undefined function."

# def test_syntaxerror(arg)
#     return "Test SyntaxError!"


@PinpointCommonPlugin('', __name__)
def test_ZeroDivisionError(arg):
    return 3/arg


class UserDefineException(Exception):

    def __init__(self, user):
        self.user = user
        self.value = "UserDefineException:" + self.user + " raise an exception!"

    def __str__(self):
        return self.value


@PinpointCommonPlugin('', __name__)
def test_userexception(arg):
    if arg == "Evy":
        raise UserDefineException(arg)
    else:
        return arg


@PinpointCommonPlugin('', __name__)
def test_exception_in_recursion(n):
    if n > 0:
        return n/test_exception_in_recursion(n-1)
    else:
        return 0
