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
#include <string.h>
#include <unistd.h>

struct repository {
	char *host;
	char *user;
	char *name;
};

int
valid_pattern(char *pattern)
{
	if (strstr(pattern, "@") != NULL) {
		return 0;
	} else {
		return 1;
	}
}

struct repository
extract_repository_from_pattern(char *pattern)
{
	struct repository repository;

	char *host = NULL;
	char *user = NULL;
	char *repository_name = NULL;

	// Host
	char *host_start = strstr(pattern, "@");
	char *host_end = strstr(pattern, ":");
	host = strndup(host_start + 1, host_end - host_start - 1);

	// User or organization
	char *user_start = host_end;
	char *user_end = strstr(pattern, "/");
	user = strndup(user_start + 1, user_end - user_start - 1);

	// Repository name
	char *repository_name_start = user_end;
	char *repository_name_end = strstr(pattern, ".git");
	repository_name = strndup(repository_name_start + 1,
	    repository_name_end - repository_name_start - 1);

	repository.host = host;
	repository.user = user;
	repository.name = repository_name;

	return repository;
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <repository>\n", argv[0]);
		return 1;
	}

	const char *BASE_PROJECT_PATH = "$HOME/src";

	char *pattern = argv[1];
	struct repository repository;

	if (valid_pattern(pattern) == 0) {
		repository = extract_repository_from_pattern(pattern);
	} else {
		fprintf(stderr, "Invalid repository pattern: %s\n", pattern);
		return 1;
	}

	fprintf(stdout, "%s/%s/%s/%s", BASE_PROJECT_PATH, repository.host,
	    repository.user, repository.name);

	return 0;
}
