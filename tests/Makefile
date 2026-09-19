BRAT=	${.CURDIR}/vendor/brat/bin/brat
BRAT_URL=	https://codeberg.org/sstephenson/brat.git

TESTS+=	clone-inside-project-base-directory.brat
TESTS+=	clone-outside-project-base-directory.brat
TESTS+=	clone-path-configuration.brat
TESTS+=	clone-rejects-bad-patterns.brat

all: test

test: ${BRAT}
	@ ${BRAT} ${TESTFLAGS} ${TESTS:%=${.CURDIR}/%}

${BRAT}:
	mkdir -p ${.CURDIR}/vendor
	git clone ${BRAT_URL} ${.CURDIR}/vendor/brat
