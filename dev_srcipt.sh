#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PROJECT_ROOT="$( cd "$SCRIPT_DIR/.." &> /dev/null && pwd )"

cd $PROJECT_ROOT

# start the development environment
start_dev() {
    echo "🚀 Starting development environment..."
    docker-compose up
    echo "✅ Development environment started!"
    echo "💻 Connect to the development container with: docker-compose exec code_educator bash"
}

# connect to the development container
connect_dev() {
    echo "🔌 Connecting to development container..."
    docker-compose exec code_educator bash
}

# run tests
run_tests() {
    echo "🧪 Running tests..."
    docker-compose exec code_educator pytest
}

build() {
    echo "🔨 Building project..."
    docker-compose exec code_educator bash -c "mkdir -p build && cd build && cmake .. && make"
    echo "✅ Build completed!"
}

download_model() {
    echo "📥 Downloading CodeLlama model..."
    docker-compose exec ollama ollama pull codellama
    echo "✅ Model downloaded successfully!"
}

status() {
    echo "📊 Docker containers status:"
    docker-compose ps

    echo -e "\n🔍 Checking Ollama API accessibility..."
    if docker-compose exec code_educator curl -s --max-time 5 http://ollama:11434/api/tags >/dev/null; then
        echo "✅ Ollama API is accessible from code_educator container!"
    else
        echo "❌ Ollama API is NOT accessible from code_educator container!"
    fi

    echo -e "\n📋 Available models:"
    docker-compose exec ollama ollama list
}

cleanup() {
    echo "🧹 Cleaning up development environment..."
    docker-compose down
    echo "✅ Development environment stopped!"
}

# deep clean the development environment
# This will remove all containers and volumes including model data
# WARNING: This will delete all containers and volumes including model data!
deep_clean() {
    echo "⚠️ WARNING: This will delete all containers and volumes including model data!"
    read -p "Are you sure you want to continue? (y/N): " confirm
    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]; then
        echo "🧹 Deep cleaning development environment..."
        docker-compose down -v
        echo "✅ Development environment and volumes removed!"
    else
        echo "❌ Operation cancelled."
    fi
}

show_help() {
    echo "Usage: $0 {start|connect|test|build|model|status|cleanup|clean-all|help}"
    echo ""
    echo "Commands:"
    echo "  start     - Start the development environment"
    echo "  connect   - Connect to the development container"
    echo "  test      - Run tests"
    echo "  build     - Build the C++ code"
    echo "  model     - Download the CodeLlama model"
    echo "  status    - Check status of containers and API"
    echo "  cleanup   - Stop and remove containers"
    echo "  clean-all - Remove all containers and volumes (including model data)"
    echo "  help      - Show this help message"
}

case $1 in
    start)
        start_dev
        ;;
    connect)
        connect_dev
        ;;
    test)
        run_tests
        ;;
    build)
        build
        ;;
    model)
        download_model
        ;;
    status)
        status
        ;;
    cleanup)
        cleanup
        ;;
    clean-all)
        deep_clean
        ;;
    help|*)
        show_help
        exit 0
        ;;
esac
