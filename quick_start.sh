#!/bin/bash

# 색상 정의
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🚀 Code Educator 빠른 시작${NC}"
echo "=========================="

# 1. Docker 확인
if ! command -v docker &> /dev/null; then
    echo -e "${RED}❌ Docker가 설치되어 있지 않습니다.${NC}"
    echo "Docker를 먼저 설치해주세요: https://docs.docker.com/get-docker/"
    exit 1
fi

# 2. Docker Compose 확인
if ! command -v docker-compose &> /dev/null; then
    echo -e "${YELLOW}⚠️  docker-compose가 없습니다. docker compose를 사용합니다.${NC}"
    DOCKER_COMPOSE="docker compose"
else
    DOCKER_COMPOSE="docker-compose"
fi

# 3. 기존 컨테이너 정리
echo -e "${YELLOW}🧹 기존 컨테이너 정리 중...${NC}"
$DOCKER_COMPOSE down 2>/dev/null

# 4. 서비스 시작
echo -e "${GREEN}🔧 서비스 시작 중...${NC}"
$DOCKER_COMPOSE up -d

# 5. 대기
echo -e "${BLUE}⏳ 서비스가 준비될 때까지 대기 중...${NC}"
sleep 10

# 6. 모델 다운로드 확인
echo -e "${BLUE}📥 CodeLlama 모델 다운로드 중...${NC}"
docker exec code_educator_ollama ollama pull codellama &

# 7. 상태 확인
echo -e "${GREEN}📊 서비스 상태 확인${NC}"
echo "=========================="

# API 헬스체크
if curl -s http://localhost:8000/health > /dev/null 2>&1; then
    echo -e "${GREEN}✅ FastAPI 서버: http://localhost:8000${NC}"
    echo -e "${GREEN}✅ API 문서: http://localhost:8000/docs${NC}"
else
    echo -e "${RED}❌ FastAPI 서버가 응답하지 않습니다${NC}"
fi

# Ollama 체크
if curl -s http://localhost:11434/api/tags > /dev/null 2>&1; then
    echo -e "${GREEN}✅ Ollama API: http://localhost:11434${NC}"
else
    echo -e "${YELLOW}⚠️  Ollama가 아직 준비 중입니다${NC}"
fi

# 프론트엔드 체크
if curl -s http://localhost > /dev/null 2>&1; then
    echo -e "${GREEN}✅ 프론트엔드: http://localhost${NC}"
else
    echo -e "${YELLOW}⚠️  프론트엔드가 아직 준비 중입니다${NC}"
fi

echo ""
echo -e "${GREEN}🎉 서버가 실행 중입니다!${NC}"
echo ""
echo "📝 사용 방법:"
echo "  - API 테스트: http://localhost:8000/docs"
echo "  - 프론트엔드: http://localhost"
echo "  - 로그 보기: $DOCKER_COMPOSE logs -f"
echo "  - 중지하기: $DOCKER_COMPOSE down"
echo ""
echo -e "${YELLOW}💡 팁: 모델 다운로드는 백그라운드에서 진행됩니다${NC}"
echo -e "${YELLOW}   첫 실행 시 5-10분 정도 걸릴 수 있습니다${NC}"