VERSION=	0.0.1
CLONE_PATH?=	~/src

PROG=	clone

SRCS=	clone.c
SRCS+=	repository.c

OBJS=		${SRCS:.c=.o}

PREFIX=		/usr/local
MANPREFIX=	${PREFIX}/share/man

INSTALL_PROGRAM=	install -s -m 0755
INSTALL_MAN=		install -m 0644

CFLAGS+=	-pedantic -Wall -Wextra
CFLAGS+=	-DVERSION=\"${VERSION}\"
CFLAGS+=	-DCLONE_PATH=\"${CLONE_PATH}\"
# Uncomment to enable debugging
# CFLAGS+=	-g -DDEBUG=1

all: ${PROG}

${PROG}: ${OBJS}
	${CC} -o ${PROG} ${OBJS} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}
.PHONY: clean

install: ${PROG}
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	${INSTALL_PROGRAM} ${PROG} ${DESTDIR}${PREFIX}/bin
	${INSTALL_MAN} ${PROG}.1 ${DESTDIR}${MANPREFIX}/man1
.PHONY: install

lint:
	mandoc -Tlint -Wstyle ${PROG}.1
	knfmt -ds ${SRCS}
.PHONY: lint

fmt:
	knfmt -is ${SRCS}
	${MAKE} -C ${.CURDIR}/tests fmt
.PHONY: fmt

test: ${PROG}
	${MAKE} -C ${.CURDIR}/tests
.PHONY: test
