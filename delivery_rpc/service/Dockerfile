FROM python:3.9
EXPOSE 65432

RUN useradd -ms /bin/bash nonroot

WORKDIR /app/
ADD requirements.txt /app/
RUN pip install -r requirements.txt
ADD main.py /app/
RUN mkdir -p /app/data
RUN chmod -R 755 /app/
RUN chmod 777 /app/data

USER nonroot
CMD [ "python", "./main.py" ]