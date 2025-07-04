#!/bin/bash
# manage.sh - Unified interface for project shell scripts

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

run_setup() {
    "$SCRIPT_DIR/setup_venv.sh" "$@"
}

run_activate() {
    "$SCRIPT_DIR/activate.sh" "$@"
}

run_venv() {
    "$SCRIPT_DIR/venv_manager.sh" "$@"
}

run_dev() {
    "$SCRIPT_DIR/dev_script.sh" "$@"
}

usage() {
    cat <<EOF
Usage: $0 <command> [args]

Commands:
  setup           Run setup_venv.sh
  activate        Run activate.sh
  venv <args>     Forward args to venv_manager.sh
  dev <args>      Forward args to dev_script.sh
  help            Show this message
EOF
}

case "${1-}" in
    setup)
        shift
        run_setup "$@"
        ;;
    activate)
        shift
        run_activate "$@"
        ;;
    venv)
        shift
        run_venv "$@"
        ;;
    dev)
        shift
        run_dev "$@"
        ;;
    help|"")
        usage
        ;;
    *)
        echo "Unknown command: $1"
        usage
        exit 1
        ;;
esac
