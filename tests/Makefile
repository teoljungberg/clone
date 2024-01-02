PROJECT_ROOT := $(shell git rev-parse --show-toplevel)

TESTS+=	clone-inside-project-base-directory.sh
TESTS+=	clone-outside-project-base-directory.sh

all: test

test: ${TESTS:.sh=.fake}

.SUFFIXES: .sh .fake

.sh.fake:
	@ sh ${PROJECT_ROOT}/tests/t.sh ${TESTFLAGS} ${PROJECT_ROOT}/tests/test-helper.sh ${PROJECT_ROOT}/tests/$<

fmt:
	shfmt -w -i 2 ${TESTS}
