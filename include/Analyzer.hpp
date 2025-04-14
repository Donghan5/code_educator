#pragma once

#include "CodeParser.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>

namespace code_educator {
struct AnalysisResult {
	int lineCount;
	int commentCount;
	double commentRatio; // (comment / code)
	int nestingLength;
	int cyclomaticComplexity;
	std::map<std::string, int> tokenFrequency;
	std::vector<std::string> potentialIssues;
	std::vector<std::string> suggestions;


	std::map<std::string, std::string> metadata; // e.g., {"author": "John Doe", "date": "2023-10-01"}
};


class Analyzer {
	public:
		// Constructor and destructor
		Analyzer();
		virtual ~Analyzer();

		AnalysisResult analyze(const std::string& code);
		AnalysisResult analyzeWithSturcture(const std::string& code, const CodeStructure& structure);

		// Calculate the quality of the code based on various metrics
		// 0 to 100
		int calculateQuality(const AnalysisResult& result);

		std::vector<std::string> generateSuggestions(const std::string& code, const CodeStructure& structure);

		AnalysisResult analyzePython(const std::string& code);
		AnalysisResult analyzeJavaScript(const std::string& code);
		AnalysisResult analyzeCpp(const std::string& code);
		AnalysisResult analyzeC(const std::string& code);

	private:
		int countLines(const std::string& code);
		int countComments(const std::string& code, const std::string& language);
		int calculateNestingLength(const std::string& code, const std::string& language);
		int calculateCyclomaticComplexity(const std::string& code, const std::string& language);
		std::map<std::string, int> calculateTokenFrequency(const std::string& code);
		std::vector<std::string> findPotentialIssues(const std::string& code, const std::string& language);

		CodeParser parser;  // instance of CodeParser to parse the code
};
}   // namespace code_educator
