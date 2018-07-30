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

import json
import re
import os

def get_json_conf(conf_path):
    with open(conf_path, 'r') as f:
        return json.load(f)


# render {{key}} => value
# case sensitive
def render_conf(input_conf_path, output_conf_path, kmap):
    p = re.compile("\{\{(\w*)\}\}")

    context = ""
    # print "input_conf_path %s output_conf_path %s" % (input_conf_path,output_conf_path)
    with open(input_conf_path, 'r') as infile:
        for line in infile.readlines():
            m = p.search(line)
            if m:
                while m is not None:
                    context += line[0: m.start()] + str(kmap[m.group(1)])
                    line = line[m.end():]
                    m = p.search(line)
            context += line

    with open(output_conf_path, 'w') as outfile: 
        outfile.write(context)


def render_str(input_str, kmap):
    p = re.compile("\{\{(\w*)\}\}")
    m = p.search(input_str)
    if m:
        new_line = ""
        while m is not None:
            new_line += input_str[0: m.start()] + str(kmap[m.group(1)])
            input_str = input_str[m.end():]
            m = p.search(input_str)
        new_line += input_str
    else:
        new_line = input_str
    return new_line


def render_dict(d, kmap):
    for key, value in d.items():
        if isinstance(value, (str, unicode)):
            d[key] = render_str(value, kmap)
    return d

def get_local_port(conf):
    # if (os.getenv('PINPOINT_BACKEND_TYPYE') is None):
    #     port = conf["nginx_listen_port"]
    # elif(os.getenv('PINPOINT_BACKEND_TYPYE').upper() == "NGINX"):
    #     port = conf["nginx_listen_port"]
    # elif (os.getenv('PINPOINT_BACKEND_TYPYE').upper() == "APACHE"):
    #     port = conf["httpd_listen_port"]
    # else:
    port = conf["nginx_listen_port"]
    return port