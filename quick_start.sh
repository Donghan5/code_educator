#!/usr/bin/env bash

# ───────────────────────── 색상 ──────────────────────────
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🚀 Code Educator 빠른 시작${NC}"
echo "==============================================="

# ───────────────────── 전제 조건 체크 ────────────────────
need_cmd () {
  command -v "$1" &>/dev/null || {
    echo -e "${RED}❌ '$1' 없어서 진행 불가${NC}"
    exit 1
  }
}

need_cmd docker
need_cmd python3
need_cmd g++
need_cmd cmake
need_cmd make

# ───────────────────── C++ 코어 모듈 빌드 ────────────────
echo -e "${BLUE}🔨 C++ 코어 모듈 빌드 중...${NC}"

EXT_SUFFIX=$(python3 - <<'PY'
import sysconfig, pathlib, sys
print(sysconfig.get_config_var("EXT_SUFFIX") or ".so")
PY
)

OUT_DIR="build"
OUT_FILE="${OUT_DIR}/code_educator_core${EXT_SUFFIX}"

if [[ -f "$OUT_FILE" ]]; then
  echo -e "${GREEN}✅ 이미 빌드됨 → ${OUT_FILE}${NC}"
else
  mkdir -p "$OUT_DIR"                       || { echo -e "${RED}mkdir 실패${NC}"; exit 1; }
  pushd "$OUT_DIR" >/dev/null               || exit 1
  cmake -S .. -B .                                  || { echo -e "${RED}cmake 실패${NC}"; exit 1; }
  make -j"$(nproc)"                          || { echo -e "${RED}make 실패${NC}"; exit 1; }
  popd >/dev/null
  echo -e "${GREEN}✅ 빌드 완료 → ${OUT_FILE}${NC}"
fi

# ───────────────────── Docker Compose 준비 ───────────────
if command -v docker-compose &>/dev/null; then
  DOCKER_COMPOSE="docker-compose"
else
  DOCKER_COMPOSE="docker compose"
  echo -e "${YELLOW}⚠️  docker-compose 명령이 없어서 'docker compose'로 대체${NC}"
fi

echo -e "${YELLOW}🧹 기존 컨테이너 정리 중...${NC}"
$DOCKER_COMPOSE down &>/dev/null

echo -e "${GREEN}🔧 Docker 서비스 시작 중...${NC}"
$DOCKER_COMPOSE up -d

# ───────────────────── 서비스 헬스체크 ───────────────────
echo -e "${BLUE}⏳ 서비스 준비 대기 (최대 10초)...${NC}"
for i in {1..10}; do
  curl -s http://localhost:8000/health &>/dev/null && break
  sleep 1
done

echo "-----------------------------------------------"
[[ $(curl -s -o /dev/null -w '%{http_code}' http://localhost:8000/health) == "200" ]] \
  && echo -e "${GREEN}✅ FastAPI 서버 OK → http://localhost:8000${NC}" \
  || echo -e "${RED}❌ FastAPI 서버 응답 없음${NC}"

[[ $(curl -s -o /dev/null -w '%{http_code}' http://localhost) == "200" ]] \
  && echo -e "${GREEN}✅ 프론트엔드 OK → http://localhost${NC}" \
  || echo -e "${YELLOW}⚠️  프론트엔드 준비 중${NC}"

[[ $(curl -s http://localhost:11434/api/tags &>/dev/null; echo $? ) == 0 ]] \
  && echo -e "${GREEN}✅ Ollama API OK → http://localhost:11434${NC}" \
  || echo -e "${YELLOW}⚠️  Ollama 준비 중${NC}"
echo "-----------------------------------------------"

# ───────────────────── 모델 다운로드 백그라운드 ───────────
echo -e "${BLUE}📥 CodeLlama 모델(Pull) 시작...${NC}"
docker exec code_educator_ollama ollama pull codellama &>/dev/null &

echo -e "${GREEN}🎉 서버가 실행 중이야!${NC}"
echo ""
echo "📝 사용 팁"
echo "  • API Docs  : http://localhost:8000/docs"
echo "  • Frontend  : http://localhost"
echo "  • 로그 보기: $DOCKER_COMPOSE logs -f"
echo "  • 중지     : $DOCKER_COMPOSE down"
echo ""
echo -e "${YELLOW}💡 첫 실행은 모델 다운로드 때문에 몇 분 걸릴 수 있어${NC}"
