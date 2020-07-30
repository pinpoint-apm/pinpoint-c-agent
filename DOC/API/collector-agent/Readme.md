## Json string map to Pinpoint item

Json String | Pinpoint Name | Details 
:---|:---|:---
tid|transactionId|Transaction ID
sid|SpanId|Span ID
S|startTime|Start time of request
E|elapsed|Elapse time
FT|front type|Language type(PHP:1500, Python:1700)
appname|applicationName|Application name
appid|agentId|Agent ID
stp|serviceType|Service type([See details](https://github.com/naver/pinpoint/blob/master/commons/src/main/java/com/navercorp/pinpoint/common/trace/ServiceType.java))
name| apids|Function name
uri|rpc|URI
clues|TAnnotation|-1:arguments<br>14:return<br>40:http url<br>41:http param<br>42:http param entity<br>45:cookies<br>46:http status code
psid|parentSpanId|Parent span ID
nsid|nextSpanId|Next span ID
dst|destinationId|Destination ID
client|remoteAddr|Client address
server|endPoint|Server address
ERR|err|Error
EXP|exp|Exception
pname|parentApplicationName|Parent application name
ptype|parentApplicationType|Parent application type
Ah|acceptorHost|Acceptor host
NP|NginxProxy header|Nginx proxy header
AP|ApacheProxy header|Apache proxy header
