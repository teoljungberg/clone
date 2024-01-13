#include <fnmatch.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int valid_git_ssh_pattern(char *pattern);
int valid_git_https_pattern(char *pattern);
int valid_git_clone_url(char *pattern);
