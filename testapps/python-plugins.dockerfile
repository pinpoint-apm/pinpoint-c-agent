FROM python:3.8

COPY setup.py /pinpoint-c-agent/setup.py
COPY common/ /pinpoint-c-agent/common
COPY README /pinpoint-c-agent/README
COPY plugins/PY /pinpoint-c-agent/plugins/PY
COPY src/PY /pinpoint-c-agent/src/PY


RUN pip install -r /pinpoint-c-agent/plugins/PY/requirements.txt
RUN cd /pinpoint-c-agent && pip install -e .
CMD ["python","-m","unittest","discover","-s","/pinpoint-c-agent/plugins/PY/pinpointPy/"]
# CMD [ "python","-m","unittest","pinpointPy.libs._MysqlConnector.test_case.Test_Case" ]
# CMD [ "bash" ]
# python -m unittest discover -s /pinpoint-c-agent/plugins/PY/pinpointPy/