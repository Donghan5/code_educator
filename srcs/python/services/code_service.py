# srcs/python/services/code_service.py
import os
from typing import Dict, List, Optional, Any
from ..api import OllamaAPI

# C++ 모듈 가져오기
try:
    import code_educator_core as ce
    HAS_CORE = True
except ImportError:
    HAS_CORE = False

class CodeAnalysisService:
    """코드 분석 관련 비즈니스 로직을 처리하는 서비스"""
    
    def __init__(self):
        self.has_core = HAS_CORE
        if self.has_core:
            self.parser = ce.CodeParser()
            self.analyzer = ce.Analyzer()

    def analyze_code(self, code: str, include_ai: bool = False, 
                    ai_model: str = "codellama") -> Dict[str, Any]:
        """
        코드 분석 실행
        """
        if not self.has_core:
            return self._basic_analysis(code)
        
        try:
            # C++ 코어 모듈로 분석
            structure = self.parser.parse(code)
            analysis = self.analyzer.analyze(code)
            quality_score = self.analyzer.calculate_quality(analysis)
            
            result = {
                "language": structure.language,
                "complexity": structure.complexity,
                "imports": list(structure.imports),
                "functions": list(structure.functions),
                "classes": list(structure.classes),
                "line_count": analysis.lineCount,
                "comment_count": analysis.commentCount,
                "comment_ratio": analysis.commentRatio,
                "nesting_depth": analysis.nestingLength,
                "cyclomatic_complexity": analysis.cyclomaticComplexity,
                "potential_issues": list(analysis.potentialIssues),
                "suggestions": list(analysis.suggestions),
                "quality_score": quality_score,
                "metadata": dict(analysis.metadata)
            }
            
            # AI 분석 추가
            if include_ai:
                ai_analysis = self._get_ai_analysis(code, structure.language, ai_model)
                result["ai_analysis"] = ai_analysis
                
            return result
            
        except Exception as e:
            raise Exception(f"코드 분석 중 오류 발생: {str(e)}")

    def analyze_file(self, file_path: str, include_ai: bool = False, 
                    ai_model: str = "codellama") -> Dict[str, Any]:
        """
        파일 분석
        """
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                code = f.read()
            
            result = self.analyze_code(code, include_ai, ai_model)
            result["file_path"] = file_path
            result["file_name"] = os.path.basename(file_path)
            
            return result
            
        except Exception as e:
            raise Exception(f"파일 분석 중 오류 발생: {str(e)}")

    def _basic_analysis(self, code: str) -> Dict[str, Any]:
        """
        C++ 모듈이 없을 때 기본 분석
        """
        lines = code.splitlines()
        line_count = len([line for line in lines if line.strip()])
        
        # 간단한 언어 감지
        language = self._detect_language_simple(code)
        
        return {
            "language": language,
            "complexity": len(code) // 100,  # 간단한 복잡도
            "imports": [],
            "functions": [],
            "classes": [],
            "line_count": line_count,
            "comment_count": 0,
            "comment_ratio": 0.0,
            "nesting_depth": 0,
            "cyclomatic_complexity": 1,
            "potential_issues": ["C++ 코어 모듈을 사용할 수 없어 기본 분석만 가능합니다."],
            "suggestions": ["C++ 모듈을 빌드하면 더 자세한 분석이 가능합니다."],
            "quality_score": 50,  # 기본값
            "metadata": {}
        }

    def _detect_language_simple(self, code: str) -> str:
        """간단한 언어 감지"""
        if "def " in code or "import " in code:
            return "python"
        elif "#include" in code or "std::" in code:
            return "cpp"
        elif "function " in code or "const " in code or "=>" in code:
            return "javascript"
        elif "printf" in code and "#include" in code:
            return "c"
        return "unknown"

    def _get_ai_analysis(self, code: str, language: str, model: str) -> Optional[str]:
        """AI 분석 요청"""
        try:
            api = OllamaAPI(model=model)
            if not api.check_connection():
                return None
                
            prompt = self._create_analysis_prompt(code, language)
            return api.generate_async(prompt)
            
        except Exception as e:
            print(f"AI 분석 중 오류: {e}")
            return None

    def _create_analysis_prompt(self, code: str, language: str) -> str:
        """AI 분석용 프롬프트 생성"""
        # 코드가 너무 길면 자르기
        code_sample = code[:4000] if len(code) > 4000 else code
        
        prompt = f"""다음 {language} 코드를 분석하고 개선점을 한국어로 제안해주세요:

```{language}
{code_sample}
```

다음 관점에서 분석해주세요:
1. 코드 품질과 가독성
2. 성능 최적화 가능성
3. 보안 이슈
4. 베스트 프랙티스 준수
5. 구체적인 개선 제안

{f"참고: 이 코드는 {len(code) - 4000}자가 더 있지만 길이 제한으로 잘렸습니다." if len(code) > 4000 else ""}
"""
        return prompt

    def get_supported_languages(self) -> List[str]:
        """지원하는 언어 목록"""
        if self.has_core:
            return ["python", "cpp", "c", "javascript"]
        else:
            return ["python", "cpp", "c", "javascript"]  # 기본 감지는 여전히 가능

    def get_analysis_stats(self) -> Dict[str, Any]:
        """분석 통계 정보"""
        return {
            "core_module_available": self.has_core,
            "supported_languages": self.get_supported_languages(),
            "features": {
                "detailed_analysis": self.has_core,
                "complexity_calculation": self.has_core,
                "quality_scoring": self.has_core,
                "ai_analysis": True
            }
        }