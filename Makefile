# Code Educator Project Makefile
# 기본 변수 설정
PYTHON := python3
CMAKE := cmake
MAKE := make
DOCKER_COMPOSE := docker-compose
BUILD_DIR := build
INSTALL_DIR := $(shell pwd)/dist

.PHONY: all build test clean install docker-build docker-start docker-stop docker-test docker-model fmt help

# 기본 타겟
all: build

# 로컬 빌드
build:
	@echo "🔨 Building project locally..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE)
	@echo "✅ Build completed!"

# 로컬 테스트
test: build
	@echo "🧪 Running tests..."
	@cd $(BUILD_DIR) && $(CMAKE) --build . --target test
	@echo "✅ Tests completed!"

kinstall: build
	@echo "📦 Installing Python package..."
	$(PYTHON) -m pip install -e .
	@echo "✅ Installation completed!"

# 클린업
clean:
	@echo "🧹 Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@rm -rf *.egg-info
	@rm -rf dist/
	@rm -rf __pycache__
	@rm -rf **/__pycache__
	@echo "✅ Clean completed!"

# Docker 환경
docker-build:
	@echo "🐳 Building Docker containers..."
	$(DOCKER_COMPOSE) build
	@echo "✅ Docker build completed!"

docker-start:
	@echo "🚀 Starting Docker containers..."
	$(DOCKER_COMPOSE) up -d
	@echo "✅ Docker containers started!"

docker-stop:
	@echo "🛑 Stopping Docker containers..."
	$(DOCKER_COMPOSE) down
	@echo "✅ Docker containers stopped!"

docker-test: docker-start
	@echo "🧪 Running tests in Docker..."
	$(DOCKER_COMPOSE) exec codementor pytest
	@echo "✅ Docker tests completed!"

docker-build-project: docker-start
	@echo "🔨 Building project in Docker..."
	$(DOCKER_COMPOSE) exec codementor bash -c "mkdir -p build && cd build && cmake .. && make"
	@echo "✅ Docker build completed!"

docker-model: docker-start
	@echo "📥 Downloading CodeLlama model..."
	$(DOCKER_COMPOSE) exec ollama ollama pull codellama
	@echo "✅ Model downloaded successfully!"

docker-shell: docker-start
	@echo "🔌 Connecting to Docker container..."
	$(DOCKER_COMPOSE) exec codementor bash

# 도움말
help:
	@echo "Code Educator Makefile Help"
	@echo "--------------------------"
	@echo "Available targets:"
	@echo "  make              - Build the project"
	@echo "  make build        - Build the project locally"
	@echo "  make test         - Run tests locally"
	@echo "  make install      - Install the Python package locally"
	@echo "  make clean        - Clean build artifacts"
	@echo ""
	@echo "Docker targets:"
	@echo "  make docker-build        - Build Docker containers"
	@echo "  make docker-start        - Start Docker containers"
	@echo "  make docker-stop         - Stop Docker containers"
	@echo "  make docker-test         - Run tests in Docker"
	@echo "  make docker-build-project - Build project in Docker"
	@echo "  make docker-model        - Download CodeLlama model"
	@echo "  make docker-shell        - Connect to Docker container"
	@echo ""
	@echo "For more information, see README.md"
