#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/11/20

from aiohttp import web
from .AioRequestPlugin import AioRequestPlugin

@web.middleware
async def PinPointMiddleWare(request, handler):
    plugin = AioRequestPlugin("")
    plugin.onBefore(request)
    response = await handler(request)
    plugin.onEnd(response)
    return response