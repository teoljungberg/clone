#ifndef _CLONE_REPOSITORY_H_
#define _CLONE_REPOSITORY_H_

#include <fnmatch.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "url.h"

struct Repository {
	char		*host;
	char		*user;
	char		*name;
	char		*login_user;
	enum scheme	 scheme;
};

struct Repository	 extract_repository_from_url(const struct url *);
struct Repository	 extract_repository_from_cwd(const char *, const char *);
void			 overload_repository_with_pattern(struct Repository *,
			    const char *);
char			*extract_location_from_repository(const char *,
			    struct Repository);
char			*extract_url_from_repository(struct Repository);
void			 free_repository(struct Repository *);

#endif /* _CLONE_REPOSITORY_H_ */
