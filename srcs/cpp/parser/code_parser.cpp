#include "code_parser.hpp"
#include <regex>
#include <algorithm>
#include <iostream>

namespace codementor {

CodeParser::CodeParser() {
    // 초기화 코드가 필요하면 여기에 구현
}

CodeParser::~CodeParser() {
    // 정리 코드가 필요하면 여기에 구현
}

std::string CodeParser::detectLanguage(const std::string& code) {
    // 간단한 휴리스틱 기반 언어 감지
    
    // Python 특징 검사
    if (code.find("def ") != std::string::npos || 
        code.find("import ") != std::string::npos || 
        code.find("class ") != std::string::npos && code.find(":") != std::string::npos) {
        return "python";
    }
    
    // C++ 특징 검사
    if (code.find("#include") != std::string::npos || 
        code.find("int main") != std::string::npos || 
        code.find("std::") != std::string::npos) {
        return "cpp";
    }
    
    // JavaScript 특징 검사
    if (code.find("function ") != std::string::npos || 
        code.find("const ") != std::string::npos || 
        code.find("let ") != std::string::npos || 
        code.find("=>") != std::string::npos) {
        return "javascript";
    }
    
    // 기본값
    return "unknown";
}

int CodeParser::calculateComplexity(const std::string& code, const std::string& language) {
    int complexity = 0;
    
    // 기본 복잡도: 코드 길이
    complexity += code.length() / 100;
    
    // 제어 구조 카운트
    std::regex if_regex("if\\s*\\(|if\\s+");
    std::regex for_regex("for\\s*\\(|for\\s+");
    std::regex while_regex("while\\s*\\(|while\\s+");
    std::regex switch_regex("switch\\s*\\(");
    std::regex try_regex("try\\s*\\{|try:");
    
    // 정규식으로 매칭되는 제어 구조 카운트
    std::string::const_iterator searchStart(code.cbegin());
    std::string::const_iterator searchEnd(code.cend());
    std::regex_iterator<std::string::const_iterator> if_it(searchStart, searchEnd, if_regex);
    std::regex_iterator<std::string::const_iterator> for_it(searchStart, searchEnd, for_regex);
    std::regex_iterator<std::string::const_iterator> while_it(searchStart, searchEnd, while_regex);
    std::regex_iterator<std::string::const_iterator> switch_it(searchStart, searchEnd, switch_regex);
    std::regex_iterator<std::string::const_iterator> try_it(searchStart, searchEnd, try_regex);
    
    std::regex_iterator<std::string::const_iterator> end;
    
    complexity += std::distance(if_it, end) * 1;
    complexity += std::distance(for_it, end) * 2;
    complexity += std::distance(while_it, end) * 2;
    complexity += std::distance(switch_it, end) * 3;
    complexity += std::distance(try_it, end) * 1;
    
    // 중첩 레벨 추정 (탭이나 공백으로 들여쓰기 계산)
    int max_indent = 0;
    int current_indent = 0;
    
    std::istringstream stream(code);
    std::string line;
    while (std::getline(stream, line)) {
        // 라인의 앞 공백 수 계산
        size_t indent = line.find_first_not_of(" \t");
        if (indent != std::string::npos) {
            current_indent = indent;
            max_indent = std::max(max_indent, current_indent);
        }
    }
    
    // 최대 들여쓰기 수준으로 복잡도 증가
    complexity += max_indent / 2;
    
    return complexity;
}

std::vector<std::string> CodeParser::extractImports(const std::string& code, const std::string& language) {
    std::vector<std::string> imports;
    
    if (language == "python") {
        // Python import 구문 찾기
        std::regex import_regex("(import|from)\\s+([\\w\\.]+)\\s*.*");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, import_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            imports.push_back(match.str());
        }
    }
    else if (language == "cpp") {
        // C++ include 구문 찾기
        std::regex include_regex("#include\\s*[<\"]([\\w\\./]+)[>\"]");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, include_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            imports.push_back(match.str());
        }
    }
    else if (language == "javascript") {
        // JavaScript import 구문 찾기
        std::regex import_regex("(import|require)\\s*[\\({]?\\s*['\"]([\\w\\./]+)['\"]");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, import_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            imports.push_back(match.str());
        }
    }
    
    return imports;
}

std::vector<std::string> CodeParser::extractFunctions(const std::string& code, const std::string& language) {
    std::vector<std::string> functions;
    
    if (language == "python") {
        // Python 함수 정의 찾기
        std::regex func_regex("def\\s+([\\w_]+)\\s*\\(");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                functions.push_back(match[1].str());
            }
        }
    }
    else if (language == "cpp") {
        // C++ 함수 정의 찾기 (단순화된 버전)
        std::regex func_regex("(\\w+)\\s+(\\w+)\\s*\\([^)]*\\)\\s*\\{");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 2 && match[1].str() != "if" && match[1].str() != "for" && 
                match[1].str() != "while" && match[1].str() != "switch") {
                functions.push_back(match[2].str());
            }
        }
    }
    else if (language == "javascript") {
        // JavaScript 함수 정의 찾기
        std::regex func_regex("function\\s+(\\w+)\\s*\\(|const\\s+(\\w+)\\s*=\\s*function|let\\s+(\\w+)\\s*=\\s*function");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            for (size_t j = 1; j < match.size(); ++j) {
                if (match[j].matched && !match[j].str().empty()) {
                    functions.push_back(match[j].str());
                    break;
                }
            }
        }
    }
    
    return functions;
}

std::vector<std::string> CodeParser::extractClasses(const std::string& code, const std::string& language) {
    std::vector<std::string> classes;
    
    if (language == "python") {
        // Python 클래스 정의 찾기
        std::regex class_regex("class\\s+(\\w+)");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                classes.push_back(match[1].str());
            }
        }
    }
    else if (language == "cpp") {
        // C++ 클래스 정의 찾기
        std::regex class_regex("class\\s+(\\w+)");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                classes.push_back(match[1].str());
            }
        }
    }
    else if (language == "javascript") {
        // JavaScript 클래스 정의 찾기
        std::regex class_regex("class\\s+(\\w+)");
        
        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        auto words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        auto words_end = std::sregex_iterator();
        
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                classes.push_back(match[1].str());
            }
        }
    }
    
    return classes;
}

CodeStructure CodeParser::parsePython(const std::string& code) {
    CodeStructure structure;
    structure.language = "python";
    structure.imports = extractImports(code, "python");
    structure.functions = extractFunctions(code, "python");
    structure.classes = extractClasses(code, "python");
    structure.complexity = calculateComplexity(code, "python");
    
    return structure;
}

CodeStructure CodeParser::parseCpp(const std::string& code) {
    CodeStructure structure;
    structure.language = "cpp";
    structure.imports = extractImports(code, "cpp");
    structure.functions = extractFunctions(code, "cpp");
    structure.classes = extractClasses(code, "cpp");
    structure.complexity = calculateComplexity(code, "cpp");
    
    return structure;
}

CodeStructure CodeParser::parseJavaScript(const std::string& code) {
    CodeStructure structure;
    structure.language = "javascript";
    structure.imports = extractImports(code, "javascript");
    structure.functions = extractFunctions(code, "javascript");
    structure.classes = extractClasses(code, "javascript");
    structure.complexity = calculateComplexity(code, "javascript");
    
    return structure;
}

CodeStructure CodeParser::parse(const std::string& code) {
    // 언어 감지
    std::string language = detectLanguage(code);
    
    // 감지된 언어에 따라 파싱 함수 호출
    if (language == "python") {
        return parsePython(code);
    }
    else if (language == "cpp") {
        return parseCpp(code);
    }
    else if (language == "javascript") {
        return parseJavaScript(code);
    }
    
    // 알 수 없는 언어의 경우 최소 정보만 반환
    CodeStructure structure;
    structure.language = "unknown";
    structure.complexity = code.length() / 100;  // 간단한 복잡도 추정
    return structure;
}

} // namespace codementor
