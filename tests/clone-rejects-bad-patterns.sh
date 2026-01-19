if testcase "clone rejects bad patterns - git://"; then
  assert_eq \
    "clone: could not extract repository: git://github.com/user/project" \
    "$(clone "git://github.com/user/project")"
  assert_eq \
    "clone: could not extract repository: git://github.com/user/project.git" \
    "$(clone "git://github.com/user/project.git")"
fi

if testcase "clone rejects bad patterns - ssh://"; then
  assert_eq \
    "clone: could not extract repository: ssh://git@github.com:user/project" \
    "$(clone "ssh://git@github.com:user/project")"
  assert_eq \
    "clone: could not extract repository: ssh://git@github.com:user/project.git" \
    "$(clone "ssh://git@github.com:user/project.git")"
fi

if testcase "clone rejects path traversal in hostname"; then
  assert_eq \
    "clone: could not extract repository: git@../../etc:user/repo" \
    "$(clone "git@../../etc:user/repo")"
  assert_eq \
    "clone: could not extract repository: git@foo/../bar:user/repo" \
    "$(clone "git@foo/../bar:user/repo")"
fi

if testcase "clone rejects path traversal in user"; then
  assert_eq \
    "clone: could not extract repository: git@github.com:../user/repo" \
    "$(clone "git@github.com:../user/repo")"
  assert_eq \
    "clone: could not extract repository: https://github.com/../user/repo" \
    "$(clone "https://github.com/../user/repo")"
fi

if testcase "clone rejects path traversal in repository name"; then
  assert_eq \
    "clone: could not extract repository: git@github.com:user/../repo" \
    "$(clone "git@github.com:user/../repo")"
  assert_eq \
    "clone: could not extract repository: git@github.com:user/repo/../other" \
    "$(clone "git@github.com:user/repo/../other")"
fi
