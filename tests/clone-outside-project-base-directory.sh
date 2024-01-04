tmpdir=$(mktemp -d) || exit 1
cd "$tmpdir" || exit 1

if testcase "clone full git+ssh github.com URLs outside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"
fi

if testcase "clone full git+ssh git.sr.ht URLs outside clone's directory structure"; then
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"
fi

rmdir "$tmpdir"
