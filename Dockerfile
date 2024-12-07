FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /app

RUN apt-get update && apt-get install -y \
    bash \
    g++ \
    gcc \
    flex \
    bison \
    python2-dev \
    make \
    build-essential \
    python-is-python3 \
    python3-pip \
    libeigen3-dev

COPY ./commands/*.sh .

CMD ["tail", "-f", "/dev/null"]