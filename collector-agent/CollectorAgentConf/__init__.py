#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 12/9/20
from .CollectorAgentConf import CollectorAgentConf
from Common.Config import CAConfig
config = CollectorAgentConf(CAConfig)
__all__=['config']