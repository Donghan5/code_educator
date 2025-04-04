#pragma once

#include <regex>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

// structure to hold code structure information
struct CodeStructure {
    std::string language;                // Dectected language (Python, C++, JavaScript)
    int complexity;                      // Code complexity (simple metric)
    std::vector<std::string> imports;    // modules or libraries imported
    std::vector<std::string> functions;  // Function names
    std::vector<std::string> classes;    // class names

    // add metadata
    std::unordered_map<std::string, std::string> metadata;
};


class CodeParser {
public:
    CodeParser();

    virtual ~CodeParser();

    CodeStructure parse(const std::string& code);

    std::string detectLanguage(const std::string& code);

    int calculateComplexity(const std::string& code, const std::string& language);

    std::vector<std::string> extractImports(const std::string& code, const std::string& language);

    std::vector<std::string> extractFunctions(const std::string& code, const std::string& language);

    std::vector<std::string> extractClasses(const std::string& code, const std::string& language);

private:
    CodeStructure parsePython(const std::string& code);

    CodeStructure parseCpp(const std::string& code);

    CodeStructure parseJavaScript(const std::string& code);
};
