#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 9/4/19
import json
import time

from CollectorAgent.CollectorAgentConf import CollectorAgentConf
from Common.Logger import TCLogger

from PinpointAgent.PinpointAgent import PinpointAgent
from PinpointAgent.Type import PHP


class AppManagement(object):
    def __init__(self,collector_conf,service_type=PHP):
        assert isinstance(collector_conf,CollectorAgentConf)
        self.collector_conf = collector_conf
        self.default_appid = self.collector_conf.AgentID
        self.default_appname = self.collector_conf.ApplicationName
        self.app_map = {}
        self.default_app = None
        self.startTimestamp = int(time.time()*1000)
        self.create_default_implement(service_type)

    def create_default_implement(self,service_type):

        self.default_app = self.collector_conf.collector_implement(self, self.collector_conf, self.default_appid, self.default_appname, service_type)

        self.default_app.start()
        self.app_map[self.default_appid] = self.default_app

    def find_app(self, app_id, app_name,service_type):
        if app_id in self.app_map:
            app = self.app_map[app_id]
            ## check app_name
            if app.app_name != app_name:
                TCLogger.warning(" app_name can't change when using ")
                app = self.default_app
            ## check service_type

        else:
            if service_type == PHP:
                TCLogger.info("collector-agent try to create a new application agent.[%s@%s]",app_id,app_name)
                app = self.collector_conf.collector_implement(self, self.collector_conf, app_id, app_name)
                app.start()
                self.app_map[app_id] = app
            else:
                raise NotImplementedError("service_type:%d not support",service_type)

        return app

    def stop_all(self):
        for app_id,instance in self.app_map.items():
            assert(isinstance(instance,PinpointAgent))
            TCLogger.info("application is stopping [%s]",app_id)
            instance.stop()

    def handle_front_agent_data(self,client,type,body):
        content = body.decode('utf-8')
        stack = json.loads(content)

        if 'appid' not in stack:
            appid = self.default_appid
        else:
            appid = stack['appid']

        if 'appname' not in stack:
            appname = self.default_appname
        else:
            appname = stack['appname']

        ft = stack['FT']
        app = self.find_app(appid,appname,ft)
        app.sendSpan(stack)

    def tell_whoami(self):
        return {
            "time": str(self.startTimestamp),
            "id": self.default_appid,
            "name": self.default_appname
        }
