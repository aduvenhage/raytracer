FROM debian:buster-slim
WORKDIR /usr/local/raytracer

RUN apt-get update && \
    apt-get install -y build-essential cmake libjpeg-dev

COPY . .

RUN cmake -DCMAKE_BUILD_TYPE=RELEASE && \
    make -j4
