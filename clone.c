#include "clone.h"
#include "repository.h"

static void
usage(void)
{
	fprintf(stderr, "usage: clone [-n] pattern\n");
	exit(2);
}

#ifndef CLONE_PATH
#define CLONE_PATH "~/src"
#endif

char *
expand_tilde(const char *path)
{
	char *expanded, *home, *result;
	size_t home_len, path_len, total_len;

	if (path[0] != '~') {
		result = strdup(path);
		if (result == NULL)
			err(1, NULL);
		return result;
	}

	home = getenv("HOME");
	if (home == NULL || home[0] == '\0') {
		result = strdup(path);
		if (result == NULL)
			err(1, NULL);
		return result;
	}

	home_len = strlen(home);
	path_len = strlen(path);

	/* check for overflow */
	if (home_len > SIZE_MAX - path_len)
		return NULL;
	total_len = home_len + path_len;

	expanded = malloc(total_len);
	if (expanded == NULL)
		return NULL;

	snprintf(expanded, total_len, "%s%s", home, path + 1);

	return expanded;
}

char *
get_clone_path(void)
{
	static char *cached_path = NULL;
	char *expanded;
	size_t len;

	if (cached_path != NULL)
		return strdup(cached_path);

	expanded = expand_tilde(CLONE_PATH);
	if (expanded == NULL)
		err(1, NULL);

	/* strip trailing slash if present */
	len = strlen(expanded);
	if (len > 0 && expanded[len - 1] == '/')
		expanded[len - 1] = '\0';

	cached_path = expanded;
	return strdup(cached_path);
}

int
valid_git_ssh_pattern(char *pattern)
{
	char *valid_patterns[] = {
		"git@*:*/*.git",
		"git@*:*/*",
	};

	for (size_t i = 0; i < sizeof(valid_patterns) /
	    sizeof(valid_patterns[0]); i++) {
		if (fnmatch(valid_patterns[i], pattern, 0) == 0)
			return 1;
	}

	return 0;
}

int
valid_git_https_pattern(char *pattern)
{
	char *valid_patterns[] = {
		"https://*/*/*.git",
		"https://*/*/*",
	};

	for (size_t i = 0; i < sizeof(valid_patterns) /
	    sizeof(valid_patterns[0]); i++) {
		if (fnmatch(valid_patterns[i], pattern, 0) == 0)
			return 1;
	}

	return 0;
}

int
unsupported_git_clone_patterns(char *pattern)
{
	char *invalid_patterns[] = {
		"git://*/*",
		"git://*/*.git",
		"ssh://git@*/*",
		"ssh://git@*/*.git",
	};

	for (size_t i = 0; i < sizeof(invalid_patterns) /
	    sizeof(invalid_patterns[0]); i++) {
		if (fnmatch(invalid_patterns[i], pattern, 0) == 0)
			return 1;
	}

	return 0;
}

int
valid_git_clone_url(char *pattern)
{
	if (valid_git_ssh_pattern(pattern) || valid_git_https_pattern(pattern))
		return 1;
	else if (unsupported_git_clone_patterns(pattern))
		return 1;
	else
		return 0;
}

int
cwd_is_inside_clone_path(char *clone_path)
{
	char cwd[PATH_MAX];
	size_t len;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return 0;

	/* clone_path must match at start of cwd */
	len = strlen(clone_path);
	if (strncmp(cwd, clone_path, len) != 0)
		return 0;

	/* must be followed by '/' or end of string */
	if (cwd[len] != '/' && cwd[len] != '\0')
		return 0;

	return 1;
}

int
invalid_repository(struct Repository repository)
{
	if (repository.host == NULL || repository.user == NULL ||
	    repository.name == NULL || repository.protocol == UNDEFINED)
		return 1;
	if (repository.host[0] == '\0' || repository.user[0] == '\0' ||
	    repository.name[0] == '\0')
		return 1;
	return 0;
}

int
main(int argc, char *argv[])
{
	struct Repository repository = { NULL, NULL, NULL, UNDEFINED };
	char *clone_path, *location, *pattern, *url;
	int nflag = 0;
	int ch;

	while ((ch = getopt(argc, argv, "n")) != -1) {
		switch (ch) {
		case 'n':
			nflag = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	clone_path = get_clone_path();

	if (argc != 1)
		usage();

	pattern = argv[0];

	if (valid_git_clone_url(pattern))
		repository = extract_repository_from_pattern(pattern);
	else if (cwd_is_inside_clone_path(clone_path))
		repository = extract_repository_from_cwd(clone_path, pattern);

	if (invalid_repository(repository))
		errx(1, "could not extract repository: %s", pattern);

	location = extract_location_from_repository(clone_path, repository);
	url = extract_url_from_repository(repository);

	if (location == NULL || url == NULL)
		err(1, NULL);

	if (nflag) {
		fprintf(stdout, "%s %s %s\n", "git clone", url, location);
		free(clone_path);
		free(location);
		free(url);
		free_repository(&repository);
		return 0;
	}

	char *const cmd[] = { "git", "clone", url, location, NULL };

	execvp(cmd[0], cmd);
	err(1, "git");
}
