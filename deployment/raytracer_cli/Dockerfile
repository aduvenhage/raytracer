FROM debian:buster-slim
WORKDIR /usr/local/raytracer

RUN apt-get update && \
    apt-get install -y build-essential cmake libjpeg-dev

COPY . .
