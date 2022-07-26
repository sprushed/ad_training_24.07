FROM python:buster

WORKDIR /app
COPY src/requirements.txt /app/
RUN pip3 install -r /app/requirements.txt
COPY src /app/

USER nobody

CMD ["python3", "/app/main.py"]
