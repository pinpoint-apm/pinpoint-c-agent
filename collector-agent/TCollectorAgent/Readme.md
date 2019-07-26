#-------------------------------------------------------------------------------
# Copyright 2019 NAVER Corp
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#-------------------------------------------------------------------------------
## Json string map to thrift item

Json String | Thrift Name
---|----
tid|transactionId
psid|parentSpanId
nspid|nextSpanId
name| apids[name]
calls|
uri|rpc
client|remoteAddr
server|endPoint
dst|destinationId
http_host|
ER|err
EMSG|exceptionInfo
S|startTime
E|elapsed
clues|TAnnotation
pname|parentApplicationName
ptype|parentApplicationType
stp|serviceType
NP|NginxProxy header
AP|ApacheProxy header
Ah|acceptorHost


## NextSpan

``` 
+------------------+          +---------------------+
|   Span           |          |  pspan              |
|   NextSpan       |          |  span               |
|   TransactionID  |          |  nextspan           |
|                  |          |  TransactionID      |
|                  |          |                     |
+------------------+          +---------------------+
```

## Special header

> PINPOINT_SAMPLE_HTTP_HEADER: FALSE 

    drop this request
    
    
## PHP-Cli to Collector-agent

```buildoutcfg
+---------------------------------+
|         4|         8|   ......  |
+---------------------------------+
|  TYPE    | LEN      |  Payload  |
+----------+----------+-----------+

```
 
### Collector to php-c

> type = 0

id,time,name
