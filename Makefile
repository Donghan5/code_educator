# Code Educator Project Makefile with Virtual Environment Support
PYTHON := python3
CMAKE := cmake
MAKE := make
VENV_DIR := venv
VENV_PYTHON := $(VENV_DIR)/bin/python
VENV_PIP := $(VENV_DIR)/bin/pip
BUILD_DIR := build

# 가상환경 활성화 상태 확인
VIRTUAL_ENV_CHECK := $(shell echo $$VIRTUAL_ENV)
ifeq ($(VIRTUAL_ENV_CHECK),)
    PYTHON_CMD := $(VENV_PYTHON)
    PIP_CMD := $(VENV_PIP)
    VENV_ACTIVE := false
else
    PYTHON_CMD := python
    PIP_CMD := pip
    VENV_ACTIVE := true
endif

.PHONY: all setup venv venv-check activate install build test server cli clean help

# 기본 타겟
all: setup build

# 프로젝트 초기 설정
setup: venv install

# 가상환경 생성
venv:
	@echo "🐍 가상환경 생성 중..."
	@if [ ! -d "$(VENV_DIR)" ]; then \
		$(PYTHON) -m venv $(VENV_DIR); \
		echo "✅ 가상환경 생성 완료"; \
	else \
		echo "✅ 가상환경이 이미 존재합니다"; \
	fi

# 가상환경 상태 확인
venv-check:
	@if [ ! -d "$(VENV_DIR)" ]; then \
		echo "❌ 가상환경이 없습니다. 'make venv'로 생성하세요"; \
		exit 1; \
	fi
	@if [ "$(VENV_ACTIVE)" = "false" ]; then \
		echo "⚠️  가상환경이 비활성화되어 있습니다"; \
		echo "활성화: source venv/bin/activate"; \
	else \
		echo "✅ 가상환경 활성화됨"; \
	fi

# 가상환경 활성화 (안내)
activate:
	@echo "가상환경 활성화 방법:"
	@echo "  source venv/bin/activate"
	@echo ""
	@echo "또는 활성화 스크립트 사용:"
	@echo "  ./activate.sh"

# 의존성 설치
install: venv-check
	@echo "📦 의존성 설치 중..."
	@$(VENV_PIP) install --upgrade pip
	@if [ -f "requirements.txt" ]; then \
		$(VENV_PIP) install -r requirements.txt; \
	fi
	@echo "✅ 의존성 설치 완료"

# C++ 모듈 빌드
build: venv-check
	@echo "🔨 C++ 모듈 빌드 중..."
	@if ! command -v cmake >/dev/null 2>&1; then \
		echo "❌ CMake가 설치되어 있지 않습니다."; \
		echo "💡 CMake 설치 방법:"; \
		echo "   Ubuntu/Debian: sudo apt install cmake build-essential"; \
		echo "   macOS: brew install cmake"; \
		echo "   또는 Python 전용 모드: make build-python-only"; \
		exit 1; \
	fi
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE)
	@$(VENV_PIP) install -e .
	@echo "✅ 빌드 완료"

# 빌드 (가상환경 없이)
build-native:
	@echo "🔨 네이티브 빌드 중..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE)
	@echo "✅ 네이티브 빌드 완료"

# 테스트 실행
test: venv-check
	@echo "🧪 테스트 실행 중..."
	@if [ "$(VENV_ACTIVE)" = "false" ]; then \
		$(VENV_PYTHON) -m pytest; \
	else \
		python -m pytest; \
	fi
	@echo "✅ 테스트 완료"

# CLI 테스트
cli-test: venv-check
	@echo "🧪 CLI 테스트 중..."
	@if [ "$(VENV_ACTIVE)" = "false" ]; then \
		$(VENV_PYTHON) -m srcs.python.cli check; \
	else \
		code-educator check; \
	fi

# 웹 서버 실행
server: venv-check
	@echo "🚀 웹 서버 시작 중..."
	@if [ "$(VENV_ACTIVE)" = "false" ]; then \
		$(VENV_PYTHON) -m uvicorn srcs.python.server:app --reload; \
	else \
		uvicorn srcs.python.server:app --reload; \
	fi

# 개발 서버 (포트 지정)
dev-server: venv-check
	@echo "🚀 개발 서버 시작 중 (포트 8000)..."
	@if [ "$(VENV_ACTIVE)" = "false" ]; then \
		$(VENV_PYTHON) -m uvicorn srcs.python.server:app --reload --host 0.0.0.0 --port 8000; \
	else \
		uvicorn srcs.python.server:app --reload --host 0.0.0.0 --port 8000; \
	fi

# 환경 정보 표시
info:
	@echo "📊 Code Educator 환경 정보"
	@echo "=========================="
	@echo "Python: $(shell $(PYTHON) --version)"
	@echo "가상환경 디렉토리: $(VENV_DIR)"
	@echo "가상환경 존재: $(shell [ -d $(VENV_DIR) ] && echo 'Yes' || echo 'No')"
	@echo "가상환경 활성화: $(VENV_ACTIVE)"
	@echo "빌드 디렉토리: $(BUILD_DIR)"
	@echo "현재 VIRTUAL_ENV: $(VIRTUAL_ENV_CHECK)"

# 클린업
clean:
	@echo "🧹 정리 중..."
	@rm -rf $(BUILD_DIR)
	@rm -rf *.egg-info
	@rm -rf dist/
	@find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	@find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@echo "✅ 정리 완료"

# 가상환경까지 삭제
clean-all: clean
	@echo "🗑️  가상환경까지 삭제 중..."
	@rm -rf $(VENV_DIR)
	@echo "✅ 전체 정리 완료"

# 도움말
help:
	@echo "Code Educator Makefile 도움말"
	@echo "============================="
	@echo ""
	@echo "🚀 빠른 시작:"
	@echo "  make setup        - 가상환경 생성 및 의존성 설치"
	@echo "  source venv/bin/activate  - 가상환경 활성화"
	@echo "  make build        - 프로젝트 빌드"
	@echo "  make server       - 웹 서버 실행"
	@echo ""
	@echo "📦 설정:"
	@echo "  make venv         - 가상환경 생성"
	@echo "  make install      - 의존성 설치"
	@echo "  make activate     - 가상환경 활성화 방법 안내"
	@echo ""
	@echo "🔨 빌드:"
	@echo "  make build        - C++ 모듈 포함 전체 빌드"
	@echo "  make build-native - 가상환경 없이 빌드"
	@echo ""
	@echo "🧪 테스트:"
	@echo "  make test         - 전체 테스트 실행"
	@echo "  make cli-test     - CLI 기능 테스트"
	@echo ""
	@echo "🚀 실행:"
	@echo "  make server       - 웹 서버 실행 (개발 모드)"
	@echo "  make dev-server   - 개발 서버 (포트 8000 고정)"
	@echo ""
	@echo "📊 정보:"
	@echo "  make info         - 환경 정보 표시"
	@echo "  make venv-check   - 가상환경 상태 확인"
	@echo ""
	@echo "🧹 정리:"
	@echo "  make clean        - 빌드 파일 정리"
	@echo "  make clean-all    - 가상환경까지 전체 삭제"
	@echo ""
	@echo "💡 팁:"
	@echo "  - 처음 사용: make setup"
	@echo "  - 가상환경 활성화 후 더 빠른 명령어 사용 가능"
	@echo "  - Docker 사용: docker-compose up"