#include <fnmatch.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CLONE_H
#define CLONE_H 1

int valid_git_ssh_pattern(char *pattern);
int valid_git_https_pattern(char *pattern);
int valid_git_clone_url(char *pattern);

#endif
