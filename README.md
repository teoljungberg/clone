# clone

## Usage

clone(1) little program in C that converts the pattern of any of the following:

```
git@github.com:user/repository
git@github.com:user/repository.git
user/repository
```

Into a local path of the following form and prints it to stdout:

```
$HOME/src/github.com/user/repository
```

It also handles different git hosts, such as gitlab.com, bitbucket.org, etc.

```
git@host:user/repository
```

Converts to:

```
$HOME/src/host/user/repository
```

E.g. to clone(1) a repository from git.sr.ht, it converts:

```
git@git:~user/repository
```

Into the following path:

```
$HOME/src/git.sr.ht/~user/repository
```

### Directory aware

clone(1) is directory aware, as in clone(1) is executed inside a project
structure, and a shortcut to a clone(1) command is given. Then the project path
structure is used to clone the project

Example:

```
% pwd
$HOME/src/github.com/example
% clone another-project
```

Then the command returned would be:

```
$HOME/src/github.com/example/another-project
```
