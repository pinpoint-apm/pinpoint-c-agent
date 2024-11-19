FROM ghcr.io/immich-app/immich-machine-learning:${IMMICH_VERSION:-release}

# RUN apt update && apt install -y gcc g++
# COPY pinpointpy-1.4.0.tar.gz /usr/src/app/pinpointpy-1.4.0.tar.gz
# RUN pip install /usr/src/app/pinpointpy-1.4.0.tar.gz
RUN pip install starlette-context && pip install starlette
RUN pip install -i https://test.pypi.org/simple/ pinpointPy==1.4.0
COPY main.py /usr/src/app/main.py