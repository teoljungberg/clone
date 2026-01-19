#include "clone.h"
#include "repository.h"

/*
 * Find ".git" suffix at end of string.
 * Returns pointer to the suffix, or NULL if not present.
 */
static char *
find_git_suffix(char *str)
{
	size_t len;

	if (str == NULL)
		return NULL;
	len = strlen(str);
	if (len >= 4 && strcmp(str + len - 4, ".git") == 0)
		return str + len - 4;
	return NULL;
}

char *
copy_substring(char *start, char *end)
{
	char *result;

	if (start == NULL || end == NULL || end <= start)
		return NULL;

	result = strndup(start, end - start);
	if (result == NULL)
		err(1, NULL);
	return result;
}

void
extract_repository_from_ssh_pattern(struct Repository *repository,
    char *pattern)
{
	char *at, *colon, *end, *slash;

	repository->protocol = SSH;

	at = strchr(pattern, '@');
	colon = strchr(pattern, ':');
	slash = strchr(pattern, '/');

	if (at != NULL && colon != NULL && at < colon)
		repository->host = copy_substring(at + 1, colon);

	if (colon != NULL && slash != NULL && colon < slash)
		repository->user = copy_substring(colon + 1, slash);

	if (slash != NULL) {
		end = find_git_suffix(slash);
		if (end == NULL)
			end = pattern + strlen(pattern);
		repository->name = copy_substring(slash + 1, end);
	}
}

void
extract_repository_from_https_pattern(struct Repository *repository,
    char *pattern)
{
	char *host_end, *host_start, *name_end, *name_start, *proto, *user_end,
	     *user_start;

	repository->protocol = HTTPS;

	proto = strstr(pattern, "://");
	if (proto == NULL)
		return;
	host_start = proto + 3;
	host_end = strchr(host_start, '/');
	if (host_end == NULL)
		return;
	repository->host = copy_substring(host_start, host_end);

	user_start = host_end + 1;
	user_end = strchr(user_start, '/');
	if (user_end == NULL)
		return;
	repository->user = copy_substring(user_start, user_end);

	name_start = user_end + 1;
	name_end = find_git_suffix(name_start);
	if (name_end == NULL)
		name_end = pattern + strlen(pattern);
	repository->name = copy_substring(name_start, name_end);
}

void
overload_repository_with_pattern(struct Repository *repository, char *pattern)
{
	char *end, *start;

	if (repository == NULL || pattern == NULL)
		return;

	if (fnmatch("*/*", pattern, 0) == 0) {
		/* pattern has both user and repository name */
		start = pattern;
		end = strchr(pattern, '/');
		if (end) {
			free(repository->user);
			repository->user = copy_substring(start, end);

			start = end + 1;
			end = find_git_suffix(start);

			free(repository->name);
			if (end)
				repository->name = copy_substring(start, end);
			else
				repository->name = strdup(start);
			if (repository->name == NULL)
				err(1, NULL);
		}
	} else {
		free(repository->name);
		end = find_git_suffix(pattern);
		if (end)
			repository->name = copy_substring(pattern, end);
		else
			repository->name = strdup(pattern);
		if (repository->name == NULL)
			err(1, NULL);
	}
}

struct Repository
extract_repository_from_pattern(char *pattern)
{
	struct Repository repository = {NULL, NULL, NULL, UNDEFINED};

	if (pattern == NULL)
		return repository;

	if (valid_git_ssh_pattern(pattern))
		extract_repository_from_ssh_pattern(&repository, pattern);
	else if (valid_git_https_pattern(pattern))
		extract_repository_from_https_pattern(&repository, pattern);

	return repository;
}

struct Repository
extract_repository_from_cwd(char *clone_path, char *pattern)
{
	struct Repository repository = {NULL, NULL, NULL, SSH};
	char cwd[PATH_MAX];
	char *end, *start;

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return repository;

	start = strstr(cwd, clone_path);
	if (start == NULL)
		return repository;

	start += strlen(clone_path);
	if (*start == '/')
		start++;
	end = strchr(start, '/');
	if (end) {
		repository.host = copy_substring(start, end);
		start = end + 1;
	} else {
		repository.host = strdup(start);
		if (repository.host == NULL)
			err(1, NULL);
	}

	end = strchr(start, '/');
	if (end) {
		repository.user = copy_substring(start, end);
		start = end + 1;
	} else {
		repository.user = strdup(start);
		if (repository.user == NULL)
			err(1, NULL);
		start = start + strlen(start);
	}

	end = strchr(start, '/');
	if (end == NULL)
		end = find_git_suffix(start);
	if (end != NULL) {
		repository.name = copy_substring(start, end);
	} else {
		repository.name = strdup(start);
		if (repository.name == NULL)
			err(1, NULL);
	}

	overload_repository_with_pattern(&repository, pattern);

	return repository;
}

char *
extract_location_from_repository(char *clone_path, struct Repository repository)
{
	char *out;
	int required_size;

	required_size = snprintf(NULL, 0, "%s/%s/%s/%s", clone_path,
	    repository.host, repository.user, repository.name);
	if (required_size < 0)
		return NULL;

	out = malloc(required_size + 1);
	if (out == NULL)
		return NULL;

	snprintf(out, required_size + 1, "%s/%s/%s/%s", clone_path,
	    repository.host, repository.user, repository.name);

	return out;
}

char *
extract_ssh_url_from_repository(struct Repository repository)
{
	char *out;
	int required_size;

	required_size = snprintf(NULL, 0, "git@%s:%s/%s", repository.host,
	    repository.user, repository.name);
	if (required_size < 0)
		return NULL;

	out = malloc(required_size + 1);
	if (out == NULL)
		return NULL;

	snprintf(out, required_size + 1, "git@%s:%s/%s", repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_https_url_from_repository(struct Repository repository)
{
	char *out;
	int required_size;

	required_size = snprintf(NULL, 0, "https://%s/%s/%s", repository.host,
	    repository.user, repository.name);
	if (required_size < 0)
		return NULL;

	out = malloc(required_size + 1);
	if (out == NULL)
		return NULL;

	snprintf(out, required_size + 1, "https://%s/%s/%s", repository.host,
	    repository.user, repository.name);

	return out;
}

char *
extract_url_from_repository(struct Repository repository)
{
	if (repository.protocol == SSH)
		return extract_ssh_url_from_repository(repository);
	else if (repository.protocol == HTTPS)
		return extract_https_url_from_repository(repository);
	else
		return NULL;
}

void
free_repository(struct Repository *repository)
{
	free(repository->host);
	free(repository->user);
	free(repository->name);
	repository->host = NULL;
	repository->user = NULL;
	repository->name = NULL;
	repository->protocol = UNDEFINED;
}
