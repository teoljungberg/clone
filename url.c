#include "compat.h"
#include "url.h"

#include <err.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <string.h>

static int
parse_scp(const char *pattern, struct url *url)
{
	const char *at, *colon;

	at = strchr(pattern, '@');
	colon = strchr(pattern, ':');

	if (at == NULL || colon == NULL || at >= colon)
		return -1;

	url->scheme = SCHEME_SCP;

	url->user = strndup(pattern, at - pattern);
	if (url->user == NULL)
		err(1, NULL);

	url->host = strndup(at + 1, colon - (at + 1));
	if (url->host == NULL)
		err(1, NULL);

	url->path = strdup(colon + 1);
	if (url->path == NULL)
		err(1, NULL);

	return 0;
}

static int
parse_standard_url(const char *pattern, struct url *url, enum scheme scheme)
{
	const char *at, *colon, *host_end, *host_start, *proto;

	proto = strstr(pattern, "://");
	if (proto == NULL)
		return -1;

	host_start = proto + 3;
	host_end = strchr(host_start, '/');
	if (host_end == NULL)
		return -1;

	url->scheme = scheme;

	/* extract user from user@host if present */
	at = memchr(host_start, '@', host_end - host_start);
	if (at != NULL) {
		url->user = strndup(host_start, at - host_start);
		if (url->user == NULL)
			err(1, NULL);
		host_start = at + 1;
	}

	/* extract port from host:port if present */
	colon = memchr(host_start, ':', host_end - host_start);
	if (colon != NULL) {
		url->port = strndup(colon + 1, host_end - (colon + 1));
		if (url->port == NULL)
			err(1, NULL);
		url->host = strndup(host_start, colon - host_start);
	} else {
		url->host = strndup(host_start, host_end - host_start);
	}
	if (url->host == NULL)
		err(1, NULL);

	url->path = strdup(host_end + 1);
	if (url->path == NULL)
		err(1, NULL);

	return 0;
}

static const struct {
	const char *prefix;
	size_t      len;
	enum scheme scheme;
} protocols[] = {
	{ "https://", 8, SCHEME_HTTPS },
	{ "git://",   6, SCHEME_GIT   },
	{ "ssh://",   6, SCHEME_SSH   },
	{ "http://",  7, SCHEME_HTTP  },
	{ "ftps://",  7, SCHEME_FTP   },
	{ "ftp://",   6, SCHEME_FTP   },
};

int
parse_url(const char *pattern, struct url *url)
{
	size_t i;

	if (pattern == NULL || url == NULL)
		return -1;

	memset(url, 0, sizeof(*url));
	url->scheme = SCHEME_UNDEFINED;

	for (i = 0; i < nitems(protocols); i++) {
		if (strncmp(pattern, protocols[i].prefix, protocols[i].len) == 0)
			return parse_standard_url(pattern, url, protocols[i].scheme);
	}

	if (fnmatch("*@*:*/*", pattern, 0) == 0)
		return parse_scp(pattern, url);

	return -1;
}

int
invalid_scheme(enum scheme scheme)
{
	return scheme == SCHEME_GIT ||
	    scheme == SCHEME_HTTP ||
	    scheme == SCHEME_FTP;
}

void
free_url(struct url *url)
{
	free(url->user);
	free(url->host);
	free(url->port);
	free(url->path);
	url->user = NULL;
	url->host = NULL;
	url->port = NULL;
	url->path = NULL;
	url->scheme = SCHEME_UNDEFINED;
}
