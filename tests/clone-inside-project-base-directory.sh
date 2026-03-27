cd "$ROOT" || exit 1

if testcase "clone full git+ssh github.com URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $HOME/src/github.com/user/project" \
    "$(clone "git@github.com:user/project.git")"
fi

if testcase "clone full git+ssh git.sr.ht URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src/git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $HOME/src/git.sr.ht/~user/project" \
    "$(clone "git@git.sr.ht:~user/project.git")"
fi

if testcase "clone full ssh:// URLs inside clone's directory structure"; then
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $HOME/src/got.gameoftrees.org/anonymous/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/got.git")"
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $HOME/src/got.gameoftrees.org/anonymous/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/got")"
fi

if testcase "clone full https github.com URLs inside clone's directory structure"; then
  assert_eq \
    "git clone https://github.com/user/project $HOME/src/github.com/user/project" \
    "$(clone "https://github.com/user/project")"
  assert_eq \
    "git clone https://github.com/user/project $HOME/src/github.com/user/project" \
    "$(clone "https://github.com/user/project.git")"
fi

# Shorthand patterns always reconstruct with git@ regardless of how the host
# directory was created. There is no protocol stored in the directory tree.
if testcase "clone shorthand patterns inside ssh:// host directory structure use git@ protocol"; then
  prior=$(pwd)
  tmpdir=$(mktemp -d "$HOME/src/test.XXXXXX") || exit 1
  host=$(basename "$tmpdir")
  sshdir="$tmpdir/anonymous"
  mkdir "$sshdir" || exit 1
  cd "$sshdir" || exit 1

  assert_eq \
    "git clone git@$host:anonymous/another-repo $HOME/src/$host/anonymous/another-repo" \
    "$(clone "another-repo")"
  assert_eq \
    "git clone git@$host:user/another-repo $HOME/src/$host/user/another-repo" \
    "$(clone "user/another-repo")"

  cd "$prior" || exit 1
  rm -rf "$tmpdir"
fi

if testcase "clone user/repository_name patterns inside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src/github.com/another-user/project" \
    "$(clone "another-user/project")"
  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src/github.com/another-user/project" \
    "$(clone "another-user/project.git")"
fi

if testcase "clone repository_name patterns inside a user's clone directory structure"; then
  prior=$(pwd)
  user=$(basename "$(dirname "$ROOT")")
  cd "$HOME/src/github.com/$user" || exit 1

  assert_eq \
    "git clone git@github.com:$user/project $HOME/src/github.com/$user/project" \
    "$(clone "project")"
  assert_eq \
    "git clone git@github.com:$user/project $HOME/src/github.com/$user/project" \
    "$(clone "project.git")"

  cd "$prior" || exit 1
fi

if testcase "clone user/repository_name patterns inside a host of a clone directory structure"; then
  cd "$HOME/src/github.com" || exit 1

  assert_eq \
    "git clone git@github.com:another-user/project $HOME/src/github.com/another-user/project" \
    "$(clone "another-user/project")"
fi
