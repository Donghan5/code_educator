FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-dev \
    python3-pip \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install --no-cache-dir \
    pybind11 \
    pytest \
    click \
    requests \
    setuptools \
    wheel

WORKDIR /app

# default Ollama API
ENV OLLAMA_API_BASE=http://ollama:11434
ENV PYTHONPATH=/app

# 기본 명령어
CMD ["bash"]
