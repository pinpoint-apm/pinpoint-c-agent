FROM python:3.8

ARG DIR="/CollectorAgent"

WORKDIR ${DIR}

ENV COLLECTOR_CONFIG=${DIR}/conf/collector.conf
ENV COLLECTOR_PATH=collector-agent
## files
COPY ${COLLECTOR_PATH}/CollectorAgent/ ${DIR}/CollectorAgent
COPY ${COLLECTOR_PATH}/Common/ ${DIR}/Common/
COPY ${COLLECTOR_PATH}/conf/ ${DIR}/conf/
COPY ${COLLECTOR_PATH}/Events/ ${DIR}/Events/
COPY ${COLLECTOR_PATH}/PHPAgent/ ${DIR}/PHPAgent/
COPY ${COLLECTOR_PATH}/PinpointAgent/ ${DIR}/PinpointAgent/
COPY ${COLLECTOR_PATH}/Proto/ ${DIR}/Proto
COPY ${COLLECTOR_PATH}/run.py/ ${DIR}/
COPY ${COLLECTOR_PATH}/init_python_env.sh ${DIR}/
COPY ${COLLECTOR_PATH}/requirements.txt ${DIR}

RUN pip --no-cache-dir install -r requirements.txt && pip --no-cache-dir install grpcio-tools && python -m grpc_tools.protoc -I${DIR}/Proto/grpc --python_out=${DIR}/Proto/grpc --grpc_python_out=${DIR}/Proto/grpc ${DIR}/Proto/grpc/*.proto

CMD ["python","run.py"]