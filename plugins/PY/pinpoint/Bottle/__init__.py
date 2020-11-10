#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/9/20

from ..plugins.common import *
from .BottlePlugin import BottlePlugin
try:
    from bottle import Bottle

    Interceptors = [
        Interceptor(Bottle,'wsgi', BottlePlugin)
    ]

    for interceptor in Interceptors:
        interceptor.enable()
    pass
except ImportError as e:
    raise e


# from .BottleMiddleWare import *


