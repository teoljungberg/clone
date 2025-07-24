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
	if (path[0] != '~')
		return strdup(path);

	char *home = getenv("HOME");
	if (home == NULL)
		return strdup(path);

	size_t home_len = strlen(home);
	size_t path_len = strlen(path);
	size_t total_len = home_len +
	    path_len; // -1 for ~ +1 for null terminator

	char *expanded = malloc(total_len);
	if (expanded == NULL)
		return NULL;

	strcpy(expanded, home);
	strcat(expanded, path + 1); // Skip the ~

	return expanded;
}

char *
get_clone_path(void)
{
	static char *cached_path = NULL;

	if (cached_path != NULL)
		return strdup(cached_path);

	char *expanded = expand_tilde(CLONE_PATH);
	if (expanded == NULL)
		exit(1);

	// Ensure it ends with a slash
	size_t len = strlen(expanded);
	if (len > 0 && expanded[len - 1] != '/') {
		char *with_slash = malloc(len + 2);
		if (with_slash == NULL) {
			free(expanded);
			exit(1);
		}
		strcpy(with_slash, expanded);
		strcat(with_slash, "/");
		free(expanded);
		expanded = with_slash;
	}

	free(cached_path);
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
	char cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return 0;
	else if (strstr(cwd, clone_path) != NULL)
		return 1;
	else
		return 0;
}

int
invalid_repository(struct Repository repository)
{
	if (repository.host == NULL || repository.user == NULL ||
	    repository.name == NULL || repository.protocol == UNDEFINED)
		return 1;
	else
		return 0;
}

int
main(int argc, char *argv[])
{
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

	char *clone_path = get_clone_path();
	char *location, *pattern, *url;
	struct Repository repository = { NULL, NULL, NULL, UNDEFINED };

	if (argc != 1)
		usage();

	pattern = argv[0];

	if (valid_git_clone_url(pattern))
		repository = extract_repository_from_pattern(pattern);
	else if (cwd_is_inside_clone_path(clone_path))
		repository = extract_repository_from_cwd(clone_path, pattern);

	if (invalid_repository(repository)) {
		fprintf(stderr,
		    "Could not extract repository from pattern or cwd: %s\n",
		    pattern);
		free(clone_path);
		free_repository(&repository);
		exit(1);
	}

	location = extract_location_from_repository(clone_path, repository);
	url = extract_url_from_repository(repository);

	if (nflag) {
		fprintf(stdout, "%s %s %s\n", "git clone", url, location);
	} else {
		const char *translated_clone_command[] = { "git", "clone", url,
			location, NULL };
		execvp(translated_clone_command[0],
		    (char *const *)translated_clone_command);
	}

	free(clone_path);
	free(location);
	free(url);
	free_repository(&repository);

	return 0;
}
