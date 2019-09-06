#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 9/4/19
import json
import time

from CollectorAgent import CollectorAgentConf
from CollectorAgent.Type import PHP
from Common import TCLogger
from PinpointAgent.PinpointAgent import PinpointAgent
from CollectorAgent.ThriftAgentImplement import ThriftAgentImplement


class AppManagement(object):
    def __init__(self,collector_conf,service_type=PHP):
        assert isinstance(collector_conf,CollectorAgentConf)
        self.collector_conf = collector_conf
        self.default_appid = self.collector_conf.AgentID
        self.default_appname = self.collector_conf.ApplicationName
        self.app_map = {}
        self.default_app = ThriftAgentImplement(self.collector_conf,self.default_appid, self.default_appname,service_type)
        self.default_app.start()
        self.app_map[self.default_appid] = self.default_app
        self.startTimestamp = int(time.time()*1000)

    def find_app(self, app_id, app_name,service_type):
        if app_id in self.app_map:
            app = self.app_map[app_id]
            ## check app_name
            if app.app_name != app_name:
                TCLogger.warning(" app_name can't change when using ")
                app = self.default_app
            ## check service_type

        else:
            if  service_type == PHP:
                app = ThriftAgentImplement(app_id, app_name)
                app.start()
                self.app_map[app_id] = app
            else:
                raise NotImplementedError("service_type:%d not support",service_type)

        return app

    def stop_all(self):
        for app_id,instance in self.app_map:
            assert(isinstance(instance,PinpointAgent))
            TCLogger.info("application is stoping",app_id)
            instance.stop()

    def handle_front_agent_data(self,client,type,body):
        content = body.decode('utf-8')
        stack = json.loads(content)
        try:
            for key in ['appid','appname','FT']:
                assert key in stack, 'request must include %s'%(key)
            appid = stack['appid']
            appname = stack['appname']
            ft = stack['FT']
            app = self.find_app(appid,appname,ft)
            app.sendSpan(stack)

        except AssertionError as e:
            TCLogger.error("front request dropped,as %s. body:[%s]",e,content)
        except Exception as e:
            TCLogger.error("front request dropped,as %s. body:[%s]",e,content)

    def tell_whoami(self):
        return {
            "time": str(self.startTimestamp),
            "id": self.default_appid,
            "name": self.default_appname
        }
