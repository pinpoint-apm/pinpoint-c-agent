#!/usr/bin/env bash
PINPOINT_DIR="/workspace"
pinpoint_agent="${PINPOINT_DIR}/pinpoint-agent-2.4.2/pinpoint-bootstrap-2.4.2.jar"
pinpoint_opt="-javaagent:$pinpoint_agent -Dpinpoint.agentId=test-callapp3 -Dpinpoint.applicationName=cd.dev.java"
maven_opt=$MAVEN_OPTS
export MAVEN_OPTS="${MAVEN_OPTS} ${pinpoint_opt}"
#mvn -f ./pom.xml clean package tomcat7:run > call.log 2>&1
MAVEN_OPTS=$maven_opt
mvn -f ./pom.xml clean package tomcat7:run 

