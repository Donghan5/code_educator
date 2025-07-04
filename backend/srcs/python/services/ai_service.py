# srcs/python/services/ai_service.py
from typing import Dict, List, Optional, Any
from ..api import OllamaAPI

class AIService:
    """AI 관련 비즈니스 로직을 처리하는 서비스"""
    
    def __init__(self):
        self.default_model = "codellama"

    def ask_question(self, question: str, model: str = None, 
                    context: Optional[str] = None) -> str:
        """
        AI에게 질문하기
        """
        model = model or self.default_model
        api = OllamaAPI(model=model)
        
        if not api.check_connection():
            raise ConnectionError("Ollama API에 연결할 수 없습니다.")
        
        # 컨텍스트가 있으면 질문에 포함
        if context:
            enhanced_question = f"""컨텍스트:
{context}

질문: {question}

위 컨텍스트를 참고해서 한국어로 답변해주세요."""
        else:
            enhanced_question = f"{question}\n\n한국어로 답변해주세요."
        
        try:
            return api.generate_async(enhanced_question)
        except Exception as e:
            raise Exception(f"AI 응답 생성 중 오류: {str(e)}")

    def get_coding_help(self, question: str, language: str = None, 
                       model: str = None) -> str:
        """
        프로그래밍 관련 도움 요청
        """
        model = model or self.default_model
        
        # 프로그래밍 특화 프롬프트
        if language:
            prompt = f"""당신은 {language} 프로그래밍 전문가입니다. 다음 질문에 대해 자세하고 실용적인 답변을 한국어로 해주세요:

{question}

답변 시 다음을 포함해주세요:
1. 명확한 설명
2. 실제 코드 예시 (가능한 경우)
3. 베스트 프랙티스
4. 주의사항이나 팁"""
        else:
            prompt = f"""당신은 프로그래밍 전문가입니다. 다음 질문에 대해 자세하고 실용적인 답변을 한국어로 해주세요:

{question}

답변 시 다음을 포함해주세요:
1. 명확한 설명  
2. 실제 코드 예시 (가능한 경우)
3. 베스트 프랙티스
4. 주의사항이나 팁"""
        
        return self.ask_question(prompt, model)

    def explain_code(self, code: str, language: str = None, 
                    model: str = None) -> str:
        """
        코드 설명 요청
        """
        model = model or self.default_model
        
        prompt = f"""다음 {language or ''}코드를 한국어로 자세히 설명해주세요:

```{language or ''}
{code}
```

다음 내용을 포함해서 설명해주세요:
1. 전체적인 기능과 목적
2. 주요 로직과 알고리즘
3. 각 부분의 역할
4. 사용된 기법이나 패턴
5. 개선 가능한 점이 있다면 제안"""

        return self.ask_question(prompt, model)

    def debug_help(self, code: str, error_message: str = None, 
                  language: str = None, model: str = None) -> str:
        """
        디버깅 도움 요청
        """
        model = model or self.default_model
        
        if error_message:
            prompt = f"""다음 {language or ''}코드에서 오류가 발생했습니다. 문제를 찾고 해결 방법을 한국어로 알려주세요:

코드:
```{language or ''}
{code}
```

오류 메시지:
```
{error_message}
```

다음 내용을 포함해서 답변해주세요:
1. 오류의 원인 분석
2. 구체적인 해결 방법
3. 수정된 코드 예시
4. 비슷한 오류를 방지하는 팁"""
        else:
            prompt = f"""다음 {language or ''}코드를 검토하고 잠재적인 문제점이나 개선점을 한국어로 알려주세요:

```{language or ''}
{code}
```

다음 관점에서 검토해주세요:
1. 문법적 오류
2. 논리적 오류
3. 성능 문제
4. 보안 이슈
5. 코드 품질 개선점"""

        return self.ask_question(prompt, model)

    def get_available_models(self) -> List[Dict[str, Any]]:
        """
        사용 가능한 모델 목록 조회
        """
        try:
            api = OllamaAPI()
            return api.list_models()
        except Exception as e:
            raise Exception(f"모델 목록 조회 중 오류: {str(e)}")

    def check_ai_status(self) -> Dict[str, Any]:
        """
        AI 서비스 상태 확인
        """
        api = OllamaAPI()
        is_connected = api.check_connection()
        
        status = {
            "connected": is_connected,
            "default_model": self.default_model,
            "available_models": []
        }
        
        if is_connected:
            try:
                status["available_models"] = api.list_models()
            except:
                pass
                
        return status

    def generate_code(self, description: str, language: str, 
                     model: str = None) -> str:
        """
        설명을 바탕으로 코드 생성
        """
        model = model or self.default_model
        
        prompt = f"""다음 설명에 맞는 {language} 코드를 생성해주세요:

요구사항: {description}

다음 형식으로 답변해주세요:
1. 코드 설명 (한국어)
2. 완전한 실행 가능한 코드
3. 사용 방법 예시
4. 추가 개선 사항 제안

코드는 명확하고 읽기 쉽게 작성하고, 적절한 주석을 포함해주세요."""

        return self.ask_question(prompt, model)