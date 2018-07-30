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

from config import *
from glob import glob
import subprocess
import sys
import os
import fnmatch
import os.path
import time
import requests


class SimulateTest(object):
    HTTP_TESTCASE_PATH = "Pinpoint-Testcase-PATH"

    def __init__(self, config_path):
        self.configPath = config_path
        # self.conf = None
        self.conf = get_json_conf(self.configPath)
        self._normalize_conf()

    def _get_render_file_path(self, input_path):
        d, f = os.path.split(input_path)
        tmp_path = os.path.abspath(self.conf["tmp_path"])
        return os.path.join(tmp_path, f)

    def _send_request(self, case_conf_path):
        case_conf = get_json_conf(case_conf_path)
        # todo : post
        api = render_str(case_conf["api"], self.conf)
        payload = render_dict(case_conf.get("payload", {}), self.conf)
        web_addr = "http://127.0.0.1:%s/%s" % (get_local_port(self.conf), api)
        print "_send_request: web_addr=%s" % web_addr
        print "_send_request: path=%s" % os.path.dirname(case_conf_path)
        print "_send_request: payload=%s" % payload
        r = requests.get(web_addr,
                     headers={SimulateTest.HTTP_TESTCASE_PATH: os.path.dirname(case_conf_path)},
                     params=payload)
        ## find unhappy keyword
        print r.text.encode('utf-8')
        keywords=['<h1>404 Not Found</h1>']
        if any(x in r.text.encode('utf-8') for x in keywords):
            print r.text.encode('utf-8')
            sys.exit("response with some unhappy keywords")



    def _normalize_conf(self):
        assert self.conf is not None
        attrs_conv_to_absolute_path = ("tmp_path",
                                       "testcase_path",
                                       "collector_code_path",
                                       "collector_config_path",
                                       "log_path",
                                       "php_config_path",
                                       "php_fpm_config_path",
                                       "pinpoint_agent_conf_path",
                                       "nginx_conf_path",
                                       "pinpoint_agent_code_path",
                                       "nginx_root_path",
                                       "httpd_root_path",
                                       "httpd_php_ini_dir",
                                       "api_table_file")
        for key, value in self.conf.items():
            if key in attrs_conv_to_absolute_path:
                self.conf[key] = os.path.abspath(value)

        attrs_log_name = ("collector_log_file_name",
                          "php_log_file_name",
                          "php_fpm_log_name",
                          "pinpoint_agent_log_name")
        for key, value in self.conf.items():
            if key in attrs_log_name:
                self.conf[key] = os.path.join(self.conf["log_path"], value)


    def _clean_pinpoint_log(self, log_root_path, log_sub_path):
        log_path = os.path.join(log_root_path, log_sub_path)
        rm_log_path_and_recreate = "rm -fr %s && mkdir -p %s" % (log_path, log_path)
        print "rm_log_path_and_recreate=%s" % rm_log_path_and_recreate
        p = subprocess.Popen(rm_log_path_and_recreate,
                             cwd=log_root_path,
                             shell=True)

        returncode = p.wait()


    def cleanup(self):
        # 1, rm all results
        testcase_path = os.path.abspath(self.conf["testcase_path"])
        rm_all_result_command = 'find . -name "*.result" | xargs rm -f'
        print "rm_all_result_command=%s" %rm_all_result_command
        p = subprocess.Popen(rm_all_result_command,
                             cwd=testcase_path,
                             shell=True)

        returncode = p.wait()

        # 2, clean up log
        log_root_path = os.path.abspath(self.conf["log_path"])
        if self.conf.get("clean_log", False):
            self._clean_pinpoint_log(log_root_path, "agent")
            self._clean_pinpoint_log(log_root_path, "collector")
            self._clean_pinpoint_log(log_root_path, "checker")

# httpd_memory_test

    def test(self):
        self.stop_agent(True)
        self.stop_collector(True)
        self.cleanup()        
        self.start_collector()
        self.build_agent()
        self.start_agent()

        ## needs memory test
        # if (os.getenv('HTTPD_MEMOERY_TEST').upper():
        if os.getenv('HTTPD_MEMOERY_TEST') is not None and os.getenv('HTTPD_MEMOERY_TEST').upper() == 'ON':
            self.run_testcases(20)
            self.stop_agent()
            self.stop_collector()
            return

        ## normal test
        self.run_testcases()
        if self.conf["stop"]:
            print "waiting 60s to stop ..."
            time.sleep(60)
            print "start to stop ..."
            self.stop_agent()
            self.stop_collector()
            self.check_result()

    def build_agent(self):
        print "You need override build_agent!"
        assert False

    def start_agent(self):
        print "You need override start_agent!"
        assert False

    def start_collector(self):
        print "start collector..."
        collector_code_path = os.path.abspath(self.conf["collector_code_path"])
        collector_config_input_path = os.path.abspath(self.conf["collector_config_path"])

        collector_config_output_path = self._get_render_file_path(collector_config_input_path)

        print "collector_code_path=%s" % collector_code_path
        print "collector_config_input_path=%s" % collector_config_input_path
        print "collector_config_output_path=%s" % collector_config_output_path

        # 1, render config
        render_conf(collector_config_input_path, collector_config_output_path, self.conf)

        # 2, build collector
        if self.conf["build"]:
            print "collector_build_commands=%s" % self.conf["collector_build_commands"]
            p = subprocess.Popen(self.conf["collector_build_commands"],
                                 cwd=collector_code_path,
                                 shell=True)

            returncode = p.wait()
            if returncode != 0:
                print "Make collector fail."
                sys.exit(-1)

        # 3, start collector
        collector_start_commands = self.conf["collector_start_commands"] % collector_config_output_path
        print "collector_start_commands=%s" % collector_start_commands
        p = subprocess.Popen(collector_start_commands,
                             cwd=collector_code_path,
                             shell=True)

        returncode = p.wait()
        if returncode != 0:
            print "Start collector fail."
            sys.exit(-1)

    def run_testcases(self, count=1):
        print "run testcases ..."
        if self.conf["core_checker"] == True: 
            print "===>Core checker enabled!"
        else:
            print "===>Core checker disabled!"
        testcase_path = os.path.join(os.path.abspath(self.conf["testcase_path"]), "trace")
        print "testcase_path=%s" % testcase_path
        filelist = os.listdir(testcase_path)
        for filename in filelist:
            abs_filename = os.path.join(testcase_path, filename)
            testcase_conf_path = os.path.join(abs_filename, "case.json")
            # print abs_filename
            # print testcase_conf_path
            if os.path.isdir(abs_filename):
                if os.path.exists(testcase_conf_path):
                    # send two packet to ensure php agent start
                    print "run testcase: %s" % testcase_conf_path
                    if count >1:
                        tcount=count
                        while tcount>=0:
                            self._send_request(testcase_conf_path)
                            tcount = tcount-1
                    else:
                        self._send_request(testcase_conf_path)
                        time.sleep(1)
                        self._send_request(testcase_conf_path)
        
                    if self.conf["core_checker"] == True:                        
                        # Sleep 2s to wait for coredump generating
                        time.sleep(2)
                        case_conf = get_json_conf(testcase_conf_path)
                        api = render_str(case_conf["api"], self.conf)
                        self._core_checker(api)        
                        

    def stop_agent(self, ignore_error=False):
        print "You need override start_agent!"
        assert False

    def stop_collector(self, ignore_error=False):
        if len(os.popen(self.conf["collector_pid"]).readlines()) == 0:
            print "collector is not running!"
        else:
            print "stop collector ..."
            collector_code_path = os.path.abspath(self.conf["collector_code_path"])
            p = subprocess.Popen(self.conf["collector_stop_commands"],
                                 cwd=collector_code_path,
                                 shell=True)

            returncode = p.wait()
            if not ignore_error and returncode != 0:
                print "stop collector fail.%s" %returncode
                sys.exit(-1) 

    def check_result(self):
        print "check result ..."
        # checker use same configuration and log files with collector
        collector_code_path = os.path.abspath(self.conf["collector_code_path"])
        checker_config_input_path = self.conf["checker_config_path"]
        checker_config_output_path = self._get_render_file_path(checker_config_input_path)

        render_conf(checker_config_input_path, checker_config_output_path, self.conf)

        checker_commands = self.conf["checker_commands"] % checker_config_output_path
        print "check_commands=%s" %checker_commands
        p = subprocess.Popen(checker_commands,
                             cwd=collector_code_path,
                             shell=True)

        returncode = p.wait()
        if returncode != 0:
            print "Check failed. See log!!!!"
            sys.exit(-1)
        else:
            print "Simulate test pass!"

    def _core_checker(self, coredump_case):
        print "Core checker..."
        path_list =  self.conf["core_check_path"]
        for path in path_list:
            core_list = glob(os.path.join(path, 'core.*'))
            print "Checking....%s" % path
            if len(core_list) == 0:
                print "===>No core found!"
            else:
                print "===>Coredump generates in folder: %s" % core_list
                print "===>Coredump generates in test case : %s" % coredump_case
                # Core generates, exit the testing!!!
                exit()
        return            

if __name__ == "__main__":
    a = SimulateTest('../conf/php_test_conf.json')
    # a.check_result()
    # a._core_checker("../testcase/php/trace/test_abstract/case.json")
    # a.start_collector()
    a.run_testcases()

