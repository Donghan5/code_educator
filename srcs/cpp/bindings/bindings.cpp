#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "CodeParser.hpp"

namespace py = pybind11;

PYBIND11_MODULE(code_educator_core, m) {
    m.doc() = "Code Educator C++ core module";

    py::class_<CodeEducator::CodeStructure>(m, "CodeStructure")
        .def(py::init<>())
        .def_readwrite("language", &CodeEducator::CodeStructure::language)
        .def_readwrite("complexity", &CodeEducator::CodeStructure::complexity)
        .def_readwrite("imports", &CodeEducator::CodeStructure::imports)
        .def_readwrite("functions", &CodeEducator::CodeStructure::functions)
        .def_readwrite("classes", &CodeEducator::CodeStructure::classes)
        .def_readwrite("metadata", &CodeEducator::CodeStructure::metadata)
        .def("__repr__",
            [](const CodeEducator::CodeStructure &cs) {
                return "<CodeStructure language='" + cs.language +
                       "' complexity=" + std::to_string(cs.complexity) +
                       " imports=" + std::to_string(cs.imports.size()) +
                       " functions=" + std::to_string(cs.functions.size()) +
                       " classes=" + std::to_string(cs.classes.size()) + ">";
            }
        );

    py::class_<CodeEducator::CodeParser>(m, "CodeParser")
        .def(py::init<>())
        .def("parse", &CodeEducator::CodeParser::parse,
             "Parse code and return the code structure",
             py::arg("code"))
        .def("detect_language", &CodeEducator::CodeParser::detectLanguage,
             "Detect programming language of code",
             py::arg("code"))
        .def("calculate_complexity", &CodeEducator::CodeParser::calculateComplexity,
             "Calculate code complexity",
             py::arg("code"), py::arg("language"))
        .def("extract_imports", &CodeEducator::CodeParser::extractImports,
             "Extract import statements from code",
             py::arg("code"), py::arg("language"))
        .def("extract_functions", &CodeEducator::CodeParser::extractFunctions,
             "Extract function definitions from code",
             py::arg("code"), py::arg("language"))
        .def("extract_classes", &CodeEducator::CodeParser::extractClasses,
             "Extract class definitions from code",
             py::arg("code"), py::arg("language"));

    m.def("version", []() { return "0.1.0"; }, "Return the version of code_educator_core");
}
