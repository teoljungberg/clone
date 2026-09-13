cd "$ROOT" || exit 1

if testcase "clone respects default CLONE_PATH"; then
  assert_eq \
    "git clone git@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "git@github.com:user/project")"
fi

if testcase "clone respects custom CLONE_PATH"; then
  tmpdir=$(mktemp -d) || exit 1
  custom_prefix="$tmpdir/usr"

  (cd "$ROOT" && make clean >/dev/null 2>&1)
  (cd "$ROOT" && make CLONE_PATH="$tmpdir/custom" >/dev/null 2>&1)
  (cd "$ROOT" && make install DESTDIR="$tmpdir" PREFIX="/usr/local" >/dev/null 2>&1)

  (cd "$ROOT" && make clean >/dev/null 2>&1)
  (cd "$ROOT" && make >/dev/null 2>&1)

  assert_eq \
    "git clone git@github.com:user/project $tmpdir/custom/github.com/user/project" \
    "$("$custom_prefix/local/bin/clone" -n "git@github.com:user/project" 2>&1)"

  rm -rf "$tmpdir"
fi

if testcase "clone expands tilde in custom CLONE_PATH"; then
  tmpdir=$(mktemp -d) || exit 1
  custom_prefix="$tmpdir/usr"

  (cd "$ROOT" && make clean >/dev/null 2>&1)
  (cd "$ROOT" && make CLONE_PATH='~/custom' >/dev/null 2>&1)
  (cd "$ROOT" && make install DESTDIR="$tmpdir" PREFIX="/usr/local" >/dev/null 2>&1)

  (cd "$ROOT" && make clean >/dev/null 2>&1)
  (cd "$ROOT" && make >/dev/null 2>&1)

  assert_eq \
    "git clone git@github.com:user/project $tmpdir/custom/github.com/user/project" \
    "$(HOME="$tmpdir" "$custom_prefix/local/bin/clone" -n "git@github.com:user/project" 2>&1)"

  rm -rf "$tmpdir"
fi

if testcase "clone keeps literal ~/src when HOME is unset or empty"; then
  assert_eq \
    "git clone git@github.com:user/project ~/src/github.com/user/project" \
    "$(unset HOME; clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project ~/src/github.com/user/project" \
    "$(HOME= clone "git@github.com:user/project")"
fi
