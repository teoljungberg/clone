tmpdir=$(mktemp -d) || exit 1
tmpdir=$(cd "$tmpdir" && pwd -P)
mkdir -p "$tmpdir/src/github.com/user/clone"

cd "$tmpdir/src/github.com/user/clone" || exit 1

if testcase "clone full git+ssh github.com URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@github.com:user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "git@github.com:user/project")"
  assert_eq \
    "git clone git@github.com:user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "git@github.com:user/project.git")"
fi

if testcase "clone full git+ssh git.sr.ht URLs inside clone's directory structure"; then
  assert_eq \
    "git clone git@git.sr.ht:~user/project $tmpdir/src/git.sr.ht/~user/project" \
    "$(HOME=$tmpdir clone "git@git.sr.ht:~user/project")"
  assert_eq \
    "git clone git@git.sr.ht:~user/project $tmpdir/src/git.sr.ht/~user/project" \
    "$(HOME=$tmpdir clone "git@git.sr.ht:~user/project.git")"
fi

if testcase "clone full ssh:// URLs inside clone's directory structure"; then
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $tmpdir/src/got.gameoftrees.org/anonymous/got" \
    "$(HOME=$tmpdir clone "ssh://anonymous@got.gameoftrees.org/got.git")"
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org/got $tmpdir/src/got.gameoftrees.org/anonymous/got" \
    "$(HOME=$tmpdir clone "ssh://anonymous@got.gameoftrees.org/got")"
fi

if testcase "clone full ssh:// URLs with port inside clone's directory structure"; then
  assert_eq \
    "git clone ssh://anonymous@got.gameoftrees.org:2222/got $tmpdir/src/got.gameoftrees.org/anonymous/got" \
    "$(HOME=$tmpdir clone "ssh://anonymous@got.gameoftrees.org:2222/got")"
fi

if testcase "clone full SCP URLs with non-git login user inside clone's directory structure"; then
  assert_eq \
    "git clone deploy@github.com:user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "deploy@github.com:user/project")"
fi

if testcase "clone full https github.com URLs inside clone's directory structure"; then
  assert_eq \
    "git clone https://github.com/user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "https://github.com/user/project")"
  assert_eq \
    "git clone https://github.com/user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "https://github.com/user/project.git")"
fi

if testcase "clone full https:// URLs with port inside clone's directory structure"; then
  assert_eq \
    "git clone https://github.com:8443/user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "https://github.com:8443/user/project")"
fi

if testcase "clone user/repository_name patterns inside clone's directory structure"; then
  cd "$tmpdir/src/github.com/user/clone" || exit 1

  assert_eq \
    "git clone git@github.com:another-user/project $tmpdir/src/github.com/another-user/project" \
    "$(HOME=$tmpdir clone "another-user/project")"
  assert_eq \
    "git clone git@github.com:another-user/project $tmpdir/src/github.com/another-user/project" \
    "$(HOME=$tmpdir clone "another-user/project.git")"
fi

if testcase "clone repository_name patterns inside a user's clone directory structure"; then
  cd "$tmpdir/src/github.com/user" || exit 1

  assert_eq \
    "git clone git@github.com:user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "project")"
  assert_eq \
    "git clone git@github.com:user/project $tmpdir/src/github.com/user/project" \
    "$(HOME=$tmpdir clone "project.git")"
fi

if testcase "clone user/repository_name patterns inside a host of a clone directory structure"; then
  cd "$tmpdir/src/github.com" || exit 1

  assert_eq \
    "git clone git@github.com:another-user/project $tmpdir/src/github.com/another-user/project" \
    "$(HOME=$tmpdir clone "another-user/project")"
fi

rm -rf "$tmpdir"
