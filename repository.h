#include <fnmatch.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#ifndef REPOSITORY_H
#define REPOSITORY_H 1

enum Protocol {
	UNDEFINED,
	SSH,
	HTTPS,
};

struct Repository {
	char *host;
	char *user;
	char *name;
	enum Protocol protocol;
};

struct Repository extract_repository_from_pattern(char *pattern);
struct Repository extract_repository_from_cwd(char *clone_path, char *pattern);

void overload_repository_with_pattern(struct Repository *repository, char
    *pattern);

char *extract_location_from_repository(char *clone_path, struct Repository
    repository);
char *extract_url_from_repository(struct Repository repository);

void free_repository(struct Repository *repository);

#endif
