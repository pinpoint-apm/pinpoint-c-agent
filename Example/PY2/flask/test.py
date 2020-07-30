#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Created by eeliu at 7/14/20
import requests
import json
response = requests.patch('http://order.demo.pinpoint.com:8182/orders/'+'90d33c40-8540-4f23-8ada-8fd88bf73da0',json='{"paymentAmount": 1850}')

print(response.status_code)
