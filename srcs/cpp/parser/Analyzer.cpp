#include "Analyzer.hpp"

namespace code_educator {


Analyzer::Analyzer(): parser() {}  // constructor

Analyzer::~Analyzer() {}  // destructor

/*
 * Analyze code and return the analysis result
 * @param code: code to analyze
 * @return: analysis result
 */
AnalysisResult Analyzer::analyze(const std::string& code) {

	// detect language
	std::string language = parser.detectLanguage(code);

	// parse code structure
	CodeStructure structure = parser.parse(code);

	// analyze code with structure
	return analyzeWithSturcture(code, structure);
}

/*
 * Analyze code with the given structure
 * @param code: code to analyze
 * @param structure: code structure
 * @return: analysis result
 */
AnalysisResult Analyzer::analyzeWithSturcture(const std::string& code, const CodeStructure& structure) {
	AnalysisResult result;

	result.lineCount = countLines(code);
	result.commentCount = countComments(code, structure.language);
	if (result.lineCount > 0) {
		result.commentRatio = static_cast<double>(result.commentCount) / result.lineCount;
	} else {
		result.commentRatio = 0.0;  // avoid division by zero
	}
	result.nestingLength = calculateNestingLength(code, structure.language);
	result.cyclomaticComplexity = calculateCyclomaticComplexity(code, structure.language);
	result.tokenFrequency = calculateTokenFrequency(code);
	result.potentialIssues = findPotentialIssues(code, structure.language);

	// generate suggestions
	result.suggestions = generateSuggestions(code, structure);

	// add metadata
	result.metadata["language"] = structure.language;
	result.metadata["function_count"] = std::to_string(structure.functions.size());
	result.metadata["class_count"] = std::to_string(structure.classes.size());
	result.metadata["import_count"] = std::to_string(structure.imports.size());

	return result;
}

/*
 * Calculate the quality of the code based on various metrics
 * @param result: analysis result
 * @return: quality score (0 to 100)
 */
int Analyzer::calculateQuality(const AnalysisResult& result) {
	// Basic score is 100 (highest)
	int score = 100;

	// Calculate the quality based on various metrics
	// 1. cyclomatic complexity
	if (result.cyclomaticComplexity > 15) {
		score -= 20;  // reduce score for high cyclomatic complexity
	}
	else if (result.cyclomaticComplexity > 10) {
		score -= 10;  // reduce score for moderate cyclomatic complexity
	}
	else if (result.cyclomaticComplexity > 5) {
		score -= 5;  // reduce score for low cyclomatic complexity
	}

	// 2. comment ratio
	if (result.commentRatio < 0.1) {
		score -= 10;  // reduce score for low comment ratio
	}
	else if (result.commentRatio < 0.4) {
		score -= 5;  // reduce score for high comment ratio
	}

	// 3. nesting length
	if (result.nestingLength > 5) {
		score -= 15;  // reduce score for high nesting length
	}
	else if (result.nestingLength > 3) {
		score -= 8;  // reduce score for moderate nesting length
	}
	else if (result.nestingLength > 1) {
		score -= 3;  // reduce score for low nesting length
	}

	// 4. potential issues
	score -= result.potentialIssues.size() * 3;  // reduce score for each potential issue

	if (score < 0) {
		score = 0;  // ensure score is not negative
	}
	else if (score > 100) {
		score = 100;  // ensure score is not greater than 100
	}
	return score;
}

/*
 * Generate suggestions based on code structure
 * @param code: code to analyze
 * @param structure: code structure
 * @return: suggestions for improvement
 */
std::vector<std::string> Analyzer::generateSuggestions(const std::string& code, const CodeStructure& structure) {
	std::vector<std::string> suggestions;

	// 1. complexity
	if (structure.complexity > 10) {
		suggestions.push_back("Consider refactoring the code to reduce complexity.");
	}

	// 2. function count
	if (structure.functions.size() > 10) {
		suggestions.push_back("Consider breaking down large functions into smaller ones.");
	}
	// 3. class count
	if (structure.classes.size() > 5) {
		suggestions.push_back("Consider breaking down large classes into smaller ones.");
	}
	// 4. import count
	if (structure.imports.size() > 5) {
		suggestions.push_back("Consider removing unused imports.");
	}

	// 5. Suggestion based on language
	if (structure.language == "python") {
		if (code.find("except:") != std::string::npos) {
			suggestions.push_back("Consider specifying the exception type in the except clause.");
		}
		else if (code.find("global ") != std::string::npos) {
			suggestions.push_back("Avoid using global variables unless necessary.");
		}
	}
	else if (structure.language == "cpp") {
		if (code.find("new") != std::string::npos && code.find("delete") == std::string::npos) {
			suggestions.push_back("Consider using smart pointers to manage memory.");
		}
		else if (code.find("using namespace std;") != std::string::npos) {
			suggestions.push_back("Avoid using 'using namespace std;' in header files.");
		}
	}
	else if (structure.language == "javascript") {
		if (code.find("var ") != std::string::npos) {
			suggestions.push_back("Consider using 'let' or 'const' instead of 'var'.");
		}
		else if (code.find("==") != std::string::npos) {
			suggestions.push_back("Consider using '===' for strict equality comparison.");
		}
	}
	else if (structure.language == "c") {
		if (code.find("malloc") != std::string::npos && code.find("free") == std::string::npos) {
			suggestions.push_back("Consider using 'free' to deallocate memory allocated with 'malloc'.");
		}
		else if (code.find("strcpy") != std::string::npos) {
			suggestions.push_back("Consider using 'strncpy' to avoid buffer overflow.");
		}
	}

	return suggestions;
}

/*
 * Analyze Python code
 * @param code: code to analyze
 * @return: analysis result
 */
AnalysisResult Analyzer::analyzePython(const std::string& code) {
	CodeStructure structure = parser.parse(code);

	if (structure.language != "python") {
		throw std::runtime_error("Language mismatch: expected Python");
	}
	return analyzeWithSturcture(code, structure);
}

/*
 * Analyze C++ code
 * @param code: code to analyze
 * @return: analysis result
 */
AnalysisResult Analyzer::analyzeCpp(const std::string& code) {
	CodeStructure structure = parser.parse(code);

	if (structure.language != "cpp") {
		throw std::runtime_error("Language mismatch: expected C++");
	}
	return analyzeWithSturcture(code, structure);
}

/*
 * Analyze JavaScript code
 * @param code: code to analyze
 * @return: analysis result
 */
AnalysisResult Analyzer::analyzeJavaScript(const std::string& code) {
	CodeStructure structure = parser.parse(code);

	if (structure.language != "javascript") {
		throw std::runtime_error("Language mismatch: expected JavaScript");
	}
	return analyzeWithSturcture(code, structure);
}

/*
 * Analyze C code
 * @param code: code to analyze
 * @return: analysis result
 */
AnalysisResult Analyzer::analyzeC(const std::string& code) {
	CodeStructure structure = parser.parse(code);

	if (structure.language != "c") {
		throw std::runtime_error("Language mismatch: expected C");
	}
	return analyzeWithSturcture(code, structure);
}

/*
 * Count lines of code
 * @param code: code to analyze
 * @return: number of lines in the code
 */
int Analyzer::countLines(const std::string& code) {
	std::istringstream stream(code);
	std::string line;
	int count = 0;

	while (std::getline(stream, line)) {
		if (!line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos) {
			count++;
		}
	}
	return count;
}

/*
 * Count comments in the code (single line and multiline)
 * @param code: code to analyze
 * @param language: language of the code
 * @return: number of comments in the code
 */
int Analyzer::countComments(const std::string& code, const std::string& language) {
	int count = 0;
	bool multilineComment = false;
	std::istringstream stream(code);
	std::string line;

	// comment for python
	if (language == "python") {
		while (std::getline(stream, line)) {
			if (line.find("#") != std::string::npos) {
				count++;
			}
		}

		if (line.find("'''") != std::string::npos || line.find("\"\"\"") != std::string::npos) {
			multilineComment = !multilineComment;
			count++;
		}
		if (multilineComment) {
			count++;
		}
	}
	// c, cpp and javascript are same format of comments
	else if (language == "cpp" || language == "c" || language == "javascript") {
		while (std::getline(stream, line)) {
			if (line.find("//") != std::string::npos) {
				count++;
			}
			if (line.find("/*") != std::string::npos && line.find("*/") != std::string::npos) {  // single line comment /* like this */
				count++;
			}
			if (line.find("/*") != std::string::npos) {
				multilineComment = true;
			}
			if (multilineComment) {
				count++;
			}
			if (line.find("*/") != std::string::npos) {  // close multiline comment
				multilineComment = false;
			}
		}
	}
	else {
		throw std::runtime_error("Unsupported language for comment counting");
	}
	return count;
}

int Analyzer::calculateNestingLength(const std::string& code, const std::string& language) {
	int maxDepth = 0;
	int currentDepth = 0;
	std::istringstream stream(code);
	std::string line;

	if (language == "python") {
		int prevIndent = 0;

		while (std::getline(stream, line)) {
			// skip empty lines
			if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
				continue;
			}

			// count indentation
			int indent = 0;
			for (size_t i = 0; i < line.size(); ++i) {
				char c = line[i];
				if (c == ' ') {  // single space
					indent++;
				}
				else if (c == '\t') {  // tab, assuming tab is 4 spaces
					indent += 4;
				}
				else {
					break;
				}
			}

			if (indent > prevIndent) {
				currentDepth += (indent - prevIndent) / 4;  // assuming 4 spaces per indent
			}
			else if (indent < prevIndent) {
				currentDepth -= (prevIndent - indent) / 4;  // assuming 4 spaces per indent
			}

			prevIndent = indent;
			maxDepth = std::max(maxDepth, currentDepth);
		}
	}
	else {  // c, c++ and javascript
		bool inString = false;
		bool inChar = false;
		bool inComment = false;

		while (std::getline(stream, line)) {
			for (size_t i = 0; i < line.size(); i++) {
				char c = line[i];

				if (c == '"' && !inChar && !inComment) {
					if (i > 0 && line[i - 1] != '\\') continue; // skip escaped quotes
					inString = !inString;
					continue;
				}

				if (c == '\'' && !inString && !inComment) {
					if (i > 0 && line[i - 1] != '\\') continue; // skip escaped quotes
					inChar = !inChar;
					continue;
				}

				if (i < line.size() - 1 && c == '/' && line[i + 1] == '/' && !inString && !inChar) {
					break;  // single line comment
				}
				if (i < line.size() - 1 && c == '/' && line[i + 1] == '*' && !inString && !inChar) {
					inComment = true;
					continue;
				}

				if (i > 0 && c == '*' && line[i - 1] == '/' && inComment) {
					inComment = false;
					continue;
				}

				if (inString || inChar || inComment) {
					continue;
				}

				// {} indentation
				if (c == '{') {
					currentDepth++;
					maxDepth = std::max(maxDepth, currentDepth);
				}
				else if (c == '}') {
					currentDepth--;
					maxDepth = std::max(maxDepth, currentDepth);
				}
			}
		}
	}

	return maxDepth;
}

int Analyzer::calculateCyclomaticComplexity(const std::string& code, const std::string& language) {
	int complexity = 1;  // start with 1 for the function itself

	// some regex of complexity
	std::regex if_regex("if\\s*\\(.*\\)");
	std::regex for_regex("for\\s*\\(.*\\)");
	std::regex while_regex("while\\s*\\(.*\\)");
	std::regex case_regex("case\\s+.*:");
	std::regex switch_regex("switch\\s*\\(.*\\)");
	std::regex and_regex("&&");
	std::regex or_regex("\\|\\|");
	std::regex tenary_regex("\\?");

	// python
	std::regex except_regex("except\\s*\\(.*\\)");
	// C++ and JavaScript
	std::regex catch_regex("catch\\s*\\(.*\\)");
	// C and C++
	std::regex goto_regex("goto\\s+\\w+");

	// search for control structures start and end
	std::string::const_iterator searchStart(code.cbegin());
	std::string::const_iterator searchEnd(code.cend());

	// calculate complexity based on control structures
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, if_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, for_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, while_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, case_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, switch_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, and_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, or_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, tenary_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, except_regex), std::sregex_iterator());
	complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, catch_regex), std::sregex_iterator());

	// special case for python
	if (language == "python") {
		complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, except_regex), std::sregex_iterator());
	}
	else if (language == "c" || language == "cpp") {  // there's no catch in c
		complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, goto_regex), std::sregex_iterator());
	}
	else if (language == "javascript" || language == "cpp") {  // there's no goto in javascript
		complexity += std::distance(std::sregex_iterator(searchStart, searchEnd, catch_regex), std::sregex_iterator());
	}

	return complexity;
}

// To calculate the token frequency
std::map<std::string, int> Analyzer::calculateTokenFrequency(const std::string& code) {
	std::map<std::string, int> frequency;
	std::regex word_regex("\\b[a-zA-Z_][a-zA-Z0-9_]*\\b");

	auto words_begin = std::sregex_iterator(code.begin(), code.end(), word_regex);
	auto words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		std::string word = (*i).str();
		frequency[word]++;
	}

	return frequency;
}

// to find the potential issues in the code based on language
std::vector<std::string> Analyzer::findPotentialIssues(const std::string& code, const std::string& language) {
	std::vector<std::string> issues;

	// length of the codes
	if (code.length() > 1000) {
		issues.push_back("Code is too long, consider breaking it into smaller functions.");
	}

	// complexity of the code
	int nestingLength = calculateNestingLength(code, language);
	if (nestingLength > 5) {
		issues.push_back("Code has high nesting length, consider refactoring.");
	}
	// cyclomatic complexity
	int cycComplexity = calculateCyclomaticComplexity(code, language);
	if (cycComplexity > 10) {
		issues.push_back("Code has high cyclomatic complexity, consider refactoring.");
	}

	// Check for potential issues based on language
	if (language == "python") {
		if (code.find("eval(") != std::string::npos) {
			issues.push_back("Avoid using eval() for security reasons.");
		}

		if (code.find("except") != std::string::npos) {
			issues.push_back("Consider specifying the exception type in the except clause.");
		}
		else if (code.find("global ") != std::string::npos) {
			issues.push_back("Avoid using global variables unless necessary.");
		}
	}
	else if (language == "cpp") {
		if (code.find("using namespace std;") != std::string::npos) {
			issues.push_back("Avoid using 'using namespace std;' in header files.");
		}
	}
	else if (language == "javascript") {
		if (code.find("eval(") != std::string::npos) {
			issues.push_back("Avoid using eval() for security reasons.");
		}
	}

	return issues;
}

}  // namespace code_educator
