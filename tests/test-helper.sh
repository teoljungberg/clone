#!/bin/sh

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)/
# shellcheck disable=SC2034

clone() {
  # redirect stderr to stdout
  "$ROOT/clone" -n "$@" 2>&1
}
