#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

// 코드 파싱 결과를 담는 구조체
struct CodeStructure {
    std::string language;                // 감지된 프로그래밍 언어
    int complexity;                      // 코드 복잡도 점수
    std::vector<std::string> imports;    // 임포트/인클루드된 모듈들
    std::vector<std::string> functions;  // 함수 이름들
    std::vector<std::string> classes;    // 클래스 이름들

    // 추가 메타데이터
    std::unordered_map<std::string, std::string> metadata;
};

// 코드 파서 인터페이스 클래스
class CodeParser {
public:
    // 생성자
    CodeParser();

    // 소멸자
    virtual ~CodeParser();

    // 코드 문자열을 파싱하여 구조 정보 반환
    CodeStructure parse(const std::string& code);

    // 코드 언어 감지 (Python, C++, JavaScript 등)
    std::string detectLanguage(const std::string& code);

    // 코드 복잡도 계산 (간단한 지표)
    int calculateComplexity(const std::string& code, const std::string& language);

    // 임포트/인클루드 구문 추출
    std::vector<std::string> extractImports(const std::string& code, const std::string& language);

    // 함수 정의 추출
    std::vector<std::string> extractFunctions(const std::string& code, const std::string& language);

    // 클래스 정의 추출
    std::vector<std::string> extractClasses(const std::string& code, const std::string& language);

private:
    // Python 코드 파싱 내부 함수
    CodeStructure parsePython(const std::string& code);

    // C++ 코드 파싱 내부 함수
    CodeStructure parseCpp(const std::string& code);

    // JavaScript 코드 파싱 내부 함수
    CodeStructure parseJavaScript(const std::string& code);
};
