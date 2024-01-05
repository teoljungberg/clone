TESTS+=	clone-inside-project-base-directory.sh
TESTS+=	clone-outside-project-base-directory.sh

all: test

test: ${TESTS:.sh=.fake}

.SUFFIXES: .sh .fake

.sh.fake:
	@ sh ${.CURDIR}/t.sh ${TESTFLAGS} ${.CURDIR}/test-helper.sh ${.CURDIR}/$<

fmt:
	shfmt -d -i 2 ${TESTS}
