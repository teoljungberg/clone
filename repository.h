#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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

int invalid_repository(struct Repository repository);

char *copy_substring(char *start, char *end);

struct Repository extract_repository_from_pattern(char *pattern);

void overload_repository_with_pattern(struct Repository *repository, char
    *pattern);

struct Repository extract_repository_from_cwd(char *clone_path, char *pattern);

char *extract_location_from_repository(char *clone_path, struct Repository
    repository);
char *extract_url_from_repository(struct Repository repository);
