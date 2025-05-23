#include "clone.h"
#include "repository.h"

static void
usage(void)
{
	fprintf(stderr, "usage: clone [-n] pattern\n");
	exit(2);
}

char *
get_clone_path(void)
{
	char *home = strdup(getenv("HOME"));
	if (home == NULL)
		exit(1);
	
	size_t clone_path_len = strlen(home) + strlen("/src/") + 1;
	char *clone_path = malloc(clone_path_len);

	if (clone_path == NULL) {
		free(home);
		exit(1);
	}

	snprintf(clone_path, clone_path_len, "%s/src/", home);
	free(home);
	return clone_path;
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
