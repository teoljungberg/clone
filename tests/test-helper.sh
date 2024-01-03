#!/bin/sh

set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)/
# shellcheck disable=SC2034
CLONE="$ROOT/clone"

clone() {
  "$CLONE" -d "$@"
}
