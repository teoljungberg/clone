// a little program in C that converts the pattern of any of the following:
//
// git@github.com:user/repository
// git@github.com:user/repository.git
// user/repository
//
// into a local path of the following form and prints it to stdout:
//
// $HOME/src/github.com/user/repository
//
// It also handles different git hosts, such as gitlab.com, bitbucket.org, etc.
//
// git@host:user/repository
//
// Converts to:
//
// $HOME/src/host/user/repository
//
// E.g. to clone a repository from git.sr.ht, it converts:
//
// git@git:~user/repository
//
// Into the following path:
//
// $HOME/src/git.sr.ht/~user/repository

#include <stdio.h>

int
main()
{
	printf("Hello, world!\n");
	return 0;
}
