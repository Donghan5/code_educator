# srcs/python/models/__init__.py
"""
Code Educator Models Package

이 패키지는 데이터 모델과 스키마를 포함합니다.
"""

from .schemas import (
    QuestionRequest, QuestionResponse,
    CodeExplainRequest, CodeGenerateRequest, DebugRequest,
    AnalyzeRequest, AnalyzeResponse,
    ModelInfo, ModelsResponse,
    HealthResponse, ErrorResponse,
    CLIAnalysisResult
)

__all__ = [
    'QuestionRequest', 'QuestionResponse',
    'CodeExplainRequest', 'CodeGenerateRequest', 'DebugRequest',
    'AnalyzeRequest', 'AnalyzeResponse',
    'ModelInfo', 'ModelsResponse',
    'HealthResponse', 'ErrorResponse',
    'CLIAnalysisResult'
]