# srcs/python/server.py
from fastapi import FastAPI, HTTPException, UploadFile, File, Depends
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import StreamingResponse
import uvicorn
import json
import io
from typing import Optional, List

from .services.ai_service import AIService
from .services.code_service import CodeAnalysisService
from .models.schemas import (
    QuestionRequest, QuestionResponse, CodeExplainRequest, CodeGenerateRequest,
    DebugRequest, AnalyzeRequest, AnalyzeResponse, ModelsResponse, HealthResponse,
    ErrorResponse, ModelInfo  # Mpython 제거하고 ModelInfo 추가
)

# FastAPI 앱 인스턴스 생성 (누락되어 있었음)
app = FastAPI(
    title="Code Educator API",
    description="AI 기반 코드 교육 API",
    version="0.1.0"
)

# CORS 미들웨어 추가
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # 개발용, 프로덕션에서는 구체적인 도메인 지정
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# 서비스 인스턴스들
ai_service = AIService()
code_service = CodeAnalysisService()

# 의존성 함수들
def get_ai_service() -> AIService:
    return ai_service

def get_code_service() -> CodeAnalysisService:
    return code_service

@app.get("/", response_model=dict)
async def root():
    """API 루트 엔드포인트"""
    return {
        "message": "Code Educator API",
        "version": "0.1.0",
        "docs": "/docs",
        "redoc": "/redoc"
    }

@app.get("/health", response_model=HealthResponse)
async def health_check(
    ai_svc: AIService = Depends(get_ai_service),
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """서버 및 서비스 상태 체크"""
    ai_status = ai_svc.check_ai_status()
    analysis_stats = code_svc.get_analysis_stats()
    
    return HealthResponse(
        status="healthy",
        ollama_connected=ai_status["connected"],
        core_module=analysis_stats["core_module_available"],
        supported_languages=analysis_stats["supported_languages"],
        features=analysis_stats["features"]
    )

@app.get("/models", response_model=ModelsResponse)
async def get_models(ai_svc: AIService = Depends(get_ai_service)):
    """사용 가능한 AI 모델 목록"""
    try:
        models = ai_svc.get_available_models()
        model_info_list = [
            ModelInfo(
                name=model.get("name", ""),
                size=model.get("size"),
                modified_at=model.get("modified_at")
            )
            for model in models
        ]
        return ModelsResponse(models=model_info_list)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# AI 관련 엔드포인트들
@app.post("/ask", response_model=QuestionResponse)
async def ask_question(
    request: QuestionRequest,
    ai_svc: AIService = Depends(get_ai_service)
):
    """AI에게 일반적인 질문하기"""
    try:
        if request.language:
            response = ai_svc.get_coding_help(
                request.question, 
                request.language, 
                request.model
            )
        else:
            response = ai_svc.ask_question(
                request.question, 
                request.model, 
                request.context
            )
        
        return QuestionResponse(response=response, model_used=request.model)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/explain", response_model=QuestionResponse)
async def explain_code(
    request: CodeExplainRequest,
    ai_svc: AIService = Depends(get_ai_service)
):
    """코드 설명 요청"""
    try:
        response = ai_svc.explain_code(
            request.code, 
            request.language, 
            request.model
        )
        return QuestionResponse(response=response, model_used=request.model)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/generate", response_model=QuestionResponse)
async def generate_code(
    request: CodeGenerateRequest,
    ai_svc: AIService = Depends(get_ai_service)
):
    """코드 생성 요청"""
    try:
        response = ai_svc.generate_code(
            request.description, 
            request.language, 
            request.model
        )
        return QuestionResponse(response=response, model_used=request.model)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/debug", response_model=QuestionResponse)
async def debug_help(
    request: DebugRequest,
    ai_svc: AIService = Depends(get_ai_service)
):
    """디버깅 도움 요청"""
    try:
        response = ai_svc.debug_help(
            request.code, 
            request.error_message, 
            request.language, 
            request.model
        )
        return QuestionResponse(response=response, model_used=request.model)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# 코드 분석 관련 엔드포인트들
@app.post("/analyze", response_model=AnalyzeResponse)
async def analyze_code(
    request: AnalyzeRequest,
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """코드 텍스트 분석"""
    try:
        result = code_svc.analyze_code(
            request.code,
            request.ai_analysis,
            request.model
        )
        return AnalyzeResponse(**result)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/analyze/file", response_model=AnalyzeResponse)
async def analyze_file(
    file: UploadFile = File(...),
    ai_analysis: bool = False,
    model: str = "codellama",
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """파일 업로드해서 코드 분석"""
    try:
        # 파일 내용 읽기
        content = await file.read()
        
        # 인코딩 시도 (UTF-8 우선, 실패 시 다른 인코딩)
        try:
            code = content.decode('utf-8')
        except UnicodeDecodeError:
            try:
                code = content.decode('latin-1')
            except UnicodeDecodeError:
                raise HTTPException(
                    status_code=400, 
                    detail="파일 인코딩을 감지할 수 없습니다. UTF-8 파일을 사용해주세요."
                )
        
        # 코드 분석
        result = code_svc.analyze_code(code, ai_analysis, model)
        result['file_name'] = file.filename
        
        return AnalyzeResponse(**result)
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/analyze/quality/{threshold}")
async def check_quality(
    threshold: int,
    code: str,
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """코드 품질 체크 (CI/CD용)"""
    try:
        result = code_svc.analyze_code(code, False)
        score = result['quality_score']
        
        return {
            "score": score,
            "threshold": threshold,
            "passed": score >= threshold,
            "issues": result['potential_issues'] if score < threshold else [],
            "suggestions": result['suggestions'] if score < threshold else []
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

# 유틸리티 엔드포인트들
@app.get("/languages")
async def get_supported_languages(
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """지원하는 프로그래밍 언어 목록"""
    return {
        "languages": code_svc.get_supported_languages(),
        "core_module_available": code_svc.has_core
    }

@app.get("/stats")
async def get_system_stats(
    ai_svc: AIService = Depends(get_ai_service),
    code_svc: CodeAnalysisService = Depends(get_code_service)
):
    """시스템 통계 정보"""
    ai_status = ai_svc.check_ai_status()
    analysis_stats = code_svc.get_analysis_stats()
    
    return {
        "ai_service": ai_status,
        "code_analysis": analysis_stats,
        "system": {
            "api_version": "0.1.0",
            "endpoints": len(app.routes)
        }
    }

# 예외 처리기들
@app.exception_handler(ConnectionError)
async def connection_error_handler(request, exc):
    return ErrorResponse(
        error="연결 오류",
        detail="Ollama API에 연결할 수 없습니다. 서버가 실행 중인지 확인해주세요.",
        code="CONNECTION_ERROR"
    )

@app.exception_handler(ValueError)
async def value_error_handler(request, exc):
    return ErrorResponse(
        error="값 오류",
        detail=str(exc),
        code="VALUE_ERROR"
    )

# 개발 모드에서만 사용할 디버그 엔드포인트들
@app.get("/debug/routes")
async def debug_routes():
    """디버그: 사용 가능한 모든 라우트 목록"""
    routes = []
    for route in app.routes:
        if hasattr(route, 'methods'):
            routes.append({
                "path": route.path,
                "methods": list(route.methods),
                "name": route.name
            })
    return {"routes": routes}

if __name__ == "__main__":
    import os
    
    # 환경 변수로 설정 가능
    host = os.environ.get("HOST", "0.0.0.0")
    port = int(os.environ.get("PORT", 8000))
    debug = os.environ.get("DEBUG", "false").lower() == "true"
    
    uvicorn.run(
        "srcs.python.server:app" if not debug else app,
        host=host,
        port=port,
        reload=debug,
        log_level="info"
    )