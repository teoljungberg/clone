#include "repository.h"

#include <err.h>
#include <fnmatch.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GIT_SUFFIX	".git"
#define GIT_SUFFIX_LEN	(sizeof(GIT_SUFFIX) - 1)

/*
 * Find ".git" suffix at end of string.
 * Returns pointer to the suffix, or NULL if not present.
 */
static const char *
find_git_suffix(const char *str)
{
	size_t len;

	if (str == NULL)
		return NULL;
	len = strlen(str);
	if (len >= GIT_SUFFIX_LEN &&
	    strcmp(str + len - GIT_SUFFIX_LEN, GIT_SUFFIX) == 0)
		return str + len - GIT_SUFFIX_LEN;
	return NULL;
}

static char *
copy_substring(const char *start, const char *end)
{
	char *result;

	if (start == NULL || end == NULL || end <= start)
		return NULL;

	result = strndup(start, end - start);
	if (result == NULL)
		err(1, NULL);
	return result;
}

static struct Repository
extract_repository_from_url(const struct url *url)
{
	struct Repository repository = {0};
	const char *name_start, *slash, *suffix;

	if (url == NULL)
		return repository;

	if (url->scheme != SCHEME_SCP && url->scheme != SCHEME_HTTPS &&
	    url->scheme != SCHEME_SSH)
		return repository;

	repository.scheme = url->scheme;

	if (url->host != NULL) {
		repository.host = strdup(url->host);
		if (repository.host == NULL)
			err(1, NULL);
	}

	if (url->port != NULL) {
		repository.port = strdup(url->port);
		if (repository.port == NULL)
			err(1, NULL);
	}

	if (url->path == NULL)
		return repository;

	if (url->scheme == SCHEME_SSH) {
		/* ssh://user@host/repo -- user from URL, name from path */
		if (url->user != NULL) {
			repository.user = strdup(url->user);
			if (repository.user == NULL)
				err(1, NULL);
		}
		suffix = find_git_suffix(url->path);
		if (suffix != NULL) {
			repository.name = strndup(url->path,
			    suffix - url->path);
		} else {
			repository.name = strdup(url->path);
		}
		if (repository.name == NULL)
			err(1, NULL);
		return repository;
	}

	/* SCP: preserve SSH login user for URL reconstruction */
	if (url->scheme == SCHEME_SCP && url->user != NULL) {
		repository.login_user = strdup(url->user);
		if (repository.login_user == NULL)
			err(1, NULL);
	}

	/* SCP and HTTPS: path is user/repo */
	slash = strchr(url->path, '/');
	if (slash == NULL)
		return repository;

	repository.user = strndup(url->path, slash - url->path);
	if (repository.user == NULL)
		err(1, NULL);

	name_start = slash + 1;
	suffix = find_git_suffix(name_start);
	if (suffix != NULL)
		repository.name = strndup(name_start, suffix - name_start);
	else
		repository.name = strdup(name_start);
	if (repository.name == NULL)
		err(1, NULL);

	return repository;
}

static void
overload_repository_with_pattern(struct Repository *repository,
    const char *pattern)
{
	const char *end, *start;

	if (repository == NULL || pattern == NULL)
		return;

	if (fnmatch("*/*", pattern, 0) == 0) {
		/* pattern has both user and repository name */
		start = pattern;
		end = strchr(pattern, '/');
		if (end != NULL) {
			free(repository->user);
			repository->user = copy_substring(start, end);

			start = end + 1;
			end = find_git_suffix(start);

			free(repository->name);
			if (end != NULL)
				repository->name = copy_substring(start, end);
			else
				repository->name = strdup(start);
			if (repository->name == NULL)
				err(1, NULL);
		}
	} else {
		free(repository->name);
		end = find_git_suffix(pattern);
		if (end != NULL)
			repository->name = copy_substring(pattern, end);
		else
			repository->name = strdup(pattern);
		if (repository->name == NULL)
			err(1, NULL);
	}
}

static struct Repository
extract_repository_from_cwd(const char *clone_path, const char *pattern)
{
	struct Repository repository = {.scheme = SCHEME_SCP};
	char cwd[PATH_MAX];
	char *end, *start;
	const char *suffix;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return repository;

	start = strstr(cwd, clone_path);
	if (start == NULL)
		return repository;

	start += strlen(clone_path);
	if (*start == '/')
		start++;
	end = strchr(start, '/');
	if (end != NULL) {
		repository.host = copy_substring(start, end);
		start = end + 1;
	} else {
		repository.host = strdup(start);
		if (repository.host == NULL)
			err(1, NULL);
	}

	end = strchr(start, '/');
	if (end != NULL) {
		repository.user = copy_substring(start, end);
		start = end + 1;
	} else {
		repository.user = strdup(start);
		if (repository.user == NULL)
			err(1, NULL);
		start = start + strlen(start);
	}

	end = strchr(start, '/');
	if (end != NULL) {
		repository.name = copy_substring(start, end);
	} else {
		suffix = find_git_suffix(start);
		if (suffix != NULL) {
			repository.name = copy_substring(start, suffix);
		} else {
			repository.name = strdup(start);
			if (repository.name == NULL)
				err(1, NULL);
		}
	}

	overload_repository_with_pattern(&repository, pattern);

	return repository;
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

static int
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

int
extract_repository(const char *clone_path, const char *pattern,
    struct Repository *repository)
{
	struct url parsed = {0};

	*repository = (struct Repository){0};

	if (parse_url(pattern, &parsed) == 0) {
		if (invalid_scheme(parsed.scheme)) {
			free_url(&parsed);
			return -1;
		}
		*repository = extract_repository_from_url(&parsed);
		free_url(&parsed);
		return 0;
	}

	if (cwd_is_inside_clone_path(clone_path)) {
		*repository = extract_repository_from_cwd(clone_path, pattern);
		return 0;
	}

	return 0;
}

static char *
format_string(const char *fmt, ...)
{
	va_list ap;
	int len;
	size_t size;
	char *out;

	va_start(ap, fmt);
	len = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	if (len < 0 || (size_t)len > SIZE_MAX - 1)
		return NULL;

	size = (size_t)len + 1;
	out = malloc(size);
	if (out == NULL)
		return NULL;

	va_start(ap, fmt);
	vsnprintf(out, size, fmt, ap);
	va_end(ap);

	return out;
}

char *
extract_location_from_repository(const char *clone_path,
    struct Repository repository)
{
	return format_string("%s/%s/%s/%s", clone_path,
	    repository.host, repository.user, repository.name);
}

static char *
extract_ssh_url_from_repository(struct Repository repository)
{
	const char *login;

	login = repository.login_user != NULL ? repository.login_user : "git";

	return format_string("%s@%s:%s/%s", login, repository.host,
	    repository.user, repository.name);
}

static char *
extract_https_url_from_repository(struct Repository repository)
{
	if (repository.port != NULL)
		return format_string("https://%s:%s/%s/%s",
		    repository.host, repository.port, repository.user,
		    repository.name);
	else
		return format_string("https://%s/%s/%s",
		    repository.host, repository.user, repository.name);
}

static char *
extract_ssh_url_string_from_repository(struct Repository repository)
{
	if (repository.port != NULL)
		return format_string("ssh://%s@%s:%s/%s",
		    repository.user, repository.host, repository.port,
		    repository.name);
	else
		return format_string("ssh://%s@%s/%s",
		    repository.user, repository.host, repository.name);
}

char *
extract_url_from_repository(struct Repository repository)
{
	if (repository.scheme == SCHEME_SCP)
		return extract_ssh_url_from_repository(repository);
	else if (repository.scheme == SCHEME_HTTPS)
		return extract_https_url_from_repository(repository);
	else if (repository.scheme == SCHEME_SSH)
		return extract_ssh_url_string_from_repository(repository);
	else
		return NULL;
}

void
free_repository(struct Repository *repository)
{
	free(repository->host);
	free(repository->user);
	free(repository->name);
	free(repository->login_user);
	free(repository->port);
	repository->host = NULL;
	repository->user = NULL;
	repository->name = NULL;
	repository->login_user = NULL;
	repository->port = NULL;
	repository->scheme = SCHEME_UNDEFINED;
}
