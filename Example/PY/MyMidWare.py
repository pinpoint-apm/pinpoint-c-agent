#!/usr/bin/env python
# -*- coding: UTF-8 -*-

from plugins.BaseFlaskPlugins import BaseFlaskPlugins
# import pinpointPy
class MyMidWare():

    def __init__(self, app,wsgi_app):
        self.app = app
        self.wsgi_app = wsgi_app

    @BaseFlaskPlugins("Flask Web App", __name__)
    def __call__(self, environ, start_response):
        return self.wsgi_app(environ, start_response)

