FROM python:3.7
WORKDIR /app/

COPY testapps/py-celery-rabbit-mq/app.py /app/app.py
COPY testapps/py-celery-rabbit-mq/requirements.txt /app/requirements.txt
COPY testapps/py-celery-rabbit-mq/run_all.sh /app/run_all.sh

# RUN pip install -i https://test.pypi.org/simple/ pinpointPy==1.4.1

COPY setup.py /pinpoint-c-agent/setup.py
COPY common/ /pinpoint-c-agent/common
COPY README /pinpoint-c-agent/README
COPY plugins/PY /pinpoint-c-agent/plugins/PY
COPY src/PY /pinpoint-c-agent/src/PY

RUN pip install -r requirements.txt
RUN cd /pinpoint-c-agent && pip install -e .



CMD [ "sh" ,"/app/run_all.sh" ]