# clone

git-clone(1) repositories into a pre-determined directory structure.

The default structure is `~/src/host/user/repository`, but the clone path is fully configurable at compile time.

## Usage

Using the `-n` flag for demonstration:

```
% clone -n git@github.com:user/repository
git clone git@github.com:/user/repository /home/user/src/github.com/user/repository
```

or

```
% cd /home/user/src/github.com/user
% clone -n another-repository
git clone git@github.com:/user/another-repository /home/user/src/github.com/user/another-repository
```

or

```
% cd /home/user/src/github.com/
/home/user/src/github.com/
% clone -n user/repository
git clone git@github.com:/user/repository /home/user/src/github.com/user/repository
```

### CDPATH

It is recommended to set the `CDPATH` environment variable to the different
hosts you have repositories from. This allows for quick access and quick cloning
due to the known directory structure of clone(1).

```
% CDPATH=$HOME/src/github.com
% cd user
~/src/github.com/user
% clone -n repository
git clone git@github.com:/user/repository /home/user/src/github.com/user/repository
```

Note: If you've configured a custom `CLONE_PATH`, adjust your `CDPATH` accordingly:

```
% CDPATH=$HOME/projects/github.com  # If using CLONE_PATH=~/projects
```

## Configuration

The default clone path is `~/src`, but it can be customized at compile time:

Custom path, clones to `~/projects/github.com/user/repository`

```
% make CLONE_PATH=~/projects
```

Nested path, clones to `~/dev/code/github.com/user/repository`

```
% make CLONE_PATH=~/dev/code
```

Absolute path, clones to `/opt/repositories/github.com/user/repository`

```
% make CLONE_PATH=/opt/repositories
```

## Installation

### UNIX

```
# make install
```

### Nix

```
% nix-env -i -f .
```

## Help

```
% man clone
```
