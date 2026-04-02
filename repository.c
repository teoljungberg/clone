#include "clone.h"
#include "repository.h"

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

char *
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

struct Repository
extract_repository_from_url(const struct url *url)
{
	struct Repository repository = {NULL, NULL, NULL, NULL, SCHEME_UNDEFINED};
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

void
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

struct Repository
extract_repository_from_cwd(const char *clone_path, const char *pattern)
{
	struct Repository repository = {NULL, NULL, NULL, NULL, SCHEME_SCP};
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

char *
extract_location_from_repository(const char *clone_path,
    struct Repository repository)
{
	char *out;
	int len;
	size_t size;

	len = snprintf(NULL, 0, "%s/%s/%s/%s", clone_path,
	    repository.host, repository.user, repository.name);
	if (len < 0 || (size_t)len > SIZE_MAX - 1)
		return NULL;

	size = (size_t)len + 1;
	out = malloc(size);
	if (out == NULL)
		return NULL;

	snprintf(out, size, "%s/%s/%s/%s", clone_path,
	    repository.host, repository.user, repository.name);

	return out;
}

char *
extract_ssh_url_from_repository(struct Repository repository)
{
	char *out;
	const char *login;
	int len;
	size_t size;

	login = repository.login_user != NULL ? repository.login_user : "git";

	len = snprintf(NULL, 0, "%s@%s:%s/%s", login, repository.host,
	    repository.user, repository.name);
	if (len < 0 || (size_t)len > SIZE_MAX - 1)
		return NULL;

	size = (size_t)len + 1;
	out = malloc(size);
	if (out == NULL)
		return NULL;

	snprintf(out, size, "%s@%s:%s/%s", login, repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_https_url_from_repository(struct Repository repository)
{
	char *out;
	int len;
	size_t size;

	len = snprintf(NULL, 0, "https://%s/%s/%s", repository.host,
	    repository.user, repository.name);
	if (len < 0 || (size_t)len > SIZE_MAX - 1)
		return NULL;

	size = (size_t)len + 1;
	out = malloc(size);
	if (out == NULL)
		return NULL;

	snprintf(out, size, "https://%s/%s/%s", repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_ssh_url_string_from_repository(struct Repository repository)
{
	char *out;
	int len;
	size_t size;

	len = snprintf(NULL, 0, "ssh://%s@%s/%s", repository.user,
	    repository.host, repository.name);
	if (len < 0 || (size_t)len > SIZE_MAX - 1)
		return NULL;

	size = (size_t)len + 1;
	out = malloc(size);
	if (out == NULL)
		return NULL;

	snprintf(out, size, "ssh://%s@%s/%s", repository.user,
	    repository.host, repository.name);

	return out;
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
	repository->host = NULL;
	repository->user = NULL;
	repository->name = NULL;
	repository->login_user = NULL;
	repository->scheme = SCHEME_UNDEFINED;
}
