FROM --platform=linux/arm64 ubuntu:22.04

# First install all system packages
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

# Then install Python packages in a separate RUN command
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

# Default command
CMD ["bash"]
