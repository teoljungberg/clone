VERSION=	0.0.1

PROG=	clone
OBJS=	clone.o

CFLAGS+=	-pedantic -Wall -Wextra
CFLAGS+=	-DVERSION=\"${VERSION}\"

all: ${PROG}

${PROG}: ${OBJS}
	${CC} -o ${PROG} ${OBJS} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}
.PHONY: clean

fmt:
	knfmt -i ${PROG}.c
	${MAKE} -C tests fmt
.PHONY: fmt

test: ${PROG}
	$(MAKE) -C tests
.PHONY: test
