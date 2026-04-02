if testcase "clone rejects bad patterns - git://"; then
  assert_eq \
    "clone: unsupported protocol: git://github.com/user/project" \
    "$(clone "git://github.com/user/project")"
  assert_eq \
    "clone: unsupported protocol: git://github.com/user/project.git" \
    "$(clone "git://github.com/user/project.git")"
fi

if testcase "clone rejects bad patterns - http://"; then
  assert_eq \
    "clone: unsupported protocol: http://github.com/user/project" \
    "$(clone "http://github.com/user/project")"
  assert_eq \
    "clone: unsupported protocol: http://github.com:8080/user/project" \
    "$(clone "http://github.com:8080/user/project")"
fi

if testcase "clone rejects bad patterns - ftp://"; then
  assert_eq \
    "clone: unsupported protocol: ftp://github.com/user/project" \
    "$(clone "ftp://github.com/user/project")"
  assert_eq \
    "clone: unsupported protocol: ftps://github.com/user/project" \
    "$(clone "ftps://github.com/user/project")"
fi

if testcase "clone rejects path traversal in hostname"; then
  assert_eq \
    "clone: could not extract repository: git@../../etc:user/repo" \
    "$(clone "git@../../etc:user/repo")"
  assert_eq \
    "clone: could not extract repository: git@foo/../bar:user/repo" \
    "$(clone "git@foo/../bar:user/repo")"
  assert_eq \
    "clone: could not extract repository: ssh://anonymous@../../etc/repo" \
    "$(clone "ssh://anonymous@../../etc/repo")"
fi

if testcase "clone rejects path traversal in user"; then
  assert_eq \
    "clone: could not extract repository: git@github.com:../user/repo" \
    "$(clone "git@github.com:../user/repo")"
  assert_eq \
    "clone: could not extract repository: https://github.com/../user/repo" \
    "$(clone "https://github.com/../user/repo")"
  assert_eq \
    "clone: could not extract repository: ssh://..@got.gameoftrees.org/got" \
    "$(clone "ssh://..@got.gameoftrees.org/got")"
fi

if testcase "clone rejects path traversal in repository name"; then
  assert_eq \
    "clone: could not extract repository: git@github.com:user/../repo" \
    "$(clone "git@github.com:user/../repo")"
  assert_eq \
    "clone: could not extract repository: git@github.com:user/repo/../other" \
    "$(clone "git@github.com:user/repo/../other")"
  assert_eq \
    "clone: could not extract repository: ssh://anonymous@got.gameoftrees.org/../got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/../got")"
fi

if testcase "clone rejects path traversal in port"; then
  assert_eq \
    "clone: could not extract repository: ssh://user@host:../22/got" \
    "$(clone "ssh://user@host:../22/got")"
  assert_eq \
    "clone: could not extract repository: https://host:../user/repo" \
    "$(clone "https://host:../user/repo")"
fi

if testcase "clone rejects bad patterns - ssh:// deep path"; then
  assert_eq \
    "clone: could not extract repository: ssh://anonymous@got.gameoftrees.org/a/b/got" \
    "$(clone "ssh://anonymous@got.gameoftrees.org/a/b/got")"
fi
