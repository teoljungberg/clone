if testcase "clone outside project base directory"; then
  TMPDIR=$(mktemp -d) || exit 1
  cd "$TMPDIR" || exit 1

  assert_eq \
    "git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"

  assert_eq \
    "git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"

  rmdir "$TMPDIR"
fi
