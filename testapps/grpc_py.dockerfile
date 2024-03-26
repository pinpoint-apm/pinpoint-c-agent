FROM python:3.8
WORKDIR /app/

COPY testapps/grpc-py/ /app/
RUN pip install -r requirements.txt

COPY setup.py /pinpoint-c-agent/setup.py
COPY common/ /pinpoint-c-agent/common
COPY README /pinpoint-c-agent/README
COPY plugins/PY /pinpoint-c-agent/plugins/PY
COPY src/PY /pinpoint-c-agent/src/PY
RUN cd /pinpoint-c-agent && pip install -e .

EXPOSE 50050
CMD [ "python", "route_guide_server.py"]