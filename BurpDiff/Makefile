EXECS=BurpDiff
MPIXX?=/usr/bin/mpicxx

all: ${EXECS}

${EXECS}: BurpDiff.cpp
	${MPIXX} -o ${EXECS} BurpDiff.cpp

clean:
	rm -f ${EXECS}
