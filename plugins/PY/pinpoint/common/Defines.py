#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# ------------------------------------------------------------------------------
#  Copyright  2020. NAVER Corp.                                                -
#                                                                              -
#  Licensed under the Apache License, Version 2.0 (the "License");             -
#  you may not use this file except in compliance with the License.            -
#  You may obtain a copy of the License at                                     -
#                                                                              -
#   http://www.apache.org/licenses/LICENSE-2.0                                 -
#                                                                              -
#  Unless required by applicable law or agreed to in writing, software         -
#  distributed under the License is distributed on an "AS IS" BASIS,           -
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    -
#  See the License for the specific language governing permissions and         -
#  limitations under the License.                                              -
# ------------------------------------------------------------------------------

# Created by eeliu at 7/31/20



PP_HTTP_PINPOINT_PSPANID = 'HTTP_PINPOINT_PSPANID'
PP_HTTP_PINPOINT_SPANID = 'HTTP_PINPOINT_SPANID'
PP_HTTP_PINPOINT_TRACEID = 'HTTP_PINPOINT_TRACEID'
PP_HTTP_PINPOINT_PAPPNAME = 'HTTP_PINPOINT_PAPPNAME'
PP_HTTP_PINPOINT_PAPPTYPE = 'HTTP_PINPOINT_PAPPTYPE'
PP_HTTP_PINPOINT_HOST = 'HTTP_PINPOINT_HOST'

PP_HEADER_PINPOINT_PSPANID = 'Pinpoint-Pspanid'
PP_HEADER_PINPOINT_SPANID = 'Pinpoint-Spanid'
PP_HEADER_PINPOINT_TRACEID = 'Pinpoint-Traceid'
PP_HEADER_PINPOINT_PAPPNAME = 'Pinpoint-Pappname'
PP_HEADER_PINPOINT_PAPPTYPE = 'Pinpoint-Papptype'
PP_HEADER_PINPOINT_HOST = 'Pinpoint-Host'

PP_HEADER_NGINX_PROXY = 'Pinpoint-ProxyNginx'
PP_HTTP_NGINX_PROXY = 'HTTP_Pinpoint-ProxyNginx'
PP_HEADER_APACHE_PROXY = 'PINPOINT_PROXYAPACHE'
PP_HTTP_APACHE_PROXY = 'HTTP_PINPOINT_PROXYAPACHE'

PP_DESTINATION='dst'
PP_HTTP_SAMPLED = 'HTTP_PINPOINT_SAMPLED'
PP_INTERCEPTER_NAME='name'
PP_APP_NAME='appname'
PP_APP_ID='appid'
PP_REQ_URI='uri'
PP_REQ_CLIENT='client'
PP_REQ_SERVER='server'
PP_SERVER_TYPE='stp'
PP_PARENT_SPAN_ID='psid'

PP_PARENT_NAME='pname'

PP_PARENT_TYPE='ptype'
PP_PARENT_HOST='Ah'
PP_NGINX_PROXY='NP'
PP_APACHE_PROXY='AP'
PP_TRANSCATION_ID='tid'
PP_SPAN_ID='sid'
PP_NOT_SAMPLED='s0'
PP_SAMPLED='s1'
PP_NEXT_SPAN_ID='nsid'
PP_ADD_EXCEPTION='EXP'


PP_SQL_FORMAT='SQL'
PP_ARGS='-1'
PP_RETURN='14'
PYTHON = '1700'
PP_METHOD_CALL= '1701'
PP_REMOTE_METHOD = '9900'

PP_HTTP_URL= '40'
PP_HTTP_PARAM= '41'
PP_HTTP_PARAM_ENTITY= '42'
PP_HTTP_COOKIE= '45'
PP_HTTP_STATUS_CODE= '46'
PP_HTTP_INTERNAL_DISPLAY=48
PP_HTTP_IO=49
PP_MESSAGE_QUEUE_URI=100


PP_MYSQL= '2101'
PP_REDIS= '8200'
PP_REDIS_REDISSON= '8203'
PP_REDIS_REDISSON_INTERNAL= '8204'
PP_POSTGRESQL='2501'
PP_MEMCACHED= '8050'
PP_MONGDB_EXE_QUERY= '2651'
PP_KAFKA='8660'

PP_KAFKA_TOPIC=140
