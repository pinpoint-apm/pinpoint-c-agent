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
appname|applicationName
appid|agentId
FT|front type(PHP)

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
