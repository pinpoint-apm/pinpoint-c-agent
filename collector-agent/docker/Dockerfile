FROM python:3.8

ARG DIR="/CollectorAgent"

WORKDIR ${DIR}
ENV COLLECTOR_CONFIG=${DIR}/conf/collector.conf
## files
COPY CollectorAgent/ ${DIR}/CollectorAgent
COPY Common/ ${DIR}/Common/
COPY conf/ ${DIR}/conf/
COPY Events/ ${DIR}/Events/
COPY PHPAgent/ ${DIR}/PHPAgent/
COPY PinpointAgent/ ${DIR}/PinpointAgent/
COPY Proto/ ${DIR}/Proto
COPY run.py/ ${DIR}/
COPY init_python_env.sh ${DIR}/
COPY requirements.txt ${DIR}
RUN pip --no-cache-dir install -r requirements.txt && pip --no-cache-dir install grpcio-tools && python -m grpc_tools.protoc -I${DIR}/Proto/grpc --python_out=${DIR}/Proto/grpc --grpc_python_out=${DIR}/Proto/grpc ${DIR}/Proto/grpc/*.proto

CMD ["python","run.py"]