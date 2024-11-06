FROM python:3.12
WORKDIR /app/

COPY testapps/django/mysite/ /app/

COPY setup.py /pinpoint-c-agent/setup.py
COPY common/ /pinpoint-c-agent/common
COPY README /pinpoint-c-agent/README
COPY plugins/PY /pinpoint-c-agent/plugins/PY
COPY src/PY /pinpoint-c-agent/src/PY

RUN pip install -r requirements.txt
RUN cd /pinpoint-c-agent && pip install -e .

EXPOSE 8000
CMD [ "python","manage.py","runserver","0.0.0.0:8000" ]