#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "code_parser.hpp"

namespace py = pybind11;

PYBIND11_MODULE(codementor_core, m) {
    m.doc() = "Code Educator C++ core module";

    py::class_<codementor::CodeStructure>(m, "CodeStructure")
        .def(py::init<>())
        .def_readwrite("language", &codementor::CodeStructure::language)
        .def_readwrite("complexity", &codementor::CodeStructure::complexity)
        .def_readwrite("imports", &codementor::CodeStructure::imports)
        .def_readwrite("functions", &codementor::CodeStructure::functions)
        .def_readwrite("classes", &codementor::CodeStructure::classes)
        .def_readwrite("metadata", &codementor::CodeStructure::metadata)
        .def("__repr__",
            [](const codementor::CodeStructure &cs) {
                return "<CodeStructure language='" + cs.language +
                       "' complexity=" + std::to_string(cs.complexity) +
                       " imports=" + std::to_string(cs.imports.size()) +
                       " functions=" + std::to_string(cs.functions.size()) +
                       " classes=" + std::to_string(cs.classes.size()) + ">";
            }
        );

    py::class_<codementor::CodeParser>(m, "CodeParser")
        .def(py::init<>())
        .def("parse", &codementor::CodeParser::parse,
             "Parse code and return the code structure",
             py::arg("code"))
        .def("detect_language", &codementor::CodeParser::detectLanguage,
             "Detect programming language of code",
             py::arg("code"))
        .def("calculate_complexity", &codementor::CodeParser::calculateComplexity,
             "Calculate code complexity",
             py::arg("code"), py::arg("language"))
        .def("extract_imports", &codementor::CodeParser::extractImports,
             "Extract import statements from code",
             py::arg("code"), py::arg("language"))
        .def("extract_functions", &codementor::CodeParser::extractFunctions,
             "Extract function definitions from code",
             py::arg("code"), py::arg("language"))
        .def("extract_classes", &codementor::CodeParser::extractClasses,
             "Extract class definitions from code",
             py::arg("code"), py::arg("language"));

    m.def("version", []() { return "0.1.0"; }, "Return the version of codementor_core");
}
