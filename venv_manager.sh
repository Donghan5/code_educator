#!/bin/bash
# venv_manager.sh - 가상환경 관리 유틸리티

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
VENV_DIR="$PROJECT_ROOT/venv"

# 색상 정의
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 로그 함수들
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

# 가상환경 상태 확인
check_venv() {
    if [ -d "$VENV_DIR" ]; then
        log_success "가상환경 존재: $VENV_DIR"
        
        # 활성화 상태 확인
        if [[ "$VIRTUAL_ENV" == "$VENV_DIR" ]]; then
            log_success "가상환경 활성화됨"
        else
            log_warning "가상환경이 비활성화 상태입니다"
            echo "활성화 방법: source venv/bin/activate"
        fi
    else
        log_error "가상환경이 존재하지 않습니다"
        echo "생성 방법: $0 create"
        return 1
    fi
}

# 가상환경 생성
create_venv() {
    log_info "가상환경 생성 중..."
    
    if [ -d "$VENV_DIR" ]; then
        log_warning "가상환경이 이미 존재합니다"
        read -p "삭제하고 다시 생성하시겠습니까? (y/N): " confirm
        if [[ $confirm == [yY] ]]; then
            rm -rf "$VENV_DIR"
        else
            log_info "작업이 취소되었습니다"
            return 0
        fi
    fi
    
    python3 -m venv "$VENV_DIR"
    log_success "가상환경 생성 완료"
    
    log_info "가상환경 활성화 및 의존성 설치 중..."
    source "$VENV_DIR/bin/activate"
    pip install --upgrade pip
    
    if [ -f "$PROJECT_ROOT/requirements.txt" ]; then
        pip install -r "$PROJECT_ROOT/requirements.txt"
    fi
    
    log_success "설정 완료"
}

# 가상환경 삭제
remove_venv() {
    if [ -d "$VENV_DIR" ]; then
        read -p "가상환경을 삭제하시겠습니까? (y/N): " confirm
        if [[ $confirm == [yY] ]]; then
            rm -rf "$VENV_DIR"
            log_success "가상환경 삭제 완료"
        else
            log_info "작업이 취소되었습니다"
        fi
    else
        log_warning "삭제할 가상환경이 없습니다"
    fi
}

# 의존성 설치
install_deps() {
    if [ ! -d "$VENV_DIR" ]; then
        log_error "가상환경이 존재하지 않습니다. 먼저 생성해주세요."
        return 1
    fi
    
    if [[ "$VIRTUAL_ENV" != "$VENV_DIR" ]]; then
        log_warning "가상환경을 활성화하고 다시 시도해주세요"
        echo "활성화: source venv/bin/activate"
        return 1
    fi
    
    log_info "의존성 설치 중..."
    pip install --upgrade pip
    
    if [ -f "$PROJECT_ROOT/requirements.txt" ]; then
        pip install -r "$PROJECT_ROOT/requirements.txt"
    fi
    
    log_success "의존성 설치 완료"
}

# 프로젝트 빌드
build_project() {
    if [[ "$VIRTUAL_ENV" != "$VENV_DIR" ]]; then
        log_warning "가상환경을 활성화하고 다시 시도해주세요"
        return 1
    fi
    
    log_info "프로젝트 빌드 중..."
    cd "$PROJECT_ROOT"
    
    # C++ 모듈 빌드
    if command -v cmake &> /dev/null && [ -f "CMakeLists.txt" ]; then
        mkdir -p build
        cd build
        cmake .. && make
        cd "$PROJECT_ROOT"
        pip install -e .
        log_success "C++ 모듈 빌드 완료"
    else
        log_warning "CMake 또는 CMakeLists.txt가 없습니다. Python만 설치합니다."
        pip install -e .
    fi
    
    log_success "프로젝트 빌드 완료"
}

# 개발 서버 실행
run_server() {
    if [[ "$VIRTUAL_ENV" != "$VENV_DIR" ]]; then
        log_warning "가상환경을 활성화하고 다시 시도해주세요"
        return 1
    fi
    
    log_info "개발 서버 시작 중..."
    cd "$PROJECT_ROOT"
    uvicorn srcs.python.server:app --reload --host 0.0.0.0 --port 8000
}

# CLI 테스트
test_cli() {
    if [[ "$VIRTUAL_ENV" != "$VENV_DIR" ]]; then
        log_warning "가상환경을 활성화하고 다시 시도해주세요"
        return 1
    fi
    
    log_info "CLI 테스트 중..."
    code-educator check
}

# 패키지 정보 표시
show_info() {
    echo "🐍 Code Educator 가상환경 정보"
    echo "================================"
    echo "프로젝트 경로: $PROJECT_ROOT"
    echo "가상환경 경로: $VENV_DIR"
    echo "현재 활성화: ${VIRTUAL_ENV:-"없음"}"
    echo ""
    
    if [ -d "$VENV_DIR" ]; then
        echo "설치된 패키지:"
        if [[ "$VIRTUAL_ENV" == "$VENV_DIR" ]]; then
            pip list | grep -E "(code-educator|fastapi|click|requests)"
        else
            echo "  (가상환경을 활성화하면 확인 가능)"
        fi
    fi
}

# 도움말
show_help() {
    echo "Code Educator 가상환경 관리 도구"
    echo ""
    echo "사용법: $0 [명령어]"
    echo ""
    echo "명령어:"
    echo "  create    - 가상환경 생성 및 설정"
    echo "  remove    - 가상환경 삭제"
    echo "  check     - 가상환경 상태 확인"
    echo "  install   - 의존성 설치"
    echo "  build     - 프로젝트 빌드"
    echo "  server    - 개발 서버 실행"
    echo "  test      - CLI 테스트"
    echo "  info      - 환경 정보 표시"
    echo "  help      - 이 도움말 표시"
    echo ""
    echo "예시:"
    echo "  $0 create          # 처음 설정"
    echo "  source venv/bin/activate  # 가상환경 활성화"
    echo "  $0 build           # 프로젝트 빌드"
    echo "  $0 server          # 서버 실행"
}

# 메인 로직
case "${1:-help}" in
    create)
        create_venv
        ;;
    remove)
        remove_venv
        ;;
    check)
        check_venv
        ;;
    install)
        install_deps
        ;;
    build)
        build_project
        ;;
    server)
        run_server
        ;;
    test)
        test_cli
        ;;
    info)
        show_info
        ;;
    help|*)
        show_help
        ;;
esac