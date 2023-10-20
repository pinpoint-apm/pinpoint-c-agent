FROM python:3.11
WORKDIR /app/

COPY ./PY/ /app/
RUN pip install -r requirements.txt

EXPOSE 8184
CMD [ "python", "app.py" ]