# Copyright 2018 NAVER Corp.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from simulate_test import SimulateTest
from config import *
import subprocess
import sys
import requests
import time


class PHPSimulateTest(SimulateTest):
    def __init__(self, config_path):
        SimulateTest.__init__(self, config_path)

    def _build_agent(self):
        print "1, compile php agent"
        # 1, build
        print "pinpoint_agent_build_commands=%s" % self.conf["pinpoint_agent_build_commands"]
        print "pinpoint_agent_code_path=%s" % self.conf["pinpoint_agent_code_path"]
        p = subprocess.Popen(self.conf["pinpoint_agent_build_commands"],
                             cwd=self.conf["pinpoint_agent_code_path"],
                             shell=True)

        returncode = p.wait()
        if returncode != 0:
            print "Make agent fail."
            sys.exit(-1)

    def  render_all_conf(self):
        # 1, render config
        pinpoint_agent_conf_input_path = self.conf["pinpoint_agent_conf_path"]
        pinpoint_agent_conf_output_path =self._get_render_file_path(pinpoint_agent_conf_input_path)
        print "pinpoint_agent_conf_input_path=%s" % pinpoint_agent_conf_input_path
        print "pinpoint_agent_conf_output_path=%s" % pinpoint_agent_conf_output_path

        render_conf(pinpoint_agent_conf_input_path, pinpoint_agent_conf_output_path, self.conf)

        keymap = {key: value for key, value in self.conf.items()}
        keymap["pinpoint_agent_conf_path"] = pinpoint_agent_conf_output_path

        php_config_input_path = self.conf["php_config_path"]
        php_config_output_path = self._get_render_file_path(php_config_input_path)
        print "php_config_input_path=%s" % php_config_input_path
        print "php_config_output_path=%s" % php_config_output_path
        render_conf(php_config_input_path, php_config_output_path, keymap)

        php_fpm_config_input_path = self.conf["php_fpm_config_path"]
        php_fpm_config_output_path =self._get_render_file_path(php_fpm_config_input_path)
        print "php_fpm_config_input_path=%s" % php_fpm_config_input_path
        print "php_fpm_config_output_path=%s" % php_fpm_config_output_path
        render_conf(php_fpm_config_input_path, php_fpm_config_output_path, keymap)

    def _start_php_fpm(self):
        print "2, start php-fpm"
        php_config_input_path = self.conf["php_config_path"]
        php_config_output_path = self._get_render_file_path(php_config_input_path)

        php_fpm_config_input_path = self.conf["php_fpm_config_path"]
        php_fpm_config_output_path =self._get_render_file_path(php_fpm_config_input_path)

        # 1, start
        php_fpm_start_commands = self.conf["php_fpm_start_commands"] % (php_config_output_path,
                                                                        php_fpm_config_output_path)
        print "php_fpm_start_commands=%s" % php_fpm_start_commands
        p = subprocess.Popen(php_fpm_start_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)

        returncode = p.wait()
        if returncode != 0:
            print "Start php-fpm fail."
            sys.exit(-1)

    def _start_nginx(self):
        print "3, start nginx"
        nginx_conf_input_path = self.conf["nginx_conf_path"]
        nginx_conf_output_path = self._get_render_file_path(nginx_conf_input_path)
        print "nginx_conf_input_path=%s" % nginx_conf_input_path
        print "nginx_conf_output_path=%s" % nginx_conf_output_path
        render_conf(nginx_conf_input_path, nginx_conf_output_path, self.conf)

        nginx_start_commands = self.conf["nginx_start_commands"] % nginx_conf_output_path
        print "nginx_start_commands=%s" % nginx_start_commands
        p = subprocess.Popen(nginx_start_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)
        returncode = p.wait()
        if returncode != 0:
            print "Start nginx fail."
            sys.exit(-1)

    def _start_httpd(self):
        print "-------------- start httpd ----------------"
        httpd_conf_input_path  = self.conf["httpd_conf_path"]
        httpd_conf_output_path = self._get_render_file_path(httpd_conf_input_path)
        print "httpd_conf_input_path=%s" % httpd_conf_input_path
        print "httpd_conf_output_path=%s" % httpd_conf_output_path
        render_conf(httpd_conf_input_path, httpd_conf_output_path, self.conf)

        httpd_start_commands = self.conf["httpd_start_commands"] % httpd_conf_output_path
        print "httpd_start_commands=%s" % httpd_start_commands
        p = subprocess.Popen(httpd_start_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)
        returncode = p.wait()
        if returncode != 0:
            print "Start httpd fail."
            sys.exit(-1)


    def _start_agent_plugin(self):
        # send a packet
        print "send a packet to start php plugin..."
        web_addr = "http://127.0.0.1:%s/index.php" %get_local_port(self.conf)
        print "web_addr=%s" % web_addr
        requests.get(web_addr)
        time.sleep(5)

    def _stop_nginx(self, ignore_error=False):
        print "stop nginx"
        nginx_conf_input_path = self.conf["nginx_conf_path"]
        nginx_conf_output_path = self._get_render_file_path(nginx_conf_input_path)

        print "nginx_conf_input_path=%s" % nginx_conf_input_path
        print "nginx_conf_output_path=%s" % nginx_conf_output_path
        render_conf(nginx_conf_input_path, nginx_conf_output_path, self.conf)
        nginx_stop_commands = self.conf["nginx_stop_commands"] % nginx_conf_output_path
        print "nginx_stop_commands=%s" % nginx_stop_commands
        p = subprocess.Popen(nginx_stop_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)

        returncode = p.wait()
        if not ignore_error and returncode != 0:
            print "Stop nginx fail."
            sys.exit(-1)

    def _stop_httpd(self, ignore_error=False):    
        print "stop httpd"
        httpd_conf_input_path = self.conf["httpd_conf_path"]
        httpd_conf_output_path = self._get_render_file_path(httpd_conf_input_path)

        print "httpd_conf_input_path=%s" % httpd_conf_input_path
        print "httpd_conf_output_path=%s" % httpd_conf_output_path
        render_conf(httpd_conf_input_path, httpd_conf_output_path, self.conf)
        httpd_stop_commands = self.conf["httpd_stop_commands"] % httpd_conf_output_path
        print "nginx_stop_commands=%s" % httpd_stop_commands
        p = subprocess.Popen(httpd_stop_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)

        returncode = p.wait()
        if not ignore_error and returncode != 0:
            print "Stop httpd fail."
            sys.exit(-1)

    def _stop_php_fpm(self, ignore_error=False):
        php_fpm_stop_commands = self.conf["php_fpm_stop_commands"]
        print "php_fpm_stop_commands=%s" % php_fpm_stop_commands
        p = subprocess.Popen(php_fpm_stop_commands,
                             cwd=self.conf["tmp_path"],
                             shell=True)

        returncode = p.wait()
        if not ignore_error and returncode != 0:
            print "Stop php-fpm fail."
            sys.exit(-1)

    def build_agent(self):
        print "start php agent ..."
        if self.conf["build"]:
            self._build_agent()

    def start_agent(self):
        self.render_all_conf()
        if os.getenv('PINPOINT_BACKEND_TYPYE') is None  or (os.getenv('PINPOINT_BACKEND_TYPYE').upper() == "NGINX"):
            self._start_php_fpm()
            self._start_nginx()
        else:
            self._start_httpd()
        self._start_agent_plugin()

    def stop_agent(self, ignore_error=False):
        if os.getenv('PINPOINT_BACKEND_TYPYE') is None  or (os.getenv('PINPOINT_BACKEND_TYPYE').upper() == "NGINX"):
            print "stop php agent"
            self._stop_nginx(ignore_error)
            if not self.conf.get("coverage", True):
                self._stop_php_fpm(ignore_error)
        else:   
            self._stop_httpd(ignore_error) 
            # self._stop_php_fpm(ignore_error)