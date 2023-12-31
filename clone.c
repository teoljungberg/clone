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
cwd_is_inside_base_project_path(char *base_project_path)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if (strstr(cwd, base_project_path) != NULL) {
		return 0;
	} else {
		return 1;
	}
}

struct Repository
extract_repository_from_cwd(char *base_project_path, char *pattern)
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
	if ((start = strstr(cwd, base_project_path))) {
		start += strlen(base_project_path);
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
	if (repository.host != NULL ||
	    repository.user != NULL ||
	    repository.name != NULL) {
		return 0;
	} else {
		return 1;
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <repository>\n", argv[0]);
		return 1;
	}

	char *pattern = argv[1];
	char *base_project_path = getenv("HOME");
	strcat(base_project_path, "/src/");
	struct Repository repository;

	if (valid_pattern(pattern) == 0) {
		repository = extract_repository_from_pattern(pattern);
	} else if (cwd_is_inside_base_project_path(base_project_path) == 0) {
		repository = extract_repository_from_cwd(base_project_path,
		    pattern);
	} else {
		fprintf(stderr, "Invalid repository pattern: %s\n", pattern);
		return 1;
	}

	if (valid_repository(repository) == 0) {
		fprintf(stdout, "%s/%s/%s/%s\n", base_project_path,
		    repository.host, repository.user, repository.name);
	} else {
		fprintf(stderr, "Invalid repository pattern: %s\n", pattern);
		return 1;
	}

	return 0;
}
