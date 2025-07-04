#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "CodeParser.hpp"
#include "Analyzer.hpp"

namespace py = pybind11;

PYBIND11_MODULE(code_educator_core, m) {
    m.doc() = "Code Educator C++ core module";

    // CodeStructure 바인딩
    py::class_<code_educator::CodeStructure>(m, "CodeStructure")
        .def(py::init<>())
        .def_readwrite("language", &code_educator::CodeStructure::language)
        .def_readwrite("complexity", &code_educator::CodeStructure::complexity)
        .def_readwrite("imports", &code_educator::CodeStructure::imports)
        .def_readwrite("functions", &code_educator::CodeStructure::functions)
        .def_readwrite("classes", &code_educator::CodeStructure::classes)
        .def_readwrite("metadata", &code_educator::CodeStructure::metadata)
        .def("__repr__",
            [](const code_educator::CodeStructure &cs) {
                return "<CodeStructure language='" + cs.language +
                       "' complexity=" + std::to_string(cs.complexity) +
                       " imports=" + std::to_string(cs.imports.size()) +
                       " functions=" + std::to_string(cs.functions.size()) +
                       " classes=" + std::to_string(cs.classes.size()) + ">";
            }
        );

    // CodeParser
    py::class_<code_educator::CodeParser>(m, "CodeParser")
        .def(py::init<>())
        .def("parse", &code_educator::CodeParser::parse,
             "Parse code and return the code structure",
             py::arg("code"))
        .def("detect_language", &code_educator::CodeParser::detectLanguage,
             "Detect programming language of code",
             py::arg("code"))
        .def("calculate_complexity", &code_educator::CodeParser::calculateComplexity,
             "Calculate code complexity",
             py::arg("code"), py::arg("language"))
        .def("extract_imports", &code_educator::CodeParser::extractImports,
             "Extract import statements from code",
             py::arg("code"), py::arg("language"))
        .def("extract_functions", &code_educator::CodeParser::extractFunctions,
             "Extract function definitions from code",
             py::arg("code"), py::arg("language"))
        .def("extract_classes", &code_educator::CodeParser::extractClasses,
             "Extract class definitions from code",
             py::arg("code"), py::arg("language"));

    // AnalysisResult 바인딩
    py::class_<code_educator::AnalysisResult>(m, "AnalysisResult")
        .def(py::init<>())
        .def_readwrite("line_count", &code_educator::AnalysisResult::lineCount)
        .def_readwrite("comment_count", &code_educator::AnalysisResult::commentCount)
        .def_readwrite("comment_ratio", &code_educator::AnalysisResult::commentRatio)
        .def_readwrite("nesting_depth", &code_educator::AnalysisResult::nestingLength)
        .def_readwrite("cyclomatic_complexity", &code_educator::AnalysisResult::cyclomaticComplexity)
        .def_readwrite("token_frequency", &code_educator::AnalysisResult::tokenFrequency)
        .def_readwrite("potential_issues", &code_educator::AnalysisResult::potentialIssues)
        .def_readwrite("suggestions", &code_educator::AnalysisResult::suggestions)
        .def_readwrite("metadata", &code_educator::AnalysisResult::metadata)
        .def("__repr__",
            [](const code_educator::AnalysisResult &ar) {
                return "<AnalysisResult lines=" + std::to_string(ar.lineCount) +
                       " comments=" + std::to_string(ar.commentCount) +
                       " comment_ratio=" + std::to_string(ar.commentRatio) +
                       " nesting=" + std::to_string(ar.nestingLength) +
                       " complexity=" + std::to_string(ar.cyclomaticComplexity) +
                       " issues=" + std::to_string(ar.potentialIssues.size()) +
                       " suggestions=" + std::to_string(ar.suggestions.size()) + ">";
            }
        );

    // Analyzer
    py::class_<code_educator::Analyzer>(m, "Analyzer")
        .def(py::init<>())
        .def("analyze", &code_educator::Analyzer::analyze,
             "Analyze code and return detailed analysis results",
             py::arg("code"))
        .def("analyze_with_structure", &code_educator::Analyzer::analyzeWithSturcture,
             "Analyze code with existing structure information",
             py::arg("code"), py::arg("structure"))
        .def("calculate_quality_score", &code_educator::Analyzer::calculateQuality,
             "Calculate code quality score (0-100)",
             py::arg("result"))
        .def("generate_suggestions", &code_educator::Analyzer::generateSuggestions,
             "Generate code improvement suggestions",
             py::arg("code"), py::arg("structure"))
        .def("analyze_python", &code_educator::Analyzer::analyzePython,
             "Analyze Python code",
             py::arg("code"))
        .def("analyze_cpp", &code_educator::Analyzer::analyzeCpp,
             "Analyze C++ code",
             py::arg("code"))
        .def("analyze_javascript", &code_educator::Analyzer::analyzeJavaScript,
             "Analyze JavaScript code",
             py::arg("code"));

    m.def("version", []() { return "0.1.0"; }, "Return the version of code_educator_core");
}
