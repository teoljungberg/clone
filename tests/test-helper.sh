#!/bin/sh

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)/
# shellcheck disable=SC2034
CLONE="$ROOT/clone"

clone() {
  # redirect stderr to stdout
  "$CLONE" -n "$@" 2>&1
}
