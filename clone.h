#ifndef _CLONE_H_
#define _CLONE_H_

#include <err.h>
#include <fnmatch.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int	valid_git_ssh_pattern(char *);
int	valid_git_https_pattern(char *);
int	valid_git_clone_url(char *);

#endif /* _CLONE_H_ */
