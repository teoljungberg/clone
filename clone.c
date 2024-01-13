#include <fnmatch.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: clone [-n] pattern\n");
	exit(2);
}

struct Repository {
	char *host;
	char *user;
	char *name;
};

int
invalid_repository(struct Repository repository)
{
	if (repository.host == NULL || repository.user == NULL ||
	    repository.name == NULL)
		return 1;
	else
		return 0;
}

char *
get_clone_path(void)
{
	char *clone_path = getenv("HOME");
	strcat(clone_path, "/src/");

	return clone_path;
}

int
valid_git_clone_url(char *pattern)
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

char *
copy_substring(char *start, char *end)
{
	if (!start || !end || end <= start)
		return NULL;

	return strndup(start, end - start);
}

struct Repository
extract_repository_from_pattern(char *pattern)
{
	struct Repository repository = {NULL, NULL, NULL};

	if (!pattern)
		return repository;

	char *at = strchr(pattern, '@');
	char *colon = strchr(pattern, ':');
	char *slash = strchr(pattern, '/');

	if (at && colon && at < colon)
		repository.host = copy_substring(at + 1, colon);

	if (colon && slash && colon < slash)
		repository.user = copy_substring(colon + 1, slash);

	if (slash) {
		char *end = strstr(slash, ".git");
		if (!end)
			end = pattern + strlen(pattern);
		repository.name = copy_substring(slash + 1, end);
	}

	return repository;
}

int
cwd_is_inside_clone_path(char *clone_path)
{
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return 0;

	if (strstr(cwd, clone_path) != NULL)
		return 1;
	else
		return 0;
}

void
overload_repository_with_pattern(struct Repository *repository, char *pattern)
{
	if (!repository || !pattern)
		return;

	char *end, *start;

	if (fnmatch("*/*", pattern, 0) == 0) {
		// Pattern has both user and repository name
		start = pattern;
		end = strchr(pattern, '/');
		if (end) {
			free(repository->user);
			repository->user = copy_substring(start, end);

			start = end + 1;
			end = strstr(start, ".git");

			free(repository->name);
			if (end)
				repository->name = copy_substring(start, end);
			else
				repository->name = strdup(start);
		}
	} else {
		// Pattern has only repository name
		free(repository->name);
		end = strstr(pattern, ".git");
		if (end)
			repository->name = copy_substring(pattern, end);
		else
			repository->name = strdup(pattern);
	}
}

struct Repository
extract_repository_from_cwd(char *clone_path, char *pattern)
{
	struct Repository repository = {NULL, NULL, NULL};

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return repository;

	char *start = strstr(cwd, clone_path);
	if (!start)
		return repository;
	start += strlen(clone_path);

	char *end = strchr(start, '/');
	if (end) {
		repository.host = copy_substring(start, end);
		start = end + 1;
	}

	end = strchr(start, '/');
	if (end) {
		repository.user = copy_substring(start, end);
		start = end + 1;
	}

	end = strchr(start, '/');
	if (!end)
		end = strstr(start, ".git");
	if (end)
		repository.name = copy_substring(start, end);
	else
		repository.name = strdup(start);

	overload_repository_with_pattern(&repository, pattern);

	return repository;
}

char *
extract_location_from_repository(char *clone_path, struct Repository repository)
{
	char format[12] = "%s/%s/%s/%s";
	int required_size;

	required_size = snprintf(NULL, 0, format, clone_path, repository.host,
	    repository.user, repository.name);

	if (required_size < 0)
		return NULL;

	char *out = malloc(required_size + 1);
	if (!out)
		return NULL;

	snprintf(out, required_size + 1, format, clone_path, repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_url_from_repository(struct Repository repository)
{
	char format[13] = "git@%s:%s/%s";
	int required_size;

	required_size = snprintf(NULL, 0, format, repository.host,
	    repository.user, repository.name);

	if (required_size < 0)
		return NULL;

	char *out = malloc(required_size + 1);
	if (!out)
		return NULL;

	snprintf(out, required_size + 1, format, repository.host,
	    repository.user, repository.name);

	return out;
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
	struct Repository repository;

	if (argc != 1)
		usage();

	pattern = argv[0];

	if (valid_git_clone_url(pattern)) {
		repository = extract_repository_from_pattern(pattern);
	} else if (cwd_is_inside_clone_path(clone_path)) {
		repository = extract_repository_from_cwd(clone_path, pattern);
	} else {
		fprintf(stderr, "Invalid repository pattern or cwd: %s\n",
		    pattern);
		exit(1);
	}

	if (invalid_repository(repository)) {
		fprintf(stderr, "Could not extract repository from %s or cwd\n",
		    pattern);
		exit(1);
	}

	location = extract_location_from_repository(clone_path, repository);
	url = extract_url_from_repository(repository);

	if (nflag) {
		fprintf(stdout, "%s %s %s\n", "git clone", url,
		    location);
	} else {
		const char *translated_clone_command[] = { "git",
			"clone", url, location, NULL };
		execvp(translated_clone_command[0],
		    (char *const *)translated_clone_command);
	}

	free(location);
	free(url);
	free(repository.host);
	free(repository.user);
	free(repository.name);

	return 0;
}
