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
test_case "git@github.com:teoljungberg/dotfiles"
test_case "git@github.com:teoljungberg/dotfiles.git"
test_case "teoljunberg/dotfiles"
test_case "dotfiles"
test_case "git@git.sr.ht:~teoljungberg/dotfiles"
test_case "git@git.sr.ht:~teoljungberg/dotfiles.git"

echo "Cloning outside of project base directory:"
TMPDIR=$(mktemp -d) || exit 1
(
  cd "$TMPDIR" || exit 1
  echo "pwd: $(pwd)"
  test_case "git@github.com:teoljungberg/dotfiles"
  test_case "git@git.sr.ht:~teoljungberg/dotfiles"
)
rmdir "$TMPDIR"
