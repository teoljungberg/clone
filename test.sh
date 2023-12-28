#!/bin/sh

set -e

# Generate absolut root to project
ROOT=$(pwd)
CLONE="$ROOT/clone"

test_case() {
  echo "% clone $1"
  "$CLONE" "$1"
  echo
}

echo "Cloning variations of URLs in pwd:"
echo "pwd: $(pwd)"
cd "$ROOT" || exit 1
test_case "git@github.com:user/project"
test_case "git@github.com:user/project.git"
test_case "another-user/project"
test_case "project"
test_case "git@git.sr.ht:~user/project"
test_case "git@git.sr.ht:~user/project.git"

echo "Cloning outside of project base directory:"
TMPDIR=$(mktemp -d) || exit 1
(
  cd "$TMPDIR" || exit 1
  echo "pwd: $(pwd)"
  test_case "git@github.com:user/project"
  test_case "git@git.sr.ht:~user/project"
)
rmdir "$TMPDIR"
