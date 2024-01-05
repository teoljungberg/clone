#include <fnmatch.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-d] pattern\n", getprogname());
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
		"*@*:*/*.git",
		"*@*:*/*",
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
		"git@*",
	};

	if (fnmatch(url_patterns[0], pattern, 0) == 0 ||
	    fnmatch(url_patterns[1], pattern, 0) == 0)
		return 1;
	else
		return 0;
}

struct Repository
extract_repository_from_pattern(char *pattern)
{
	struct Repository repository;

	char *end, *start;
	start = "";
	end = "";

	// Host
	if ((start = strstr(pattern, "@"))) {
		end = strstr(start, ":");
		repository.host = strndup(start + 1, end - start - 1);
	}

	// User or organization
	if ((start = end)) {
		end = strstr(start, "/");
		repository.user = strndup(start + 1, end - start - 1);
	}

	// Repository name
	if ((start = end)) {
		end = strstr(start, ".git");
		repository.name = strndup(start + 1, end - start - 1);
	}

	return repository;
}

int
cwd_is_inside_clone_path(char *clone_path)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	if (strstr(cwd, clone_path) != NULL)
		return 0;
	else
		return 1;
}

struct Repository
extract_repository_from_cwd(char *clone_path, char *pattern)
{
	struct Repository repository;

	char *end, *start;
	start = "";
	end = "";

	char cwd[1024];
	getcwd(cwd, sizeof(cwd));

	// Host
	if ((start = strstr(cwd, clone_path))) {
		start += strlen(clone_path);
		end = strstr(start, "/");
		repository.host = strndup(start, end - start);
	}

	// User or organization
	if ((start = end)) {
		start += strlen("/");
		end = strstr(start, "/");
		repository.user = strndup(start, end - start);
	}

	// Repository name
	if ((start = end)) {
		start += strlen("/");
		end = strstr(start, "/");
		repository.name = strndup(start, end - start);
	}

	// Add pattern if it exists as:
	//	`user-or-organization / repository-name`
	if (fnmatch("*/*", pattern, 0) == 0) {
		start = pattern;
		end = strstr(pattern, "/");
		repository.user = strndup(start, end - start);

		start = end + 1;
		end = strstr(start, ".git");
		repository.name = strndup(start, end - start);
	} else {
		// If pattern is only a repository name, use the user from the cwd and
		// add the repository name from the pattern
		start = pattern;
		end = strstr(pattern, ".git");
		repository.name = strndup(start, end - start);
	}

	return repository;
}

char *
extract_location_from_repository(char *clone_path, struct Repository repository)
{
	char format[12] = "%s/%s/%s/%s";
	char *out;

	asprintf(&out, format, clone_path, repository.host, repository.user,
	    repository.name);

	return out;
}

char *
extract_url_from_repository(struct Repository repository)
{
	char format[13] = "git@%s:%s/%s";
	char *out;

	asprintf(&out, format, repository.host, repository.user,
	    repository.name);

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

	if (argc < 1)
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
		execvp("clone",
		    (char *const *)translated_clone_command);
	}

	return 0;
}
