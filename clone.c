#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Repository {
	char *host;
	char *user;
	char *name;
};

char *
get_clone_path(void)
{
	char *clone_path = getenv("HOME");
	strcat(clone_path, "/src/");

	return clone_path;
}

int
valid_pattern(char *pattern)
{
	char *valid_patterns[] = {
		"*@*:*/*.git",
		"*@*:*/*",
	};

	if (fnmatch(valid_patterns[0], pattern, 0) == 0 ||
	    fnmatch(valid_patterns[1], pattern, 0) == 0) {
		return 0;
	} else {
		return 1;
	}
}

struct Repository
extract_repository_from_pattern(char *pattern)
{
	struct Repository repository;

	char *host = NULL;
	char *user = NULL;
	char *repository_name = NULL;

	char *start, *end;
	start = "";
	end = "";

	// Host
	if ((start = strstr(pattern, "@"))) {
		end = strstr(start, ":");
		host = strndup(start + 1, end - start - 1);
	}

	// User or organization
	if ((start = end)) {
		end = strstr(start, "/");
		user = strndup(start + 1, end - start - 1);
	}

	// Repository name
	if ((start = end)) {
		end = strstr(start, ".git");
		repository_name = strndup(start + 1, end - start - 1);
	}

	repository.host = host;
	repository.user = user;
	repository.name = repository_name;

	return repository;
}

int
cwd_is_inside_clone_path(char *clone_path)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if (strstr(cwd, clone_path) != NULL) {
		return 0;
	} else {
		return 1;
	}
}

struct Repository
extract_repository_from_cwd(char *clone_path, char *pattern)
{
	struct Repository repository;

	char *host = NULL;
	char *user = NULL;
	char *repository_name = NULL;

	char *start, *end;
	start = "";
	end = "";

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	// Host
	if ((start = strstr(cwd, clone_path))) {
		start += strlen(clone_path);
		end = strstr(start, "/");
		host = strndup(start, end - start);
	}

	// User or organization
	if ((start = end)) {
		start += strlen("/");
		end = strstr(start, "/");
		user = strndup(start, end - start);
	}

	// Repository name
	if ((start = end)) {
		start += strlen("/");
		end = strstr(start, "/");
		repository_name = strndup(start, end - start);
	}

	// Add pattern if it exists as:
	//	`user-or-organization / repository-name`
	if (fnmatch("*/*", pattern, 0) == 0) {
		start = pattern;
		end = strstr(pattern, "/");
		user = strndup(start, end - start);

		start = end + 1;
		end = strstr(start, ".git");
		repository_name = strndup(start, end - start);
	} else {
		// If pattern is only a repository name, use the user from the cwd and
		// add the repository name from the pattern
		start = pattern;
		end = strstr(pattern, ".git");
		repository_name = strndup(start, end - start);
	}

	repository.host = host;
	repository.user = user;
	repository.name = repository_name;

	return repository;
}

int
valid_repository(struct Repository repository)
{
	if (repository.host != NULL || repository.user != NULL ||
	    repository.name != NULL) {
		return 0;
	} else {
		return 1;
	}
}

char *
extract_location_from_repository(char *clone_path, struct Repository repository)
{
	int size = snprintf(NULL, 0, "%s/%s/%s/%s", clone_path, repository.host,
	    repository.user, repository.name) + 1;
	char *out = malloc(size);

	snprintf(out, size, "%s/%s/%s/%s", clone_path, repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_url_from_repository(struct Repository repository)
{
	int size = snprintf(NULL, 0, "git@%s/%s/%s", repository.host,
	    repository.user, repository.name) + 1;
	char *out = malloc(size);

	snprintf(out, size, "git@%s:%s/%s", repository.host, repository.user,
	    repository.name);

	return out;
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <repository>\n", argv[0]);
		return 1;
	}

	char *pattern = argv[1];
	char *clone_path = get_clone_path();
	struct Repository repository;
	char *location;
	char *url;

	if (valid_pattern(pattern) == 0) {
		repository = extract_repository_from_pattern(pattern);
	} else if (cwd_is_inside_clone_path(clone_path) == 0) {
		repository = extract_repository_from_cwd(clone_path, pattern);
	} else {
		fprintf(stderr, "Invalid repository pattern: %s\n", pattern);
		return 1;
	}

	if (valid_repository(repository) == 0) {
		location = extract_location_from_repository(clone_path,
		    repository);
		url = extract_url_from_repository(repository);

		fprintf(stdout, "%s %s", url, location);
	} else {
		fprintf(stderr, "Invalid repository pattern: %s\n", pattern);
		return 1;
	}

	return 0;
}
