BRAT=	${.CURDIR}/vendor/brat
BRAT_URL=	https://codeberg.org/api/packages/sstephenson/generic/brat/latest/brat

BRAT_TESTS+=	smoke.brat

TESTS+=	clone-inside-project-base-directory.sh
TESTS+=	clone-outside-project-base-directory.sh
TESTS+=	clone-path-configuration.sh
TESTS+=	clone-rejects-bad-patterns.sh

all: test

test: ${TESTS:.sh=.fake}

.SUFFIXES: .sh .fake

.sh.fake:
	@ sh ${.CURDIR}/t.sh ${TESTFLAGS} ${.CURDIR}/test-helper.sh ${.CURDIR}/$<

fmt:
	shfmt -d -i 2 ${TESTS}

test-brat: ${BRAT}
	@ ${BRAT} ${TESTFLAGS} ${BRAT_TESTS:%=${.CURDIR}/%}

${BRAT}:
	mkdir -p ${.CURDIR}/vendor
	curl -sL ${BRAT_URL} -o $@
	chmod +x $@
