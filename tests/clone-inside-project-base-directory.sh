if testcase "clone inside project base directory"; then
  cd "$ROOT" || exit 1

  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"

  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src//github.com/another-user/project" \
    "$(clone "another-user/project")"
  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src//github.com/another-user/project" \
    "$(clone "another-user/project.git")"
  assert_eq \
    "git clone git@github.com:teoljungberg/project $HOME/src//github.com/teoljungberg/project" \
    "$(clone "project")"
  assert_eq \
    "git clone git@github.com:teoljungberg/project $HOME/src//github.com/teoljungberg/project" \
    "$(clone "project.git")"

  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"
fi
