FROM maven:3.8.6-jdk-8 as build
WORKDIR /workspace
COPY testapps/java_call_app /workspace

RUN mvn package

FROM tomcat:9
COPY --from=build  /workspace/target/javacallapp.war /usr/local/tomcat/webapps/
WORKDIR /workspace
ENV AGENT_VERSION=2.5.4
RUN wget https://github.com/pinpoint-apm/pinpoint/releases/download/v${AGENT_VERSION}/pinpoint-agent-${AGENT_VERSION}.tar.gz && tar xvf pinpoint-agent-${AGENT_VERSION}.tar.gz
COPY testapps/java_call_app/conf/pinpoint.config  /workspace/pinpoint-agent-${AGENT_VERSION}/profiles/release/pinpoint.config
COPY testapps/java_call_app/javacallapp.xml /usr/local/tomcat/conf/Catalina/localhost/
ENV CATALINA_OPTS="-javaagent:/workspace/pinpoint-agent-${AGENT_VERSION}/pinpoint-bootstrap-${AGENT_VERSION}.jar -Dpinpoint.agentId=test-callapp3 -Dpinpoint.applicationName=cd.dev.java"

ENV MAVEN_OPTS="${MAVEN_OPTS} ${CATALINA_OPTS}"



