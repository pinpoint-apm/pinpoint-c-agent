FROM photonixapp/photonix:latest

COPY testapps/py-photonix/settings.py /srv/photonix/web/
RUN pip install pinpointpy

# COPY setup.py /pinpoint-c-agent/setup.py
# COPY common/ /pinpoint-c-agent/common
# COPY README /pinpoint-c-agent/README
# COPY plugins/PY /pinpoint-c-agent/plugins/PY
# COPY src/PY /pinpoint-c-agent/src/PY

# RUN cd /pinpoint-c-agent && pip install -e .