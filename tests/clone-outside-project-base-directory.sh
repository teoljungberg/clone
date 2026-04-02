tmpdir=$(mktemp -d) || exit 1
cd "$tmpdir" || exit 1

if testcase "clone full git+ssh github.com URLs outside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"
fi

if testcase "clone full git+ssh git.sr.ht URLs outside clone's directory structure"; then
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src/git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src/git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"
fi

if testcase "clone full https:// github.com URLs outside clone's directory structure"; then
  assert_eq \
    "git clone https://github.com/user/project $HOME/src/github.com/user/project" \
    "$(clone "https://github.com/user/project")"
  assert_eq \
    "git clone https://github.com/user/project $HOME/src/github.com/user/project" \
    "$(clone "https://github.com/user/project.git")"
fi

if testcase "clone full https:// any other URLs outside clone's directory structure"; then
  assert_eq \
    "git clone https://example.com/user/project $HOME/src/example.com/user/project" \
    "$(clone "https://example.com/user/project")"
  assert_eq \
    "git clone https://example.com/user/project $HOME/src/example.com/user/project" \
    "$(clone "https://example.com/user/project.git")"
fi

if testcase "clone full SCP URLs with non-git login user outside clone's directory structure"; then
  assert_eq \
    "git clone deploy@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "deploy@github.com:user/project")"
  assert_eq \
    "git clone deploy@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "deploy@github.com:user/project.git")"
fi

if testcase "clone full ssh:// URLs outside clone's directory structure"; then
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $HOME/src/got.gameoftrees.org/anonymous/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/got.git")"
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $HOME/src/got.gameoftrees.org/anonymous/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/got")"
fi

if testcase "clone ssh:// URLs with port outside clone's directory structure"; then
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org:2222/got $HOME/src/got.gameoftrees.org/anonymous/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org:2222/got")"
fi

if testcase "clone https:// URLs with port outside clone's directory structure"; then
  assert_eq \
    "git clone https://github.com:8443/user/project $HOME/src/github.com/user/project" \
    "$(clone "https://github.com:8443/user/project")"
fi

rmdir "$tmpdir"
