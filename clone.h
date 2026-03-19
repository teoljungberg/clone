#ifndef _CLONE_H_
#define _CLONE_H_

#include <sys/param.h>

#include <err.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef __dead
#define __dead __attribute__((__noreturn__))
#endif

#ifndef nitems
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#ifndef __OpenBSD__
#define pledge(promises, execpromises) (0)
#endif

int	valid_git_ssh_pattern(const char *);
int	valid_git_https_pattern(const char *);
int	is_url_pattern(const char *);

#endif /* _CLONE_H_ */
