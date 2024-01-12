#include <fnmatch.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: clone [-d] pattern\n");
	exit(2);
}

struct Repository {
	char *host;
	char *user;
	char *name;
};

int
valid_repository(struct Repository repository)
{
	if (repository.host != NULL || repository.user != NULL ||
	    repository.name != NULL)
		return 0;
	else
		return 1;
}

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
		"git@*:*/*.git",
		"git@*:*/*",
	};

	if (fnmatch(valid_patterns[0], pattern, 0) == 0 ||
	    fnmatch(valid_patterns[1], pattern, 0) == 0)
		return 0;
	else
		return 1;
}

int
non_url_pattern(char *pattern)
{
	char *url_patterns[] = {
		"https://*",
		"git://*",
	};

	if (fnmatch(url_patterns[0], pattern, 0) == 0 ||
	    fnmatch(url_patterns[1], pattern, 0) == 0)
		return 1;
	else
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
		return 1;

	if (strstr(cwd, clone_path) != NULL)
		return 0;
	else
		return 1;
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
	repository.name = end ? copy_substring(start, end) : strdup(start);

	overload_repository_with_pattern(&repository, pattern);

	return repository;
}

char *
extract_location_from_repository(char *clone_path, struct Repository repository)
{
	char format[12] = "%s/%s/%s/%s";
	char *out;
	int asprintf_result;

	asprintf_result = asprintf(&out, format, clone_path, repository.host,
	    repository.user,
	    repository.name);

	if (asprintf_result == -1)
		return NULL;

	return out;
}

char *
extract_url_from_repository(struct Repository repository)
{
	char format[13] = "git@%s:%s/%s";
	char *out;
	int asprintf_result;

	asprintf_result = asprintf(&out, format, repository.host,
	    repository.user,
	    repository.name);

	if (asprintf_result == -1)
		return NULL;

	return out;
}

int
main(int argc, char *argv[])
{
	int dflag = 0;
	int ch;

	while ((ch = getopt(argc, argv, "d")) != -1) {
		switch (ch) {
		case 'd':
			dflag = 1;
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

	if (valid_pattern(pattern) == 0) {
		repository = extract_repository_from_pattern(pattern);
	} else if (cwd_is_inside_clone_path(clone_path) == 0 &&
	    non_url_pattern(pattern) == 0) {
		repository = extract_repository_from_cwd(clone_path, pattern);
	} else {
		fprintf(stderr, "Invalid repository pattern or cwd: %s\n",
		    pattern);
		exit(1);
	}

	if (valid_repository(repository) != 0) {
		fprintf(stderr, "Could not extract repository from %s or pwd",
		    pattern);
		exit(1);
	}

	location = extract_location_from_repository(clone_path,
	    repository);
	url = extract_url_from_repository(repository);

	if (dflag == 1) {
		fprintf(stdout, "%s %s %s\n", "git clone", url,
		    location);
	} else {
		const char *translated_clone_command[] = { "git",
			"clone", url, location, NULL };
		execvp(translated_clone_command[0],
		    (char *const *)translated_clone_command);
	}

	return 0;
}
