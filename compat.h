#ifndef _CLONE_COMPAT_H_
#define _CLONE_COMPAT_H_

#ifndef __dead
#define __dead __attribute__((__noreturn__))
#endif

#ifndef nitems
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#ifndef __OpenBSD__
#define pledge(promises, execpromises) (0)
#endif

#endif
