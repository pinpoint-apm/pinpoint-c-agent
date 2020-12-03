#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/1/20

from oslo_log import log as logging
import six
import webob.dec
import webob.exc

from cinder.api.openstack import wsgi
from cinder import exception
from cinder.i18n import _, _LE, _LI
from cinder import utils
from cinder.wsgi import common as base_wsgi
import pinpointPy
from CinderRequestPlugin import CinderRequestPlugin

LOG = logging.getLogger(__name__)


class ProfileWrapper(base_wsgi.Middleware):
    
    @webob.dec.wsgify(RequestClass=wsgi.Request)
    def __call__(self, req):
        response = None
        try:
            plugin = CinderRequestPlugin("cinder filter")
            plugin.onBefore(self,req)
            response = req.get_response(self.application)
        except Exception as e: 
            response = None
            plugin.onException(e)
        finally:
            plugin.onEnd(response)
        return response