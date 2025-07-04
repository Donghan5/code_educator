#!/bin/bash
# setup_venv.sh - 가상환경 설정 스크립트

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$PROJECT_ROOT/venv"
PYTHON_VERSION="python3"

echo "🐍 Code Educator 가상환경 설정"
echo "프로젝트 경로: $PROJECT_ROOT"

# Python 버전 확인
if ! command -v $PYTHON_VERSION &> /dev/null; then
    echo "❌ Python3이 설치되어 있지 않습니다."
    exit 1
fi

PYTHON_VER_INFO=$($PYTHON_VERSION --version)
echo "✅ $PYTHON_VER_INFO 발견"

# 가상환경 생성
if [ ! -d "$VENV_DIR" ]; then
    echo "📦 가상환경 생성 중..."
    $PYTHON_VERSION -m venv "$VENV_DIR"
    echo "✅ 가상환경 생성 완료: $VENV_DIR"
else
    echo "✅ 가상환경이 이미 존재합니다: $VENV_DIR"
fi

# 가상환경 활성화
echo "🔄 가상환경 활성화 중..."
source "$VENV_DIR/bin/activate"

# pip 업그레이드
echo "📈 pip 업그레이드 중..."
pip install --upgrade pip

# requirements.txt가 있으면 의존성 설치
if [ -f "$PROJECT_ROOT/requirements.txt" ]; then
    echo "📋 의존성 설치 중..."
    pip install -r "$PROJECT_ROOT/requirements.txt"
else
    echo "📋 기본 의존성 설치 중..."
    pip install click requests fastapi uvicorn python-multipart pydantic pybind11 setuptools wheel
fi

# C++ 모듈 빌드 (선택사항)
echo "🔨 C++ 모듈 빌드 시도 중..."
if command -v cmake &> /dev/null; then
    cd "$PROJECT_ROOT"
    if [ -f "CMakeLists.txt" ]; then
        mkdir -p build
        cd build
        cmake .. && make
        cd "$PROJECT_ROOT"
        
        # Python 패키지 설치
        pip install -e .
        echo "✅ C++ 모듈 빌드 및 설치 완료"
    else
        echo "⚠️ CMakeLists.txt를 찾을 수 없습니다. C++ 모듈 빌드를 건너뜁니다."
    fi
else
    echo "⚠️ CMake가 설치되어 있지 않습니다. C++ 모듈 빌드를 건너뜁니다."
    echo "   기본 분석 기능만 사용 가능합니다."
fi

echo ""
echo "🎉 설정 완료!"
echo ""
echo "가상환경 활성화:"
echo "  source venv/bin/activate"
echo ""
echo "CLI 사용:"
echo "  code-educator ask \"파이썬이란?\""
echo "  code-educator analyze test.py"
echo ""
echo "웹 서버 실행:"
echo "  uvicorn srcs.python.server:app --reload"
echo ""
echo "가상환경 비활성화:"
echo "  deactivate"