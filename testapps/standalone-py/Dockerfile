FROM python:3.11
WORKDIR /app/

COPY testapps/standalone-py/run.py /app/run.py

# COPY setup.py /pinpoint-c-agent/setup.py
# COPY common/ /pinpoint-c-agent/common
# COPY README /pinpoint-c-agent/README
# COPY plugins/PY /pinpoint-c-agent/plugins/PY
# COPY src/PY /pinpoint-c-agent/src/PY

# # RUN pip install -r requirements.txt
# RUN cd /pinpoint-c-agent && pip install -e .
RUN pip install -i https://test.pypi.org/simple/ pinpointPy==1.4.0

CMD [ "python", "run.py" ]