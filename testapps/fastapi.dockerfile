FROM python:3.12
WORKDIR /app/

COPY testapps/fastapi/ /app/
RUN pip install -r requirements.txt

COPY setup.py /pinpoint-c-agent/setup.py
COPY common/ /pinpoint-c-agent/common
COPY README /pinpoint-c-agent/README
COPY plugins/PY /pinpoint-c-agent/plugins/PY
COPY src/PY /pinpoint-c-agent/src/PY
RUN cd /pinpoint-c-agent && pip install -e .

EXPOSE 8000
CMD [ "uvicorn", "main:app","--host=0.0.0.0","--port=8000","--reload" ]