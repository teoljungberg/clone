cd "$ROOT" || exit 1

if testcase "clone full git+ssh github.com URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $HOME/src//github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"
fi

if testcase "clone full git+ssh git.sr.ht URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src//git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"
fi

if testcase "clone user/repository name patterns inside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src//github.com/another-user/project" \
    "$(clone "another-user/project")"
  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src//github.com/another-user/project" \
    "$(clone "another-user/project.git")"
fi

if testcase "clone repository_name patterns inside a user's clone directory structure"; then
  user=$(basename $(dirname $ROOT))

  assert_eq \
    "git clone git@github.com:$user/project $HOME/src//github.com/$user/project" \
    "$(clone "project")"
  assert_eq \
    "git clone git@github.com:$user/project $HOME/src//github.com/$user/project" \
    "$(clone "project.git")"
fi
