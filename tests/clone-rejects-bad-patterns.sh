if testcase "clone rejects bad git:// patterns"; then
  assert_eq \
    "Invalid repository pattern or cwd: git://github.com/user/project" \
    "$(clone "git://github.com/user/project")"
  assert_eq \
    "Invalid repository pattern or cwd: git://github.com/user/project.git" \
    "$(clone "git://github.com/user/project.git")"
fi

if testcase "clone rejects bad ssh:// patterns"; then
  assert_eq \
    "Invalid repository pattern or cwd: ssh://git@github.com:user/project" \
    "$(clone "ssh://git@github.com:user/project")"
  assert_eq \
    "Invalid repository pattern or cwd: ssh://git@github.com:user/project.git" \
    "$(clone "ssh://git@github.com:user/project.git")"
fi
