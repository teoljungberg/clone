#ifndef _CLONE_URL_H_
#define _CLONE_URL_H_

enum scheme {
	SCHEME_UNDEFINED,

	/* Supported */
	SCHEME_SCP,	/* [user@]host:path */
	SCHEME_SSH,	/* ssh://[user@]host/path */
	SCHEME_HTTPS,	/* https://host/path */

	/* Unsupported: no authentication, transmitted in cleartext */
	SCHEME_GIT,	/* git://host/path */
	SCHEME_HTTP,	/* http://host/path */
	SCHEME_FTP,	/* ftp[s]://host/path */
};

struct url {
	enum scheme	 scheme;
	char		*user;
	char		*host;
	char		*port;
	char		*path;
};

int	 parse_url(const char *, struct url *);
void	 free_url(struct url *);

#endif /* _CLONE_URL_H_ */
