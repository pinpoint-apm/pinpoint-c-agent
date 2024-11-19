#!/bin/bash

celery -A app.celery worker &

python app.py 