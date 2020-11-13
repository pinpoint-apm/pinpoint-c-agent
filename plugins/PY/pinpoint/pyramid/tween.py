#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 11/12/20

from .PyRequestPlugin import PyRequestPlugin
from pinpoint.libs import monkey_patch_for_pinpoint
monkey_patch_for_pinpoint()

def pinpoint_tween(handler,registry):

    def tween(request):
        plugin = PyRequestPlugin("pinpoint_tween")
        plugin.onBefore(request)

        response = handler(request)
        plugin.onEnd(response)

        return response

    return tween
