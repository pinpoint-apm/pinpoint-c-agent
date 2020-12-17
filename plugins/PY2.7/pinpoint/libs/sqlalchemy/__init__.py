#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Created by eeliu at 7/31/20



def monkey_patch():
    try:
        from .sqlalchemyPlugin import before_cursor_execute,after_cursor_execute
    except ImportError:
        pass

__all__=['monkey_patch']