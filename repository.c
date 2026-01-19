#include "clone.h"
#include "repository.h"

char *
copy_substring(char *start, char *end)
{
	if (!start || !end || end <= start)
		return NULL;

	char *result = strndup(start, end - start);
	if (result == NULL)
		err(1, NULL);
	return result;
}

void
extract_repository_from_ssh_pattern(struct Repository *repository,
    char *pattern)
{
	repository->protocol = SSH;

	char *at = strchr(pattern, '@');
	char *colon = strchr(pattern, ':');
	char *slash = strchr(pattern, '/');

	if (at && colon && at < colon)
		repository->host = copy_substring(at + 1, colon);

	if (colon && slash && colon < slash)
		repository->user = copy_substring(colon + 1, slash);

	if (slash) {
		char *end = strstr(slash, ".git");
		if (!end)
			end = pattern + strlen(pattern);
		repository->name = copy_substring(slash + 1, end);
	}
}

void
extract_repository_from_https_pattern(struct Repository *repository,
    char *pattern)
{
	repository->protocol = HTTPS;

	char *proto = strstr(pattern, "://");
	if (!proto)
		return;
	char *host_start = proto + 3;
	char *host_end = strchr(host_start, '/');
	if (!host_end)
		return;
	repository->host = copy_substring(host_start, host_end);

	char *user_start = host_end + 1;
	char *user_end = strchr(user_start, '/');
	if (!user_end)
		return;
	repository->user = copy_substring(user_start, user_end);

	char *name_start = user_end + 1;
	char *name_end = strstr(name_start, ".git");
	if (!name_end)
		name_end = pattern + strlen(pattern);
	repository->name = copy_substring(name_start, name_end);
}

void
overload_repository_with_pattern(struct Repository *repository, char *pattern)
{
	if (!repository || !pattern)
		return;

	char *end, *start;

	if (fnmatch("*/*", pattern, 0) == 0) {
		/* pattern has both user and repository name */
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
			if (repository->name == NULL)
				err(1, NULL);
		}
	} else {
		free(repository->name);
		end = strstr(pattern, ".git");
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

	if (!pattern)
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
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return repository;

	char *start = strstr(cwd, clone_path);
	if (!start)
		return repository;

	start += strlen(clone_path);
	if (*start == '/')
		start++;
	char *end = strchr(start, '/');
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
	if (!end)
		end = strstr(start, ".git");
	if (end) {
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
extract_ssh_url_from_repository(struct Repository repository)
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

char *
extract_https_url_from_repository(struct Repository repository)
{
	char format[17] = "https://%s/%s/%s";
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
