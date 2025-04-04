#include "CodeParser.hpp"
#include <regex>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

CodeParser::CodeParser() {
}

CodeParser::~CodeParser() {
}

std::string CodeParser::detectLanguage(const std::string& code) {

    // Python characteristics check
    if (code.find("def ") != std::string::npos ||
        code.find("import ") != std::string::npos ||
        code.find("class ") != std::string::npos && code.find(":") != std::string::npos) {
        return "python";
    }

    // C++ characteristics check
    if (code.find("#include") != std::string::npos ||
        code.find("int main") != std::string::npos ||
        code.find("std::") != std::string::npos) {
        return "cpp";
    }

    // JavaScript characteristics check
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

    // default complexity based on lines of code
    complexity += code.length() / 100;

    // control structures complexity
    std::regex if_regex("if\\s*\\(|if\\s+");
    std::regex for_regex("for\\s*\\(|for\\s+");
    std::regex while_regex("while\\s*\\(|while\\s+");
    std::regex switch_regex("switch\\s*\\(");
    std::regex try_regex("try\\s*\\{|try:");

    // search for control structures
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

    // indentation complexity
    int max_indent = 0;
    int current_indent = 0;

    std::istringstream stream(code);
    std::string line;
    while (std::getline(stream, line)) {
        // skip empty lines
        size_t indent = line.find_first_not_of(" \t");
        if (indent != std::string::npos) {
            current_indent = indent;
            max_indent = std::max(max_indent, current_indent);
        }
    }

    complexity += max_indent / 2;

    return complexity;
}

std::vector<std::string> CodeParser::extractImports(const std::string& code, const std::string& language) {
    std::vector<std::string> imports;

    if (language == "python") {
        std::regex import_regex("(import|from)\\s+([\\w\\.]+)\\s*.*");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, import_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            imports.push_back(match.str());
        }
    }
    else if (language == "cpp") {
        // C++ include
        std::regex include_regex("#include\\s*[<\"]([\\w\\./]+)[>\"]");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, include_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            imports.push_back(match.str());
        }
    }
    else if (language == "javascript") {
        // JavaScript import
        std::regex import_regex("(import|require)\\s*[\\({]?\\s*['\"]([\\w\\./]+)['\"]");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, import_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

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
        // Find Python function definitions
        std::regex func_regex("def\\s+([\\w_]+)\\s*\\(");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                functions.push_back(match[1].str());
            }
        }
    }
    else if (language == "cpp") {
        std::regex func_regex("(\\w+)\\s+(\\w+)\\s*\\([^)]*\\)\\s*\\{");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 2 && match[1].str() != "if" && match[1].str() != "for" &&
                match[1].str() != "while" && match[1].str() != "switch") {
                functions.push_back(match[2].str());
            }
        }
    }
    else if (language == "javascript") {
        // JavaScript function definition search
        std::regex func_regex("function\\s+(\\w+)\\s*\\(|const\\s+(\\w+)\\s*=\\s*function|let\\s+(\\w+)\\s*=\\s*function");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, func_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

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
        std::regex class_regex("class\\s+(\\w+)");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                classes.push_back(match[1].str());
            }
        }
    }
    else if (language == "cpp") {
        // C++ class definetion search
        std::regex class_regex("class\\s+(\\w+)");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            if (match.size() > 1) {
                classes.push_back(match[1].str());
            }
        }
    }
    else if (language == "javascript") {
        // JavaScript definition search
        std::regex class_regex("class\\s+(\\w+)");

        std::string::const_iterator searchStart(code.cbegin());
        std::string::const_iterator searchEnd(code.cend());
        std::sregex_iterator words_begin = std::sregex_iterator(searchStart, searchEnd, class_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

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
    std::string language = detectLanguage(code);  // dectect language

    if (language == "python") {
        return parsePython(code);
    }
    else if (language == "cpp") {
        return parseCpp(code);
    }
    else if (language == "javascript") {
        return parseJavaScript(code);
    }

    CodeStructure structure;
    structure.language = "unknown";
    structure.complexity = code.length() / 100;
    return structure;
}
