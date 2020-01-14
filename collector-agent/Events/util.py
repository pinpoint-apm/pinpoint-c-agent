#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 1/14/20

import gevent
def try_to_recycle_the_event_hub():
    gevent.reinit()
    # loop = gevent.config.loop(default=True)
    # loop.destory()
    # gevent.joinall()