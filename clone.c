#include "clone.h"
#include "repository.h"
#include "url.h"

extern char *__progname;

static void __dead
usage(void)
{
	fprintf(stderr, "usage: %s [-n] pattern\n", __progname);
	exit(1);
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
		errx(1, "path too long");
	total_len = home_len + path_len;

	expanded = malloc(total_len);
	if (expanded == NULL)
		err(1, NULL);

	snprintf(expanded, total_len, "%s%s", home, path + 1);

	return expanded;
}

char *
get_clone_path(void)
{
	static char *cached_path = NULL;
	char *expanded, *resolved, *result;
	size_t len;

	if (cached_path != NULL) {
		result = strdup(cached_path);
		if (result == NULL)
			err(1, NULL);
		return result;
	}

	expanded = expand_tilde(CLONE_PATH);
	if (expanded == NULL)
		err(1, NULL);

	/* strip trailing slash if present */
	len = strlen(expanded);
	if (len > 0 && expanded[len - 1] == '/')
		expanded[len - 1] = '\0';

	/* resolve symlinks so getcwd() comparisons work */
	resolved = realpath(expanded, NULL);
	if (resolved != NULL) {
		free(expanded);
		expanded = resolved;
	}

	cached_path = expanded;
	result = strdup(cached_path);
	if (result == NULL)
		err(1, NULL);
	return result;
}

int
cwd_is_inside_clone_path(const char *clone_path)
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

static int
contains_path_traversal(const char *str)
{
	if (str == NULL)
		return 1;
	if (strstr(str, "..") != NULL)
		return 1;
	if (strchr(str, '/') != NULL)
		return 1;
	if (strchr(str, ':') != NULL)
		return 1;
	return 0;
}

int
invalid_repository(struct Repository repository)
{
	if (repository.host == NULL || repository.user == NULL ||
	    repository.name == NULL || repository.scheme == SCHEME_UNDEFINED)
		return 1;
	if (repository.host[0] == '\0' || repository.user[0] == '\0' ||
	    repository.name[0] == '\0')
		return 1;
	if (contains_path_traversal(repository.host) ||
	    contains_path_traversal(repository.user) ||
	    contains_path_traversal(repository.name))
		return 1;
	if (repository.port != NULL &&
	    contains_path_traversal(repository.port))
		return 1;
	return 0;
}

int
main(int argc, char *argv[])
{
	struct Repository repository = {0};
	struct url parsed = {0};
	char *cmd[] = { "git", "clone", NULL, NULL, NULL };
	char *clone_path, *clone_url, *location, *pattern;
	int nflag = 0;
	int ch;

	if (pledge("stdio rpath exec", NULL) == -1)
		err(1, "pledge");

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

	if (parse_url(pattern, &parsed) == 0) {
		if (parsed.scheme == SCHEME_GIT ||
		    parsed.scheme == SCHEME_HTTP ||
		    parsed.scheme == SCHEME_FTP) {
			free_url(&parsed);
			errx(1, "unsupported protocol: %s",
			    pattern);
		}
		repository = extract_repository_from_url(&parsed);
		free_url(&parsed);
	} else if (cwd_is_inside_clone_path(clone_path)) {
		repository = extract_repository_from_cwd(clone_path, pattern);
	}

	if (invalid_repository(repository)) {
		free(clone_path);
		free_repository(&repository);
		errx(1, "could not extract repository: %s", pattern);
	}

	location = extract_location_from_repository(clone_path, repository);
	clone_url = extract_url_from_repository(repository);

	if (location == NULL || clone_url == NULL) {
		free(clone_path);
		free(location);
		free(clone_url);
		free_repository(&repository);
		err(1, NULL);
	}

	if (nflag) {
		fprintf(stdout, "%s %s %s\n", "git clone", clone_url, location);
		free(clone_path);
		free(location);
		free(clone_url);
		free_repository(&repository);
		return 0;
	}

	cmd[2] = clone_url;
	cmd[3] = location;
	execvp(cmd[0], cmd);
	err(1, "git");
}
