# srcs/python/models/schemas.py
from pydantic import BaseModel, Field
from typing import List, Dict, Optional, Any

class QuestionRequest(BaseModel):
    question: str = Field(..., description="AI에게 물어볼 질문")
    model: str = Field(default="codellama", description="사용할 AI 모델")
    language: Optional[str] = Field(None, description="프로그래밍 언어 (코딩 질문인 경우)")
    context: Optional[str] = Field(None, description="추가 컨텍스트")

class QuestionResponse(BaseModel):
    response: str = Field(..., description="AI 응답")
    model_used: str = Field(..., description="사용된 모델")

class CodeExplainRequest(BaseModel):
    code: str = Field(..., description="설명할 코드")
    language: Optional[str] = Field(None, description="프로그래밍 언어")
    model: str = Field(default="codellama", description="사용할 AI 모델")

class CodeGenerateRequest(BaseModel):
    description: str = Field(..., description="생성할 코드에 대한 설명")
    language: str = Field(..., description="프로그래밍 언어")
    model: str = Field(default="codellama", description="사용할 AI 모델")

class DebugRequest(BaseModel):
    code: str = Field(..., description="디버깅할 코드")
    error_message: Optional[str] = Field(None, description="오류 메시지")
    language: Optional[str] = Field(None, description="프로그래밍 언어")
    model: str = Field(default="codellama", description="사용할 AI 모델")

class AnalyzeRequest(BaseModel):
    code: str = Field(..., description="분석할 코드")
    language: Optional[str] = Field(None, description="프로그래밍 언어 (자동 감지 가능)")
    ai_analysis: bool = Field(default=False, description="AI 분석 포함 여부")
    model: str = Field(default="codellama", description="AI 분석용 모델")

class AnalyzeResponse(BaseModel):
    # 기본 정보
    language: str = Field(..., description="감지된 프로그래밍 언어")
    complexity: int = Field(..., description="코드 복잡도")
    
    # 구조 정보
    imports: List[str] = Field(..., description="import/include 문들")
    functions: List[str] = Field(..., description="함수명 목록")
    classes: List[str] = Field(..., description="클래스명 목록")
    
    # 메트릭스
    line_count: int = Field(..., description="코드 라인 수")
    comment_count: int = Field(..., description="주석 수")
    comment_ratio: float = Field(..., description="주석 비율")
    nesting_depth: int = Field(..., description="중첩 깊이")
    cyclomatic_complexity: int = Field(..., description="순환 복잡도")
    
    # 분석 결과
    potential_issues: List[str] = Field(..., description="잠재적 문제점들")
    suggestions: List[str] = Field(..., description="개선 제안들")
    quality_score: int = Field(..., description="품질 점수 (0-100)")
    
    # 추가 정보
    metadata: Dict[str, Any] = Field(default_factory=dict, description="추가 메타데이터")
    ai_analysis: Optional[str] = Field(None, description="AI 분석 결과")
    
    # 파일 정보 (파일 업로드 시)
    file_path: Optional[str] = Field(None, description="파일 경로")
    file_name: Optional[str] = Field(None, description="파일명")

class ModelInfo(BaseModel):
    name: str = Field(..., description="모델명")
    size: Optional[str] = Field(None, description="모델 크기")
    modified_at: Optional[str] = Field(None, description="수정 시간")

class ModelsResponse(BaseModel):
    models: List[ModelInfo] = Field(..., description="사용 가능한 모델 목록")

class HealthResponse(BaseModel):
    status: str = Field(..., description="서버 상태")
    ollama_connected: bool = Field(..., description="Ollama 연결 상태")
    core_module: bool = Field(..., description="C++ 코어 모듈 사용 가능 여부")
    supported_languages: List[str] = Field(..., description="지원하는 언어 목록")
    features: Dict[str, bool] = Field(..., description="사용 가능한 기능들")

class ErrorResponse(BaseModel):
    error: str = Field(..., description="오류 메시지")
    detail: Optional[str] = Field(None, description="상세 오류 정보")
    code: Optional[str] = Field(None, description="오류 코드")

# CLI 응답용 모델들 (기존 CLI와의 호환성)
class CLIAnalysisResult(BaseModel):
    """CLI 출력과 호환되는 분석 결과"""
    file_path: Optional[str] = None
    language: str
    complexity: int
    imports: List[str]
    functions: List[str]
    classes: List[str]
    line_count: int
    comment_count: int
    comment_ratio: float
    nesting_depth: int
    cyclomatic_complexity: int
    potential_issues: List[str]
    suggestions: List[str]
    quality_score: int
    ai_analysis: Optional[str] = None

    def to_text_format(self) -> str:
        """CLI 형태의 텍스트 출력"""
        output = []
        if self.file_path:
            output.append(f"분석 파일: {self.file_path}")
        
        output.extend([
            f"언어: {self.language}",
            f"복잡도: {self.complexity}",
            f"라인 수: {self.line_count}",
            f"주석 수: {self.comment_count} (비율: {self.comment_ratio:.2f})",
            f"중첩 깊이: {self.nesting_depth}",
            f"순환 복잡도: {self.cyclomatic_complexity}",
            f"품질 점수: {self.quality_score}/100"
        ])
        
        if self.functions:
            output.append(f"함수: {', '.join(self.functions[:5])}")
            if len(self.functions) > 5:
                output.append(f"    (+{len(self.functions) - 5}개 더)")
        
        if self.potential_issues:
            output.append("잠재적 문제점:")
            for issue in self.potential_issues:
                output.append(f"  • {issue}")
        
        if self.suggestions:
            output.append("개선 제안:")
            for suggestion in self.suggestions:
                output.append(f"  • {suggestion}")
                
        if self.ai_analysis:
            output.append(f"AI 분석:\n{self.ai_analysis}")
        
        return "\n".join(output)