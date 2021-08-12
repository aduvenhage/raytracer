FROM arnoduvenhage/raytracer_cli

WORKDIR /usr/local/raytracer

COPY . .
COPY ./deployment/raytracer_cli/run.sh /run.sh

RUN cmake -DCMAKE_BUILD_TYPE=RELEASE && \
    make -j4
