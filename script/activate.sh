#!/bin/bash
# activate.sh - 간편한 가상환경 활성화 스크립트

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENV_DIR="$SCRIPT_DIR/venv"

# 색상 정의
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

if [ ! -d "$VENV_DIR" ]; then
    echo -e "${RED}❌ 가상환경이 존재하지 않습니다.${NC}"
    echo "먼저 가상환경을 생성해주세요:"
    echo "  ./setup_venv.sh"
    echo "  또는"
    echo "  ./venv_manager.sh create"
    exit 1
fi

# .env 파일이 있으면 로드
if [ -f "$SCRIPT_DIR/.env" ]; then
    echo -e "${GREEN}📋 환경변수 로드 중...${NC}"
    source "$SCRIPT_DIR/.env"
fi

echo -e "${GREEN}🐍 가상환경 활성화 중...${NC}"
source "$VENV_DIR/bin/activate"

echo -e "${GREEN}✅ 가상환경 활성화 완료!${NC}"
echo ""
echo "💡 사용 가능한 명령어들:"
echo "  code-educator ask \"질문\""
echo "  code-educator analyze 파일.py"
echo "  code-educator check"
echo "  uvicorn srcs.python.server:app --reload"
echo ""
echo "🚪 가상환경 비활성화: deactivate"

# 프롬프트 변경을 위한 새 셸 시작
exec $SHELL