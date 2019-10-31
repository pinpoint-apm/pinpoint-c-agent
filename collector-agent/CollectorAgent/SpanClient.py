#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 10/31/19
from CollectorAgent.GrpcClient import GrpcClient


class SpanClient(GrpcClient):
    def __init__(self,hostname,ip,ports,pid,address,meta=None,maxPending=-1,ping_timeout=10):
        super().__init__(address, meta, maxPending)