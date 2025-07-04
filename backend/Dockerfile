FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-dev \
    python3-pip \
    python3-venv \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY requirements.txt .
RUN pip3 install --no-cache-dir -r requirements.txt

COPY . .

COPY CMakeLists.txt ./CMakeLists.txt
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make || echo "C++ 빌드 실패, Python만 사용"


RUN pip3 install -e .

ENV OLLAMA_API_BASE=http://ollama:11434

EXPOSE 8000

CMD ["python3", "-m", "uvicorn", "srcs.python.server:app", "--host", "0.0.0.0", "--port", "8000", "--app-dir", "/app"]
