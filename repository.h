#ifndef _CLONE_REPOSITORY_H_
#define _CLONE_REPOSITORY_H_

#include "url.h"

struct Repository {
	char		*host;
	char		*user;
	char		*name;
	char		*login_user;
	char		*port;
	enum scheme	 scheme;
};

int			 extract_repository(const char *, const char *,
			    struct Repository *);
char			*extract_location_from_repository(const char *,
			    struct Repository);
char			*extract_url_from_repository(struct Repository);
int			 invalid_repository(struct Repository);
void			 free_repository(struct Repository *);

#endif /* _CLONE_REPOSITORY_H_ */
