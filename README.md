# clone

git-clone(1) repositories into a pre-determined directory structure.

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
