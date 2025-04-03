# Code Educator

An AI-powered CLI tool to assist with programming education, combining C++ performance with Python usability.

## Key Features

- AI-powered responses to programming questions
- Code analysis and complexity evaluation
- Code structure visualization
- Support for multiple programming languages (Python, C++, JavaScript, etc.)

## Installation

### Requirements

- Python 3.7+
- C++ compiler (GCC 7+, Clang 5+, MSVC 2019+)
- CMake 3.10+
- Ollama (for local LLM execution)

### Docker Installation (Recommended)

```bash
# Clone the repository
git clone https://github.com/yourusername/code-educator.git
cd code-educator

# Start development environment
./scripts/dev.sh start

# Download Ollama model
./scripts/dev.sh model

# Build the project
./scripts/dev.sh build
```

### Local Installation

```bash
# Clone the repository
git clone git@github.com:Donghan5/code_educator.git
cd code-educator

# Install and run Ollama
# See https://ollama.com for installation instructions
ollama pull codellama

# Create and activate virtual environment
python -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate

# Install package
pip install -e .
```

## Usage

```bash
# Ask a programming question
code-educator ask "How do I implement a recursive Fibonacci sequence in Python?"

# Check available models
code-educator models

# Verify API connection
code-educator check
```

## Project Structure

```
code-educator/
├── src/
│   ├── python/           # Python interface code
│   │   ├── cli.py        # CLI interface
│   │   ├── __init__.py   # Package initialization
│   │   └── api.py        # Ollama API integration
│   ├── cpp/              # C++ core modules
│   │   ├── parser/       # Code parsing engine
│   │   ├── analyzer/     # Code analysis logic (future implementation)
│   │   └── bindings/     # Python bindings
├── include/              # Header files
├── tests/                # Test code (future implementation)
├── docs/                 # Documentation (future implementation)
├── scripts/              # Utility scripts
├── CMakeLists.txt        # C++ build configuration
├── setup.py              # Python package configuration
└── README.md             # Project description
```

## Development

### Setting Up Development Environment

```bash
# Start development environment
./scripts/dev.sh start

# Run tests
./scripts/dev.sh test

# Clean up development environment
./scripts/dev.sh cleanup
```

### How to Contribute

1. Fork this repository
2. Create a new branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Create a Pull Request

## Performance Benefits

The Code Educator tool leverages C++ for performance-critical operations:
- Fast code parsing and analysis
- Efficient memory management
- Optimized computational tasks

This hybrid approach combines C++ performance with Python's ease of use and extensive ecosystem.

## License

Distributed under the MIT License.

## Acknowledgments

- Ollama project - For providing local LLM runtime
- pybind11 - For C++ and Python binding support
- click - For Python CLI interface implementation

---

**Note**: This project is under active development, and features may not be complete.
