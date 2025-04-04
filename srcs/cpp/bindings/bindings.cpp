#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "CodeParser.hpp"

namespace py = pybind11;

PYBIND11_MODULE(code_educator_core, m) {
    m.doc() = "Code Educator C++ core module";

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

    m.def("version", []() { return "0.1.0"; }, "Return the version of code_educator_core");
}
