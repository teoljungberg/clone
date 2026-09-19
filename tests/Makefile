BRAT=	${.CURDIR}/vendor/brat
BRAT_URL=	https://codeberg.org/api/packages/sstephenson/generic/brat/latest/brat

TESTS+=	smoke.brat
TESTS+=	clone-inside-project-base-directory.brat
TESTS+=	clone-outside-project-base-directory.brat
TESTS+=	clone-path-configuration.brat
TESTS+=	clone-rejects-bad-patterns.brat

all: test

test:
	@ ${BRAT} ${TESTFLAGS} ${TESTS:%=${.CURDIR}/%}

fmt:
	shfmt -d -i 2 ${TESTS}

${BRAT}:
	mkdir -p ${.CURDIR}/vendor
	curl -sL ${BRAT_URL} -o $@
	chmod +x $@
